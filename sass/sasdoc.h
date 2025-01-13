#ifndef SASDOC_H
#define SASDOC_H

#include "readstat/readstat.h"
#include "readstat/sas/readstat_sas.h"
#include <string>
#include <map>
#include <boost/dynamic_bitset.hpp>
#include <boost/flyweight.hpp>
#include <vector>
#include "Dataset.h"
#include "include/unistd.h"
#include "utility.h"

using std::string;
using std::vector;

namespace sass {
    using tchar = unsigned char;

    // Define a flyweight string type
    using flyweight_string = boost::flyweight<std::string>;

    // Define a variant type that can hold either a flyweight string or a double
    using Cell = std::variant<flyweight_string, double>;

    struct FATTRSTR {
        short        fwmin;         /* minimum length                  */
        short        fwmax;         /* maximum length                  */
        short        fwdef;         /* default length                  */
        short        fjust;         /* justification: 1=left 0=right   */
        short        fdmin;         /* minimum no. of decimals         */
        short        fdmax;         /* maximum no. of decimals         */
        short        fddef;         /* default no. of decimals         */
        short        fmdef;         /* default modifier                */
    };

    struct formatrec
    {
        char* code;
        FATTRSTR attrs;
    };

    class RowProxy;
    class ColProxy;

    class SasDoc : public Dataset
    {
    public:
        SasDoc();
        ~SasDoc() override = default;
        void addRow(const Row& row) override {
            // Possibly convert row to readstat logic or store in "values" array
            // or call the base if we want to keep normal row-based storage
            // todo
        }
        std::vector<VariableDef> getColumns() const;
        // Implementation of virtual method from Dataset
        std::vector<std::string> getColumnNames() const;

        // For col-based iteration
        ColProxy getColProxy(int col) override;

        static int handle_metadata_load(readstat_metadata_t* metadata, void* ctx);
        static int handle_metadata_xpt(readstat_metadata_t* metadata, void* ctx);
        static int handle_variable_load(int index, readstat_variable_t* variable, const char* val_labels, void* ctx);
        static int handle_value_load(int obs_index, readstat_variable_t* variable, readstat_value_t value, void* ctx);
        int load(std::wstring path) override;
        int save(std::wstring path) override;
        /* A callback for writing bytes to your file descriptor of choice */
        /* The ctx argument comes from the readstat_begin_writing_xxx function */
        static ssize_t write_bytes(const void* data, size_t len, void* ctx) {
            int fd = *(int*)ctx;
            return write(fd, data, len);
        }

        // todo
        static formatrec loadSASFormat(string formatName, SasDoc* data01);
        string Format(double value01, string aFormat, int w, int d);
        string Format(tchar* value, string aFormat, int w, int d);
        // virtual void DeleteContents();
        // static string Utf8ToCString(const char *utf8Str);

        string getCellTextVariable(int row, int column);

        int findColumn(const std::string& varName) const;
        // Attributes
    public:
        time_t creation_time;
        time_t modified_time;
        int64_t file_format_version;
        readstat_compress_t compression;
        readstat_endian_t endianness;
        string file_name;
        string file_label;
        string file_info;
        string file_encoding;
        unsigned int is64bit : 1;
        string release;
        string host;
        int64_t page_size;
        int64_t page_count;
        uint32_t row_length;
        int64_t file_size;

        // char **var_names;
        vector<string> var_names;
        vector<string> var_labels;
        vector<string> var_formats;
        vector<int> var_types;
        vector<int> var_length;
        vector<int> var_display_length;
        vector<int> var_decimals;
        vector<Cell> values;
        boost::dynamic_bitset<> obs_flag;
        boost::dynamic_bitset<> or_flag;
        boost::dynamic_bitset<> obs_library_filter;
        boost::dynamic_bitset<> var_flag;
        bool parseValue = true;

        std::map<string, formatrec> mapFormat;

        /**
         * Copies all the fields from `src` into `dest` except the `dest->name`.
         * This includes var_count, obs_count, var_names, var_types, var_labels,
         * var_formats, var_length, var_display_length, var_decimals, and values.
         *
         * If `src` is null or `dest` is null, does nothing.
         */
        static void copySasDocExceptName(const SasDoc* src, SasDoc* dest) {
            if (!src || !dest) {
                return;
            }

            // We do NOT overwrite `dest->name`.
            // We DO copy everything else:
            dest->creation_time = src->creation_time;
            dest->modified_time = src->modified_time;
            dest->file_format_version = src->file_format_version;
            dest->compression = src->compression;
            dest->endianness = src->endianness;
            // do not copy name:
            // dest->name = src->name; // <--- intentionally omitted
            dest->file_label = src->file_label;
            dest->file_info = src->file_info;
            dest->file_encoding = src->file_encoding;
            dest->is64bit = src->is64bit;
            dest->release = src->release;
            dest->host = src->host;
            dest->page_size = src->page_size;
            dest->page_count = src->page_count;
            dest->row_length = src->row_length;
            dest->file_size = src->file_size;

            // Copy the vectors:
            dest->var_names = src->var_names;
            dest->var_labels = src->var_labels;
            dest->var_formats = src->var_formats;
            dest->var_types = src->var_types;
            dest->var_length = src->var_length;
            dest->var_display_length = src->var_display_length;
            dest->var_decimals = src->var_decimals;

            // Copy the main `values` vector:
            // This is typically a "shallow" copy in terms of the vector data,
            // but each element is a variant<flyweight_string, double>.
            // In C++, copying a std::vector<Cell> *does* copy all elements. That
            // is effectively a ¡°deep copy¡± of the vector memory. However, the
            // `flyweight_string` inside might share the underlying string data
            // with the source. That is usually OK, unless you want total isolation.
            dest->values = src->values;

            // Copy bitsets:
            dest->obs_flag = src->obs_flag;
            dest->or_flag = src->or_flag;
            dest->obs_library_filter = src->obs_library_filter;
            dest->var_flag = src->var_flag;

            // Copy parseValue, mapFormat, etc.
            dest->parseValue = src->parseValue;
            dest->mapFormat = src->mapFormat;
        }

    };

    class ColProxy {
        public:
            ColProxy(SasDoc& doc_, int col_) : doc(doc_), col(col_) {}

            // Return the cell at row r
            Cell getCell(int r) const {
                size_t idx = static_cast<size_t>(r) * doc.getColumnCount() + col;
                return doc.values[idx];
            }

            // If you want to modify:
            void setCell(int r, const Cell& val) {
                size_t idx = static_cast<size_t>(r) * doc.getColumnCount() + col;
                doc.values[idx] = val;
            }

            // Possibly an operator[] for convenience
            Cell operator[](int r) const {
                return getCell(r);
            }
            Cell& operator[](int r) {
                size_t idx = static_cast<size_t>(r) * doc.getColumnCount() + col;
                return doc.values[idx];
            }

            // (Optional) Return column index or name:
            int getColIndex() const { return col; }
            std::string getColName() const {
                if (col >= 0 && col < (int)doc.var_names.size()) {
                    return doc.var_names[col];
                }
                return "";
            }

        private:
            SasDoc& doc;
            int col;
    };


}


#endif // SASDOC_H

