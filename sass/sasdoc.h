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


    class SasDoc : public Dataset
    {
    public:
        SasDoc();
        ~SasDoc() override = default;
        void addRow(const Row& row) override {
            // Possibly convert row to readstat logic or store in "values" array
            // or call the base if we want to keep normal row-based storage
            Dataset::addRow(row);
        }
        static int handle_metadata(readstat_metadata_t* metadata, void* ctx);
        static int handle_metadata_xpt(readstat_metadata_t* metadata, void* ctx);
        static int handle_variable(int index, readstat_variable_t* variable, const char* val_labels, void* ctx);
        static int handle_value(int obs_index, readstat_variable_t* variable, readstat_value_t value, void* ctx);
        static int read_sas7bdat(std::wstring path, void* user_ctx);
        /* A callback for writing bytes to your file descriptor of choice */
        /* The ctx argument comes from the readstat_begin_writing_xxx function */
        static ssize_t write_bytes(const void* data, size_t len, void* ctx) {
            int fd = *(int*)ctx;
            return write(fd, data, len);
        }

        static int write_sas7bdat(std::wstring path, SasDoc* ds);
        // todo
        static formatrec loadSASFormat(string formatName, SasDoc* data01);
        string Format(double value01, string aFormat, int w, int d);
        string Format(tchar* value, string aFormat, int w, int d);
        // virtual void DeleteContents();
        int GetVarNo(std::wstring varName);
        // static string Utf8ToCString(const char *utf8Str);

        string GetCellText(int row, int col);
        string getCellTextVariable(int row, int column);

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

        int obs_count;
        int var_count;
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

        string get_value_string(int row, int col) {
            return std::get<flyweight_string>(this->values[row * this->var_count + col]);
        }

        double get_value_double(int row, int col) {
            return std::get<double>(this->values[row * this->var_count + col]);
        }

    };

}


#endif // SASDOC_H

