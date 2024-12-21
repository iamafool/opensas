Severity	Code	Description	Project	File	Line	Suppression State
Error	C2338	static_assert failed: 'Unicode support requires compiling with /utf-8'	SASS	C:\workspace\c++\sashelper_qt\SASS\spdlog\fmt\bundled\base.h	458	
ChatGPT

Thought for 4 seconds
This error occurs because the underlying fmt library (bundled with spdlog) requires that the code be compiled with UTF-8 encoding support when Unicode features are enabled.

What does this mean?
fmt uses Unicode internally to handle wide or Unicode strings. On Windows with MSVC, the /utf-8 compiler flag ensures that the source code files and string literals are interpreted as UTF-8, which fmt and spdlog expect to maintain consistent behavior.

How to Fix
Option 1: Enable /utf-8 Flag in Visual Studio

Open your project¡¯s Properties.
Navigate to Configuration Properties ¡ú C/C++ ¡ú Command Line.
In the Additional Options box, add:
bash
Copy code
/utf-8
Click Apply and rebuild the project.
Option 2: Enable UTF-8 in Project Settings (Alternative)
In newer versions of Visual Studio, you can also enable a UTF-8 execution character set via the "Character Set" setting:

Right-click the project ¡ú Properties.
Under Configuration Properties ¡ú C/C++ ¡ú Advanced, look for ¡°Character Set¡±.
Change it to ¡°Use Unicode Character Set¡±.
Apply and rebuild.
If this does not solve the issue directly, still add /utf-8 in the command line options.
Why This Is Needed
The static_assert in fmt or spdlog¡¯s bundled fmt code indicates that Unicode support is on but the code is not compiled with the proper character set handling. Using /utf-8 ensures that all source code and string literals are treated as UTF-8, allowing fmt to correctly handle Unicode strings.

Summary
Add /utf-8 to your MSVC compile options to satisfy fmt and spdlog¡¯s Unicode requirements, and the static_assert error should be resolved.