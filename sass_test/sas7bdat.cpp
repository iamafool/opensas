#include <gtest/gtest.h>
#include "sasdoc.h"
#include <filesystem>

using namespace sass;
using namespace std;

namespace fs = std::filesystem;

TEST(SAS7BDAT, Read)
{
	SasDoc sasdoc;
	string path = "c:/workspace/c++/sass/test/data/dm.sas7bdat";

	SasDoc::read_sas7bdat(wstring(path.begin(), path.end()), &sasdoc);

	EXPECT_EQ(sasdoc.var_count, 16);
	EXPECT_EQ(sasdoc.obs_count, 5);

	fs::create_directories("C:/workspace/c++/sass/test/output/");
	path = "C:/workspace/c++/sass/test/output/dm.sas7bdat";
	SasDoc::write_sas7bdat(wstring(path.begin(), path.end()), &sasdoc);

	EXPECT_TRUE(fs::exists(path)) << "Expected file does not exist at path: " << path;

	SasDoc sasdoc1;
	auto rc = SasDoc::read_sas7bdat(wstring(path.begin(), path.end()), &sasdoc1);
	EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << path;

	EXPECT_EQ(sasdoc1.var_count, 16);
	EXPECT_EQ(sasdoc1.obs_count, 5);

}