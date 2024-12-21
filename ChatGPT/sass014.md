You know SAS has log and output. I also want the possibility to log to a file and output to a file. 
ChatGPT

Thought about logging and output to files for a couple of seconds
To support multiple logging destinations (console and file) and separate the concept of log (like the SAS log) from the output (like the SAS listing output), you can implement a flexible logging mechanism. This mechanism can allow you to write to:

Console (stdout/stderr)
A file
Both simultaneously if needed
Likewise for the output (the SAS equivalent of the listing output), you can create a separate output manager that can also write to the console or a file. The key idea is to separate the concept of ¡°log¡± and ¡°output¡± into two different streams or classes, each of which can direct to different sinks (console, file, or both).

Below is an example of how you might structure these classes. This is a starting point, and you can refine it as your needs grow (e.g., adding thread safety, adjusting formatting, adding timestamps, etc.).

Example: Log and Output Managers
cpp
Copy code
#include <iostream>
#include <fstream>
#include <memory>
#include <string>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // By default, logs go to standard error
    Logger(std::ostream &defaultStream = std::cerr) 
        : consoleStream(&defaultStream), fileStream(nullptr) {}

    // Set a file for logging
    bool setLogFile(const std::string &filename) {
        fileStreamOwned = std::make_unique<std::ofstream>(filename, std::ios::out | std::ios::trunc);
        if (!fileStreamOwned->is_open()) {
            fileStreamOwned.reset();
            return false;
        }
        fileStream = fileStreamOwned.get();
        return true;
    }

    // Set logging back to console only
    void clearLogFile() {
        fileStreamOwned.reset();
        fileStream = nullptr;
    }

    void log(LogLevel level, const std::string &message) {
        std::string prefix;
        switch (level) {
            case LogLevel::INFO:    prefix = "[INFO] "; break;
            case LogLevel::WARNING: prefix = "[WARNING] "; break;
            case LogLevel::ERROR:   prefix = "[ERROR] "; break;
        }

        // Log to console if available
        if (consoleStream) {
            *consoleStream << prefix << message << "\n";
        }

        // Also log to file if available
        if (fileStream) {
            *fileStream << prefix << message << "\n";
        }
    }

    void info(const std::string &message) { log(LogLevel::INFO, message); }
    void warn(const std::string &message) { log(LogLevel::WARNING, message); }
    void error(const std::string &message) { log(LogLevel::ERROR, message); }

private:
    std::ostream *consoleStream;
    std::ofstream *fileStream;
    std::unique_ptr<std::ofstream> fileStreamOwned; // Owns the file output stream if set
};

class OutputManager {
public:
    // By default, outputs go to stdout
    OutputManager(std::ostream &defaultStream = std::cout)
        : consoleStream(&defaultStream), fileStream(nullptr) {}

    bool setOutputFile(const std::string &filename) {
        fileStreamOwned = std::make_unique<std::ofstream>(filename, std::ios::out | std::ios::trunc);
        if (!fileStreamOwned->is_open()) {
            fileStreamOwned.reset();
            return false;
        }
        fileStream = fileStreamOwned.get();
        return true;
    }

    void clearOutputFile() {
        fileStreamOwned.reset();
        fileStream = nullptr;
    }

    void write(const std::string &message) {
        // Write to console if available
        if (consoleStream) {
            *consoleStream << message << "\n";
        }

        // Also write to file if available
        if (fileStream) {
            *fileStream << message << "\n";
        }
    }

private:
    std::ostream *consoleStream;
    std::ofstream *fileStream;
    std::unique_ptr<std::ofstream> fileStreamOwned;
};
How to Use:
cpp
Copy code
int main() {
    Logger log;
    OutputManager output;

    // By default, logging goes to stderr, output goes to stdout
    log.info("Program started");
    output.write("This is the SAS-like output listing");

    // Redirect log to a file
    if (log.setLogFile("sas_log.txt")) {
        log.info("Logging also redirected to sas_log.txt");
    } else {
        log.error("Failed to open sas_log.txt for logging");
    }

    // Redirect output to a file
    if (output.setOutputFile("sas_output.txt")) {
        output.write("This will go to the output file as well as console");
    } else {
        log.error("Failed to open sas_output.txt for writing output");
    }

    // Simulate some SAS interpreter activities
    log.info("Parsed DATA step successfully.");
    output.write("Result: 10 observations written.");

    // If you want to stop logging to files and revert back to console only
    log.clearLogFile();
    output.clearOutputFile();

    log.info("Reverted back to console-only logging.");
    output.write("Console-only output again.");

    return 0;
}
Notes and Improvements:
Log vs. Output Separation:
SAS differentiates between the log (which shows code run, warnings, errors, and notes) and the output listing (which shows results from procedures). Here, you have two separate classes (Logger and OutputManager). They can be given different formatting, routes, etc.

Binary or Text Files:
The example opens files in text mode. For large binary outputs or different formats, you can adjust the file open modes.

Thread Safety:
If your interpreter becomes multi-threaded, you may need to add mutexes or use a thread-safe logging mechanism.

Levels and Filters for the Output:
The Logger class shows how to differentiate between info/warning/error. The OutputManager currently just writes lines of text. You can customize further based on your needs (e.g., formatting tables, headers, etc.).

No External Dependencies:
This solution uses standard C++ only. If you want more advanced features, consider libraries like spdlog for logging or a custom formatting library for output.

In summary, this is a flexible approach: you now have two distinct channels (log and output), each capable of writing to console, file, or both. You can integrate these classes into your interpreter and call their methods at appropriate points in the parsing, execution, and output generation phases.