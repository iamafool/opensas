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
		obs_count = 0;
		var_count = 0;
	}

	// SasDoc commands

	int SasDoc::handle_metadata(readstat_metadata_t* metadata, void* ctx)
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
		data01->obs_count = obs_count;
		data01->var_count = var_count;
		data01->var_names.resize(var_count);
		data01->var_labels.resize(var_count);
		data01->var_formats.resize(var_count);
		data01->var_types.resize(var_count);
		data01->var_length.resize(var_count);
		data01->var_display_length.resize(var_count);
		data01->var_decimals.resize(var_count);

		if (data01->parseValue)
		{
			data01->values.resize(var_count * obs_count);
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
			data01->var_count = var_count;
			data01->var_names.resize(var_count);
			data01->var_labels.resize(var_count);
			data01->var_formats.resize(var_count);
			data01->var_types.resize(var_count);
			data01->var_length.resize(var_count);
			data01->var_display_length.resize(var_count);
			data01->var_decimals.resize(var_count);
		}
		else if (data01->obs_count == 0)
		{
			data01->obs_count = obs_count;
			if (data01->parseValue)
			{
				data01->values.resize(data01->var_count * obs_count);
			}
		}
		else if (data01->obs_count > 0)
		{
			data01->obs_count = obs_count;
		}

		return READSTAT_HANDLER_OK;
	}

	int SasDoc::handle_variable(int index, readstat_variable_t* variable, const char* val_labels, void* ctx)
	{
		SasDoc* data01 = (SasDoc*)ctx;
		int len = (int)strlen(variable->name) + 1;
		data01->var_names[index] = readstat_variable_get_name(variable);

		const char* pTemp = readstat_variable_get_label(variable);
		data01->var_labels[index] = pTemp != NULL ? pTemp : "";
		pTemp = readstat_variable_get_format(variable);
		data01->var_formats[index] = pTemp != NULL ? pTemp : "";
		data01->var_types[index] = variable->type;
		data01->var_length[index] = variable->storage_width;
		data01->var_display_length[index] = variable->display_width;
		data01->var_decimals[index] = variable->decimals;

		return 0;
	}

	int SasDoc::handle_value(int obs_index, readstat_variable_t* variable, readstat_value_t value, void* ctx)
	{
		SasDoc* data01 = (SasDoc*)ctx;
		int var_index = readstat_variable_get_index(variable);
		int var_count = data01->var_count;
		int str_width;

		readstat_type_t type = readstat_value_type(value);
		const char* format = readstat_variable_get_format(variable);

		if (!readstat_value_is_missing(value, variable))
		{
			switch (type) {
			case READSTAT_TYPE_STRING:
				data01->values[obs_index * var_count + var_index] =
					flyweight_string(readstat_string_value(value));
				break;
			case READSTAT_TYPE_DOUBLE:
				data01->values[obs_index * var_count + var_index] =
					(double)readstat_double_value(value);
				break;
			case READSTAT_TYPE_INT8:
				data01->values[obs_index * var_count + var_index] =
					(double)readstat_int8_value(value);
				break;
			case READSTAT_TYPE_INT16:
				data01->values[obs_index * var_count + var_index] =
					(double)readstat_int16_value(value);
				break;
			case READSTAT_TYPE_INT32:
				data01->values[obs_index * var_count + var_index] =
					(double)readstat_int32_value(value);
				break;
			case READSTAT_TYPE_FLOAT:
				data01->values[obs_index * var_count + var_index] =
					(double)readstat_float_value(value);
				break;
			default:
				break;
			}
		}
		else {
			if (type == READSTAT_TYPE_DOUBLE)
			{
				data01->values[obs_index * var_count + var_index] = -INFINITY;
			}
		}

		return READSTAT_HANDLER_OK;
	}

	int SasDoc::read_sas7bdat(wstring path, void* user_ctx)
	{
		SasDoc* data01 = (SasDoc*)user_ctx;

		data01->obs_count = 0;
		data01->var_count = 0;

		readstat_error_t error = READSTAT_OK;
		readstat_parser_t* parser = readstat_parser_init();
		readstat_set_variable_handler(parser, &handle_variable);
		if (data01->parseValue)
		{
			readstat_set_value_handler(parser, &handle_value);
		}

		string filename = string(path.begin(), path.end());
		string right9 = tail(filename, 9);
		boost::algorithm::to_lower(right9);

		string right4 = tail(filename, 4);
		boost::algorithm::to_lower(right4);

		if (right9 == ".sas7bdat")
		{
			readstat_set_metadata_handler(parser, &handle_metadata);
			error = readstat_parse_sas7bdat(parser, filename.c_str(), data01);
		}
		else if (right4 == ".xpt")
		{
			readstat_set_metadata_handler(parser, &handle_metadata_xpt);
			error = readstat_parse_xport(parser, filename.c_str(), data01);
		}

		if (error != READSTAT_OK)
		{
			printf("Error processing %s: %d\n", path, error);
			return error;
		}

		readstat_parser_free(parser);

		data01->var_flag.resize(data01->var_count, true);

		if (data01->parseValue)
		{
			// Get the format
			for (int i = 0; i < data01->var_count; i++)
			{
				string formatName = data01->var_formats[i];
				if (formatName.size() > 0 && data01->mapFormat.find(formatName) == data01->mapFormat.end())
				{
					loadSASFormat(formatName, data01);
				}
			}

			data01->obs_flag.resize(data01->obs_count, true);
			data01->or_flag.resize(data01->obs_count, false);
			data01->obs_library_filter.resize(data01->obs_count, true);
		}

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

	int SasDoc::write_sas7bdat(std::wstring path, SasDoc* doc)
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
		readstat_writer_set_file_label(writer, doc->file_label.c_str());
		readstat_writer_set_file_format_version(writer, 5);
		// readstat_writer_set_file_encoding(writer, "UTF-8");
		// etc. as needed

		// 4) Add variables
		std::vector<readstat_variable_t*> varHandles(doc->var_count, nullptr);
		for (int i = 0; i < doc->var_count; i++) {
			readstat_type_t rsType = toReadStatType(doc->var_types[i]);
			// storage width: for numeric, say 8; for string, doc->var_length[i]
			int col_width = (rsType == READSTAT_TYPE_STRING)
				? doc->var_length[i]
				: 8;
			// Add variable
			readstat_variable_t* var = readstat_add_variable(
				writer, doc->var_names[i].c_str(), rsType, col_width);

			// If you have labels or formats:
			if (!doc->var_labels[i].empty()) {
				readstat_variable_set_label(var, doc->var_labels[i].c_str());
			}
			if (!doc->var_formats[i].empty()) {
				readstat_variable_set_format(var, doc->var_formats[i].c_str());
			}
			// decimals => can set display format or ...
			// readstat_variable_set_alignment(var, READSTAT_ALIGNMENT_RIGHT);

			varHandles[i] = var;
		}

		// 5) Begin writing for row_count = doc->obs_count
		int row_count = doc->obs_count;
		readstat_error_t rc = readstat_begin_writing_sas7bdat(
			writer, &fd, row_count);
		if (rc != READSTAT_OK) {
			std::cerr << "readstat_begin_writing_sas7bdat failed with code " << rc << std::endl;
			readstat_writer_free(writer);
			close(fd);
			return -3;
		}

		// 6) Write each row
		for (int r = 0; r < row_count; r++) {
			readstat_begin_row(writer);
			for (int c = 0; c < doc->var_count; c++) {
				readstat_type_t rsType = toReadStatType(doc->var_types[c]);
				// get the Cell from doc->values
				int idx = r * doc->var_count + c;
				Cell cell = doc->values[idx];

				if (rsType == READSTAT_TYPE_STRING) {
					// Expect a flyweight_string
					if (std::holds_alternative<flyweight_string>(cell)) {
						std::string val = std::get<flyweight_string>(cell).get();
						readstat_insert_string_value(writer, varHandles[c], val.c_str());
					}
					else {
						// missing => empty
						readstat_insert_missing_value(writer, varHandles[c]);
					}
				}
				else {
					// numeric
					if (std::holds_alternative<double>(cell)) {
						double val = std::get<double>(cell);
						// If val == -INFINITY, treat as missing
						if (std::isinf(val)) {
							readstat_insert_missing_value(writer, varHandles[c]);
						}
						else {
							readstat_insert_double_value(writer, varHandles[c], val);
						}
					}
					else {
						// It's actually a string stored in the variant => missing
						readstat_insert_missing_value(writer, varHandles[c]);
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

	SasDoc::~SasDoc()
	{
	}

	int SasDoc::GetVarNo(std::wstring varName)
	{
		if (varName.size() == 0)
			return -1;
		for (int i = 0; i < var_count; i++)
		{
			if (boost::iequals(var_names[i], varName))
			{
				return i;
			}
		}
		return -1;
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



	string SasDoc::GetCellText(int row, int col)
	{
		string cellText;

		if (row >= this->obs_count || col >= this->var_count || row < 0 || col < 0)
		{
			return cellText;
		}

		if (this->var_types[col] == READSTAT_TYPE_STRING)
		{
			cellText = Format((tchar*)get_value_string(row, col).data(),
				this->var_formats[col],
				this->var_display_length[col],
				this->var_decimals[col]);
		}
		else if (this->var_types[col] == READSTAT_TYPE_DOUBLE)
		{
			cellText = Format(get_value_double(row, col),
				this->var_formats[col],
				this->var_display_length[col],
				this->var_decimals[col]);
		}
		boost::algorithm::trim(cellText);
		return cellText;
	};

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
}
