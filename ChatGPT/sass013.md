From now on, I will hand written the SAS interpreter based on the codes you given. Next question is the log. How to log it in the console? 
ChatGPT

Thought about logging in console for a couple of seconds
If you just want to print to the console for debugging or to mimic SAS¡¯s log behavior, you can start with a straightforward approach: using std::cerr or std::cout as appropriate. Over time, you can refine this into a more robust logging system.

Simple Approach: Using std::cerr or std::cout
When SAS runs code, it produces a log that shows the code submitted, any warnings, errors, and notes about what happened. For your interpreter, you could:

Print informational messages to std::cout.
Print warnings and errors to std::cerr.
Example:

cpp
Copy code
#include <iostream>
#include <string>

void logInfo(const std::string &message) {
    std::cout << "[INFO] " << message << "\n";
}

void logWarning(const std::string &message) {
    std::cerr << "[WARNING] " << message << "\n";
}

void logError(const std::string &message) {
    std::cerr << "[ERROR] " << message << "\n";
}
Then, whenever you process code, parse tokens, or encounter an error, you call these functions. For example, inside the parser, if you detect an unexpected token:

cpp
Copy code
if (unexpectedToken) {
    logError("Unexpected token found at line " + std::to_string(token.line));
}
Mimicking SAS Logging Style
SAS logs typically show the code lines being processed and then messages. For example, if you have:

cpp
Copy code
data out; set in; x = 42; if x then output; run;
You might log the code first:

cpp
Copy code
logInfo("----- SAS Code Submitted -----");
logInfo("data out; set in; x = 42; if x then output; run;");
logInfo("-----------------------------");
And as you parse and execute:

cpp
Copy code
logInfo("Reading dataset 'in'. Rows: " + std::to_string(inData.rows.size()));
logInfo("Creating dataset 'out'.");
If an error occurs, you output something like:

cpp
Copy code
logError("Invalid syntax at line 1, near 'x'");
Adding Verbosity Levels
If you want more control over logging (e.g., debug vs. info vs. error), you could define an enum and control output verbosity:

cpp
Copy code
enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

LogLevel currentLevel = LogLevel::INFO;

void log(LogLevel level, const std::string &message) {
    if (level >= currentLevel) {
        switch (level) {
            case LogLevel::DEBUG:   std::cout << "[DEBUG] " << message << "\n"; break;
            case LogLevel::INFO:    std::cout << "[INFO] " << message << "\n"; break;
            case LogLevel::WARNING: std::cerr << "[WARNING] " << message << "\n"; break;
            case LogLevel::ERROR:   std::cerr << "[ERROR] " << message << "\n"; break;
        }
    }
}
You can then call:

cpp
Copy code
log(LogLevel::INFO, "Starting interpretation of DATA step...");
log(LogLevel::ERROR, "Unexpected token 'run' without terminating semicolon.");
Using a Dedicated Logging Library
As your project grows, consider a more robust logging solution:

spdlog: A popular C++ logging library that offers formatting, levels, async logging, and more. You can print to console or files easily.
Boost.Log: Part of the Boost libraries, very flexible and feature-rich, though a bit more complex to set up.
fmt: While not a logger, it provides great formatting capabilities you can integrate into your logs.
For example, with spdlog:

cpp
Copy code
#include "spdlog/spdlog.h"

int main() {
    spdlog::set_level(spdlog::level::info); // set default log level
    spdlog::info("Starting SAS interpreter...");
    spdlog::warn("This is a warning message.");
    spdlog::error("An error occurred at line {}", lineNumber);
    return 0;
}
This approach gives you more formatting power and cleaner code but requires an external dependency.

Summary
Basic Step: Use std::cout and std::cerr for simple logging.
Refine: Create functions or a small logging class with log levels.
Advanced: Integrate a professional logging library like spdlog for better formatting, levels, and output options.
Since you¡¯re starting out and likely iterating on your interpreter design, the simplest path is to begin with std::cout and std::cerr and later move to a more sophisticated solution as your needs evolve.