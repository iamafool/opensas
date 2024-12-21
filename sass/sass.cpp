#include <iostream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <fstream>

// Pseudocode function to read SAS code from a file
std::string readSasFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (!in) {
        return "";
    }
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(static_cast<size_t>(in.tellg()));
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], static_cast<std::streamsize>(contents.size()));
    return contents;
}

// Pseudocode function to run SAS code
// In your actual code, this would parse and execute the SAS code
void runSasCode(const std::string& sasCode, spdlog::logger& logLogger, spdlog::logger& lstLogger, bool interactive) {
    // Log that we are starting
    logLogger.info("Running SAS code...");
    if (interactive) {
        logLogger.info("Mode: Interactive");
    }
    else {
        logLogger.info("Mode: File-based");
    }

    // For demonstration, just print some placeholder results:
    lstLogger.info("SAS Results:");
    lstLogger.info("OBS     VAR1     VAR2");
    lstLogger.info("1       10       20");
    lstLogger.info("2       30       40");

    // Log done
    logLogger.info("SAS code execution finished.");
}

int main(int argc, char** argv)
{
    std::string sasFile;
    std::string logFile;
    std::string lstFile;

    // Parse command line arguments
    // Expected patterns:
    // -sas=xxx.sas
    // -log=xxx.log
    // -lst=xxx.lst
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.rfind("-sas=", 0) == 0) {
            sasFile = arg.substr(5);
        }
        else if (arg.rfind("-log=", 0) == 0) {
            logFile = arg.substr(5);
        }
        else if (arg.rfind("-lst=", 0) == 0) {
            lstFile = arg.substr(5);
        }
    }

    // Determine mode:
    // 1) Interactive mode: no sasFile
    // 2) File mode (console output): sasFile given, but no log/lst
    // 3) Batch mode: sasFile, logFile, lstFile all given
    bool interactiveMode = sasFile.empty();
    bool batchMode = (!sasFile.empty() && !logFile.empty() && !lstFile.empty());
    bool fileMode = (!sasFile.empty() && logFile.empty() && lstFile.empty()); // SAS from file, output to console

    // Set up loggers
    std::shared_ptr<spdlog::logger> logLogger;
    std::shared_ptr<spdlog::logger> lstLogger;

    if (batchMode) {
        // Batch mode: log and lst to files
        auto logSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
        auto lstSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(lstFile, true);

        logLogger = std::make_shared<spdlog::logger>("log", logSink);
        lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
    }
    else {
        // Interactive or file mode: log and lst to console
        auto logSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
        auto lstSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        logLogger = std::make_shared<spdlog::logger>("log", logSink);
        lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
    }

    spdlog::register_logger(logLogger);
    spdlog::register_logger(lstLogger);
    logLogger->set_level(spdlog::level::info);
    lstLogger->set_level(spdlog::level::info);

    std::string sasCode;

    if (interactiveMode) {
        // Interactive mode: read code from stdin or a REPL-like interface
        logLogger->info("Running in interactive mode. Type SAS code and end with Ctrl+D (Unix) or Ctrl+Z (Windows):");
        std::string codeBuffer;
        while (true) {
            std::cout << "SAS> ";
            std::string line;
            if (!std::getline(std::cin, line)) {
                // End of input (Ctrl+D / Ctrl+Z)
                break;
            }

            // If user enters "quit" or "exit"
            if (line == "quit" || line == "exit") {
                break;
            }

            codeBuffer += line + "\n";

            // Check if we have a complete statement
            // This could be as simple as checking for a semicolon:
            // For a more robust check, consider actually tokenizing and checking syntax.
            if (codeBuffer.find(';') != std::string::npos) {
                // We have at least one complete statement
                // Attempt to parse the buffer
                bool success = parseAndExecuteSasCode(codeBuffer); // Your parsing function
                if (!success) {
                    std::cerr << "Error parsing code.\n";
                }

                // On success or failure, clear or adjust the buffer.
                // If you parse everything, clear the buffer:
                codeBuffer.clear();

                // If you only parsed one statement and might have more partial code left,
                // you'd remove the parsed portion and keep the remainder.
            }
            // If no semicolon found, just loop back and prompt for more input.
        }

    }
    else if (fileMode) {
        // File mode: read code from sasFile, output to console
        logLogger->info("Running from SAS file: {}", sasFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }
    }
    else if (batchMode) {
        // Batch mode: read code from sasFile, log and lst to files
        logLogger->info("Running in batch mode: SAS={} LOG={} LST={}", sasFile, logFile, lstFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }
    }

    // Run the SAS code
    runSasCode(sasCode, *logLogger, *lstLogger, interactiveMode);

    return 0;
}
