#include "sasdoc.h"
#include "sasdoc.h"
#include "sasdoc.h"
#include "sasdoc.h"
#include <ReadStat/readstat.h>
#include <cmath>
#include <cstddef>
#include <string>
#include <map>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <fcntl.h>

using tchar = unsigned char;

using namespace std;

namespace sass {
	string tail(string const& source, size_t const length)
	{
		if (length >= source.size())
		{
			return source;
		}
		return source.substr(source.size() - length);
	} // tail

	const int daysOffset = 3653;

	// SasDoc construction/destruction

	SasDoc::SasDoc()
	{
	}

	// SasDoc commands

	int SasDoc::handle_metadata_load(readstat_metadata_t* metadata, void* ctx)
	{
		int var_count, obs_count, strLen;
		SasDoc* data01 = (SasDoc*)ctx;

		var_count = readstat_get_var_count(metadata);
		obs_count = readstat_get_row_count(metadata);

		data01->creation_time = readstat_get_creation_time(metadata);
		data01->modified_time = readstat_get_modified_time(metadata);
		data01->file_format_version = readstat_get_file_format_version(metadata);
		data01->compression = readstat_get_compression(metadata);
		data01->endianness = readstat_get_endianness(metadata);
		const char* s;

		data01->file_name = readstat_get_table_name(metadata);
		data01->file_label = readstat_get_file_label(metadata);
		data01->file_encoding = readstat_get_file_encoding(metadata);

		data01->is64bit = readstat_get_file_format_is_64bit(metadata);
		data01->columns.resize(var_count);

		if (data01->parseValue)
		{
			data01->rows.resize(obs_count);
		}

		return READSTAT_HANDLER_OK;
	}

	int SasDoc::handle_metadata_xpt(readstat_metadata_t* metadata, void* ctx)
	{
		SasDoc* data01 = (SasDoc*)ctx;
		int obs_count = readstat_get_row_count(metadata);

		if (obs_count == -1)
		{
			data01->creation_time = readstat_get_creation_time(metadata);
			data01->modified_time = readstat_get_modified_time(metadata);
			data01->file_format_version = readstat_get_file_format_version(metadata);
			data01->compression = readstat_get_compression(metadata);
			data01->endianness = readstat_get_endianness(metadata);

			data01->file_name = readstat_get_table_name(metadata);
			data01->file_label = readstat_get_file_label(metadata);
			data01->file_info = "";
			data01->file_encoding = "";
			data01->host = "";
			data01->release = "";

			int var_count = readstat_get_var_count(metadata);
			data01->var_names.resize(var_count);
			data01->var_labels.resize(var_count);
			data01->var_formats.resize(var_count);
			data01->var_types.resize(var_count);
			data01->var_length.resize(var_count);
			data01->var_display_length.resize(var_count);
			data01->var_decimals.resize(var_count);
		}
		else if (data01->getRowCount() == 0)
		{
			if (data01->parseValue)
			{
				data01->values.resize(data01->getColumnCount() * obs_count);
			}
		}


		return READSTAT_HANDLER_OK;
	}

	int SasDoc::handle_variable_load(int index, readstat_variable_t* variable, const char* val_labels, void* ctx)
	{
		SasDoc* data01 = (SasDoc*)ctx;

		VariableDef varDef;
		varDef.name = readstat_variable_get_name(variable);
		const char* pTemp = readstat_variable_get_label(variable);
		varDef.label = pTemp != NULL ? pTemp : "";
		pTemp = readstat_variable_get_format(variable);
		varDef.format = pTemp != NULL ? pTemp : "";
		varDef.type = variable->type;
		varDef.length = variable->storage_width;
		varDef.display_length = variable->display_width;
		varDef.decimals = variable->decimals;

		data01->columns[index] = varDef;

		return 0;
	}

	int SasDoc::handle_value_load(int obs_index, readstat_variable_t* variable, readstat_value_t value, void* ctx)
	{
		SasDoc* data01 = (SasDoc*)ctx;
		readstat_type_t type = readstat_value_type(value);

		Row& row = data01->rows[obs_index];
		if (!readstat_value_is_missing(value, variable))
		{
			switch (type) {
			case READSTAT_TYPE_STRING:
				row.columns[variable->name] = readstat_string_value(value);
				break;
			case READSTAT_TYPE_DOUBLE:
				row.columns[variable->name] = (double)readstat_double_value(value);
				break;
			case READSTAT_TYPE_INT8:
				row.columns[variable->name] = (double)readstat_int8_value(value);
				break;
			case READSTAT_TYPE_INT16:
				row.columns[variable->name] = (double)readstat_int16_value(value);
				break;
			case READSTAT_TYPE_INT32:
				row.columns[variable->name] = (double)readstat_int32_value(value);
				break;
			case READSTAT_TYPE_FLOAT:
				row.columns[variable->name] = (double)readstat_float_value(value);
				break;
			default:
				break;
			}
		}
		else {
			if (type == READSTAT_TYPE_DOUBLE)
			{
				row.columns[variable->name] = -INFINITY;
			}
		}

		return READSTAT_HANDLER_OK;
	}

	int SasDoc::load(wstring path)
	{
		readstat_error_t error = READSTAT_OK;
		readstat_parser_t* parser = readstat_parser_init();
		readstat_set_variable_handler(parser, &handle_variable_load);
		if (this->parseValue)
		{
			readstat_set_value_handler(parser, &handle_value_load);
		}

		string filename = string(path.begin(), path.end());
		string right9 = tail(filename, 9);
		boost::algorithm::to_lower(right9);

		string right4 = tail(filename, 4);
		boost::algorithm::to_lower(right4);

		if (right9 == ".sas7bdat")
		{
			readstat_set_metadata_handler(parser, &handle_metadata_load);
			wstring temp{ filename.begin(), filename.end() };
			error = readstat_parse_sas7bdat(parser, temp.c_str(), this);
		}
		else if (right4 == ".xpt")
		{
			readstat_set_metadata_handler(parser, &handle_metadata_xpt);
			wstring temp{ filename.begin(), filename.end() };
			error = readstat_parse_xport(parser, temp.c_str(), this);
		}

		if (error != READSTAT_OK)
		{
			printf("Error processing %s: %d\n", path, error);
			return error;
		}

		readstat_parser_free(parser);

		return 0;
	}


	static readstat_type_t toReadStatType(int var_type) {
		// You store raw numeric IDs from ReadStat. 
		// For simplicity, map them to READSTAT_TYPE_DOUBLE or READSTAT_TYPE_STRING, etc.
		// If you already have them in var_types, you can directly cast. Example:
		switch (var_type) {
		case READSTAT_TYPE_STRING:
			return READSTAT_TYPE_STRING;
		case READSTAT_TYPE_DOUBLE:
		case READSTAT_TYPE_FLOAT:
		case READSTAT_TYPE_INT8:
		case READSTAT_TYPE_INT16:
		case READSTAT_TYPE_INT32:
			return READSTAT_TYPE_DOUBLE;  // we'll write them as double
		default:
			return READSTAT_TYPE_STRING;  // fallback
		}
	}

	int SasDoc::save(std::wstring path)
	{
		// 1) Convert wstring -> narrow string
		std::string path_utf8 = std::string(path.begin(), path.end());

		// 2) Open file descriptor
		int fd = open(path_utf8.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_BINARY
#ifdef _WIN32
			, _S_IREAD | _S_IWRITE
#endif
		);
		if (fd < 0) {
			std::cerr << "Cannot open file for writing: " << path_utf8 << std::endl;
			return -1;
		}

		// 3) Initialize the readstat writer
		readstat_writer_t* writer = readstat_writer_init();
		if (!writer) {
			std::cerr << "Failed to init readstat_writer." << std::endl;
			close(fd);
			return -2;
		}

		readstat_set_data_writer(writer, &write_bytes);

		// for demonstration, set some metadata
		readstat_writer_set_file_label(writer, this->file_label.c_str());
		// readstat_writer_set_file_format_version(writer, 5);
		// readstat_writer_set_file_encoding(writer, "UTF-8");
		// etc. as needed

		// 4) Add variables
		std::vector<readstat_variable_t*> varHandles;
		for (auto item : columns) {
			readstat_type_t rsType = toReadStatType(item.type);
			// storage width: for numeric, say 8; for string, doc->var_length[i]
			int col_width = (rsType == READSTAT_TYPE_STRING) ? item.length : 8;
			// Add variable
			readstat_variable_t* var = readstat_add_variable(
				writer, item.name.c_str(), rsType, col_width);

			// If you have labels or formats:
			if (!item.label.empty()) {
				readstat_variable_set_label(var, item.label.c_str());
			}
			if (!item.format.empty()) {
				readstat_variable_set_format(var, item.format.c_str());
			}
			// decimals => can set display format or ...
			// readstat_variable_set_alignment(var, READSTAT_ALIGNMENT_RIGHT);

			varHandles.push_back(var);
		}

		// 5) Begin writing for row_count
		int row_count = getRowCount();
		readstat_error_t rc = readstat_begin_writing_sas7bdat(
			writer, &fd, row_count);
		if (rc != READSTAT_OK) {
			std::cerr << "readstat_begin_writing_sas7bdat failed with code " << rc << std::endl;
			readstat_writer_free(writer);
			close(fd);
			return -3;
		}

		// 6) Write each row
		for (auto row : rows) {
			readstat_begin_row(writer);
			for (auto col : varHandles) {
				readstat_type_t rsType = toReadStatType(col->type);
				Value value = row.columns[col->name];
				if (rsType == READSTAT_TYPE_STRING) {
					if (std::holds_alternative<string>(value)) {
						std::string val = std::get<string>(value);
						readstat_insert_string_value(writer, col, val.c_str());
					}
					else {
						readstat_insert_missing_value(writer, col);
					}
				}
				else {
					if (std::holds_alternative<double>(value)) {
						double val = std::get<double>(value);
						// If val == -INFINITY, treat as missing
						if (std::isinf(val)) {
							readstat_insert_missing_value(writer, col);
						}
						else {
							readstat_insert_double_value(writer, col, val);
						}
					}
					else {
						readstat_insert_missing_value(writer, col);
					}
				}
			}
			readstat_end_row(writer);
		}

		// 7) End writing
		rc = readstat_end_writing(writer);
		if (rc != READSTAT_OK) {
			std::cerr << "readstat_end_writing failed with code " << rc << std::endl;
		}

		// 8) Cleanup
		readstat_writer_free(writer);
		close(fd);

		return 0;
	}


	formatrec SasDoc::loadSASFormat(string formatName, SasDoc* data01)
	{
		formatrec formatRec01;
		//FATTRSTR fAttrStr01;
		//char *chPtr;
		//int iRC = xfname(formatName.data(), formatName.size(), 0x45, &fAttrStr01, &chPtr);

		//if (iRC == 0)
		//{
		//    formatRec01.code = chPtr;
		//    formatRec01.attrs = fAttrStr01;
		//    data01->mapFormat.insert(std::pair<std::string, formatrec>(formatName, formatRec01));
		//}
		//else
		//{
		//    formatRec01.code = NULL;
		//}

		return formatRec01;
	}

	string SasDoc::Format(double value, string aFormat, int w, int d)
	{
		if (value == -INFINITY)
		{
			return std::string(".");
		}

		if (aFormat.length() == 0)
		{
			if (w != 0)
				aFormat = "F";
			else
				aFormat = "BEST";
		}

		formatrec formatRec01;

		std::map<std::string, formatrec>::iterator it = mapFormat.find(aFormat);
		if (it == mapFormat.end())
		{
			formatRec01 = loadSASFormat(aFormat, this);
		}
		else
		{
			formatRec01.code = it->second.code;
			formatRec01.attrs = it->second.attrs;
		}

		if (formatRec01.code == NULL)
		{
			formatRec01 = loadSASFormat("BEST", this);
		}

		// todo format

		return std::to_string(value);
	}

	std::string SasDoc::Format(tchar* value, std::string aFormat, int w, int d)
	{
		if (aFormat.length() == 0)
			return (char*)value;

		formatrec formatRec01;
		std::map<std::string, formatrec>::iterator it = mapFormat.find(aFormat);
		if (it == mapFormat.end())
		{
			formatRec01 = loadSASFormat(aFormat, this);
		}
		else
		{
			formatRec01.code = it->second.code;
			formatRec01.attrs = it->second.attrs;
		}

		if (formatRec01.code == NULL)
			return (char*)value;

		// todo format

		return (char*)value;
	}

	string GetVariableFormat(SasDoc* pDoc, int row)
	{
		string sFormat = pDoc->var_formats[row];

		if (pDoc->var_display_length[row] > 0)
		{
			sFormat = sFormat + to_string(pDoc->var_display_length[row]);
		}

		if (!sFormat.empty())
		{
			sFormat = sFormat + ".";
		}

		if (pDoc->var_decimals[row] > 0)
		{
			sFormat = sFormat + to_string(pDoc->var_decimals[row]);
		}

		return sFormat;
	}

	string GetVariableInformat(SasDoc* pDoc, int row)
	{
		if (pDoc->var_formats[row] == "$" || pDoc->var_formats[row] == "BEST")
		{
			return GetVariableFormat(pDoc, row);
		}
		else
		{
			return "";
		}
	}



	string SasDoc::getCellTextVariable(int row, int column)
	{
		string label;
		switch (column)
		{
		case 1:
			label = var_names[row];
			break;
		case 2:
			label = (var_types[row] == READSTAT_TYPE_DOUBLE ? "Numeric" : "Character");
			break;
		case 3:
			label = to_string(var_length[row]);
			break;
		case 4:
			label = GetVariableFormat(this, row);
			break;
		case 5:
			label = GetVariableInformat(this, row);
			break;
		case 6:
			label = var_labels[row];
			break;

		default:
			break;
		}
		return label;
	}

	ColProxy SasDoc::getColProxy(int col) {
		// range check
		if (col < 0 || col >= getColumnCount()) {
			throw std::out_of_range("Column index out of range");
		}
		return ColProxy(*this, col);
	}

	int SasDoc::findColumn(const std::string& varName) const {
		auto it = std::find(var_names.begin(), var_names.end(), varName);
		if (it == var_names.end()) {
			throw std::runtime_error("Column not found: " + varName);
		}
		return (int)(it - var_names.begin());
	}
}
