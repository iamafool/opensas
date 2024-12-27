// TempUtils.cpp
#include "TempUtils.h"
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sys/stat.h>
#include <iostream>

// If on Windows
#ifdef _WIN32
#include <windows.h>
#include <Shlwapi.h>
#include <direct.h>
#pragma comment(lib, "Shlwapi.lib")
#else
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif

namespace sass {

    std::string getSystemTempFolder() {
#ifdef _WIN32
        char buf[MAX_PATH];
        DWORD len = GetTempPathA(MAX_PATH, buf);
        if (len == 0) {
            // fallback
            return "C:\\Temp";
        }
        // remove trailing slash, for consistency
        if (buf[len - 1] == '\\') {
            buf[len - 1] = '\0';
        }
        return std::string(buf);
#else
        // On Unix, check $TMPDIR or fallback to /tmp
        const char* tmp = std::getenv("TMPDIR");
        if (!tmp) {
            return "/tmp";
        }
        else {
            return std::string(tmp);
        }
#endif
    }

    // We create a subfolder inside the system temp
    // e.g. on Windows: C:\Temp\sas_work_12345
    // e.g. on Unix: /tmp/sas_work_12345
    std::string createUniqueTempFolder() {
        std::string base = getSystemTempFolder();

#ifdef _WIN32
        // We can generate a unique name. 
        // One approach is to create a folder named "sas_work_XXXXXX"
        // We'll just do random or system calls
        char tmpName[MAX_PATH];
        // This is a simplistic approach, you can use GetTempFileNameA too
        sprintf_s(tmpName, "sas_work_%u", (unsigned int)GetTickCount());
        std::string folder = base + "\\" + tmpName;
        _mkdir(folder.c_str());
#else
        // On Unix
        char tmpName[64];
        sprintf(tmpName, "sas_work_%d", (int)getpid());
        std::string folder = base + "/" + tmpName;
        mkdir(folder.c_str(), 0700);
#endif

        return folder;
    }

    // Recursively remove a directory and all contents
    bool removeDirectoryRecursively(const std::string& path) {
#ifdef _WIN32
        // Windows approach: use SHFILEOPSTRUCT or manual recursion
        // For brevity, let's do a manual approach:

        std::string cmd = std::string("rd /s /q \"") + path + "\"";
        int ret = system(cmd.c_str());
        return (ret == 0);
#else
        // Unix approach
        // "rm -rf path"
        std::string cmd = std::string("rm -rf \"") + path + "\"";
        int ret = system(cmd.c_str());
        return (ret == 0);
#endif
    }

} // namespace sass
