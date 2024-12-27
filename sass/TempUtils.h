// TempUtils.h
#ifndef TEMP_UTILS_H
#define TEMP_UTILS_H

#include <string>

namespace sass {

	std::string getSystemTempFolder();
	std::string createUniqueTempFolder();
	bool removeDirectoryRecursively(const std::string& path);

} // namespace sass

#endif // TEMP_UTILS_H
