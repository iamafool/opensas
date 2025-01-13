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

	sasdoc.load(wstring(path.begin(), path.end()));

	EXPECT_EQ(sasdoc.getColumnCount(), 16);
	EXPECT_EQ(sasdoc.getRowCount(), 5);

	fs::create_directories("C:/workspace/c++/sass/test/output/");
	path = "C:/workspace/c++/sass/test/output/dm.sas7bdat";
	sasdoc.save(wstring(path.begin(), path.end()));

	EXPECT_TRUE(fs::exists(path)) << "Expected file does not exist at path: " << path;

	SasDoc sasdoc1;
	auto rc = sasdoc1.load(wstring(path.begin(), path.end()));
	EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << path;

	EXPECT_EQ(sasdoc1.getColumnCount(), 16);
	EXPECT_EQ(sasdoc1.getRowCount(), 5);

}