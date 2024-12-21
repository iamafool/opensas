Severity	Code	Description	Project	File	Line	Suppression State
Error	C1083	Cannot open include file: 'spdlog/common.h': No such file or directory	SASS	C:\workspace\c++\sashelper_qt\SASS\spdlog.h	12	
ChatGPT

Thought about file inclusion error for spdlog for a couple of seconds
The error indicates that the compiler can¡¯t find the spdlog headers. To fix this, you need to ensure that the spdlog library is installed on your system and that your compiler knows where to find its header files.

Steps to Resolve the Error:
Download and Install spdlog:
If you haven¡¯t already, you can get spdlog from its GitHub repository or through your system¡¯s package manager (if available).

If you¡¯re on Windows and using Visual Studio, you might consider using vcpkg:

bash
Copy code
vcpkg install spdlog
Then integrate vcpkg with your Visual Studio environment so your project automatically finds the include paths.

Alternatively, you can clone the repo:

bash
Copy code
git clone https://github.com/gabime/spdlog.git
This gives you a folder like spdlog/include/spdlog/....

Add spdlog to Your Include Path:
If you have the spdlog headers in a directory, say C:\path\to\spdlog\include, ensure you add this directory to your compiler¡¯s include paths.

In Visual Studio (MSVC):

Go to Project Properties ¡ú C/C++ ¡ú Additional Include Directories.
Add the path to the include directory of spdlog. For example:
vbnet
Copy code
C:\path\to\spdlog\include
In CMake (if you¡¯re using it):

cmake
Copy code
include_directories("C:/path/to/spdlog/include")
If you installed spdlog via vcpkg and integrated it, CMake might find it automatically with:

cmake
Copy code
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE spdlog::spdlog)
This also sets up include paths.

Check the Include Statement:
Make sure your code includes spdlog correctly:

cpp
Copy code
#include <spdlog/spdlog.h>
or if you¡¯re referencing something else:

cpp
Copy code
#include <spdlog/common.h>
However, typically, you wouldn¡¯t include common.h directly. You¡¯d include spdlog/spdlog.h or other main headers and let them bring in common.h as needed.

Verify Directory Structure:
After installation, your spdlog headers should be organized as:

makefile
Copy code
spdlog/
  include/
    spdlog/
      spdlog.h
      common.h
      ...
Make sure that the path you added leads to a directory that contains spdlog/ as a subdirectory.

Clean and Rebuild:
Sometimes, after adjusting include paths or installing libraries, a full rebuild is necessary.

Summary:
You need to ensure that spdlog is properly installed and that the compiler is instructed to look into the spdlog/include directory. Typically, after installing spdlog (e.g., using vcpkg or cloning and placing it in a known include path), you just add -I/path/to/spdlog/include (for GCC/Clang) or update the project settings in Visual Studio to include that directory. Then the compiler should find spdlog/common.h without issue.