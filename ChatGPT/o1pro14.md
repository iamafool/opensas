Below is an **expanded** example showing **how** you can update your **SasDoc** (or any `Dataset` descendant) **on-the-fly** whenever new variables are created in the PDV. In other words, if a user¡¯s DATA step code creates a **new variable** via `AssignmentNode` that didn¡¯t exist in the input dataset (or initial `var_names`), we need to add that variable metadata to `SasDoc` *before* we copy the row into `SasDoc->values`.

This **resolves** the problem that **`appendPdvRowToSasDoc`** only loops over `doc->var_names`, ignoring newly introduced variables. Now, newly introduced variables in the PDV also become new columns in the `SasDoc`.

---

## 1. Where the Issue Arises

1. The user code (DATA step) might do:
   ```sas
   data out;
     set in;
     newVar = oldVar + 1;
     output;
   run;
   ```
   If `newVar` **wasn¡¯t** in the input dataset, it¡¯s created in the PDV but **not** in `doc->var_names`.  
2. Then `appendPdvRowToSasDoc` only copies variables it sees in `doc->var_names`.  
3. As a result, `newVar` is never added to the final dataset¡¯s columns.

We **fix** it by:

- Checking the **PDV** for any variables not in `SasDoc->var_names`.  
- Adding them to `SasDoc->var_names`, `SasDoc->var_types`, increment `var_count`, and **expand** the `values` array to handle the new column.

---

## 2. A ¡°Sync PDV to SasDoc¡± Helper

Add a function like `syncPdvColumnsToSasDoc(...)` that:

1. **Scans** through each variable in `pdv.pdvVars`.
2. **Checks** if that variable is in `doc->var_names`.
3. If not, we **append** it to `doc->var_names`, adjust `doc->var_types`, and update `doc->var_count`.  
4. We must also expand `doc->values` to hold the new column. That means for each existing row, we must insert a placeholder cell.  

**Example**:

```cpp
void Interpreter::syncPdvColumnsToSasDoc(PDV &pdv, SasDoc* doc)
{
    // For each PdvVar in pdv, see if doc->var_names already has it
    for (size_t i = 0; i < pdv.pdvVars.size(); i++) {
        const std::string& pdvVarName = pdv.pdvVars[i].name;
        // search doc->var_names
        auto it = std::find(doc->var_names.begin(), doc->var_names.end(), pdvVarName);
        if (it == doc->var_names.end()) {
            // We have a new column
            doc->var_names.push_back(pdvVarName);
            // guess var_types: if isNumeric => READSTAT_TYPE_DOUBLE else READSTAT_TYPE_STRING
            if (pdv.pdvVars[i].isNumeric) {
                doc->var_types.push_back(READSTAT_TYPE_DOUBLE);
            } else {
                doc->var_types.push_back(READSTAT_TYPE_STRING);
            }

            // Optionally define var_labels, var_formats, var_length etc. For now we do minimal:
            doc->var_labels.push_back("");
            doc->var_formats.push_back("");
            doc->var_length.push_back(pdv.pdvVars[i].length <= 0 ? 8 : pdv.pdvVars[i].length);
            doc->var_display_length.push_back(8); // arbitrary
            doc->var_decimals.push_back(0);

            // Increase var_count
            doc->var_count = (int)doc->var_names.size();

            // Now we must expand doc->values to accommodate this new column.
            // For each existing row in doc->obs_count, we insert a missing placeholder
            // typically -INFINITY for numeric or "" for string
            int oldRowCount = doc->obs_count;
            if (oldRowCount > 0) {
                // The old doc->values size was var_count-1 * oldRowCount, we just incremented var_count
                // So we re-allocate doc->values to the new size
                // we must do it carefully: we had old size = (var_count-1) * oldRowCount
                // new size = var_count * oldRowCount
                // We'll copy from old array to new array
                // but a simpler approach is to do it row by row
                int newVarCount = doc->var_count; // after increment
                std::vector<Cell> oldValues = doc->values;
                doc->values.clear();
                doc->values.resize(newVarCount * oldRowCount);

                // row by row copy
                for (int r = 0; r < oldRowCount; r++) {
                    // copy old row
                    for (int c = 0; c < newVarCount - 1; c++) {
                        // the old column c in that row used to be old row index = r*(var_count-1) + c
                        doc->values[r * newVarCount + c] = oldValues[r*(newVarCount - 1) + c];
                    }
                    // fill the new column with missing
                    if (pdv.pdvVars[i].isNumeric) {
                        doc->values[r * newVarCount + (newVarCount - 1)] = double(-INFINITY);
                    } else {
                        doc->values[r * newVarCount + (newVarCount - 1)] = flyweight_string("");
                    }
                }
            }
            else {
                // If there are no existing rows, nothing to do except ensure doc->values is correct size
                // doc->obs_count=0 => doc->values is 0 sized anyway
            }
        }
    }
}
```

### Explanation

- We loop over **all** PDV variables.  
- If the PDV variable is missing in `doc->var_names`, we **append** it. Then we update `doc->var_count`.  
- We **expand** `doc->values` so each existing row gets a missing placeholder for that new variable.

**Important**: We only do this expansion **once** per newly discovered variable, so you typically call `syncPdvColumnsToSasDoc` before you call `appendPdvRowToSasDoc`.

---

## 3. Updated `appendPdvRowToSasDoc`

Now, **before** we do the final copy of PDV => row in `SasDoc`, we do:

```cpp
void Interpreter::appendPdvRowToSasDoc(PDV &pdv, SasDoc* doc)
{
    // Step 1: sync new PDV columns to doc
    syncPdvColumnsToSasDoc(pdv, doc);

    // Step 2: now doc->var_count, doc->var_names, etc. is up-to-date
    int outRowIndex = doc->obs_count;
    doc->obs_count++;
    doc->values.resize(doc->var_count * doc->obs_count);

    // For each column in doc->var_names
    for (int c = 0; c < doc->var_count; c++) {
        const std::string& varName = doc->var_names[c];
        int pdvIndex = pdv.findVarIndex(varName);
        if (pdvIndex >= 0) {
            doc->values[outRowIndex * doc->var_count + c] = pdv.getValue(pdvIndex);
        } else {
            // missing
            if (doc->var_types[c] == READSTAT_TYPE_STRING) {
                doc->values[outRowIndex * doc->var_count