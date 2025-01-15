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
        bool parseValue = true;

        std::map<string, formatrec> mapFormat;

        /**
         * Copies all the fields from `src` into `dest` except the `dest->name`.
         * This includes var_count, obs_count, var_names, var_types, var_labels,
         * var_formats, var_length, var_display_length, var_decimals, and values.
         *
         * If `src` is null or `dest` is null, does nothing.
         */
        void copyFrom(Dataset* src) {
            if (!src) {
                return;
            }

            auto srcSasDoc = dynamic_cast<SasDoc*>(src);
            if (srcSasDoc)
            {
                // We do NOT overwrite `this->name`.
                // We DO copy everything else:
                this->creation_time = srcSasDoc->creation_time;
                this->modified_time = srcSasDoc->modified_time;
                this->file_format_version = srcSasDoc->file_format_version;
                this->compression = srcSasDoc->compression;
                this->endianness = srcSasDoc->endianness;
                this->file_label = srcSasDoc->file_label;
                this->file_info = srcSasDoc->file_info;
                this->file_encoding = srcSasDoc->file_encoding;
                this->is64bit = srcSasDoc->is64bit;
                this->release = srcSasDoc->release;
                this->host = srcSasDoc->host;
                this->page_size = srcSasDoc->page_size;
                this->page_count = srcSasDoc->page_count;
                this->row_length = srcSasDoc->row_length;
                this->file_size = srcSasDoc->file_size;
                this->parseValue = srcSasDoc->parseValue;
                this->mapFormat = srcSasDoc->mapFormat;
                this->columns = srcSasDoc->columns;
                this->rows = srcSasDoc->rows;
            }

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

