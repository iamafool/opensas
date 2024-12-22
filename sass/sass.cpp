#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <string>
#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "DataEnvironment.h"
#include "AST.h"

// Function to read SAS code from a file
std::string readSasFile(const std::string &filename) {
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

// Function to run SAS code
void runSasCode(const std::string &sasCode, Interpreter &interpreter, bool interactive) {
    // Lexing
    Lexer lexer(sasCode);
    std::vector<Token> tokens;
    Token tok;
    while ((tok = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        tokens.push_back(tok);
    }

    // Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> program;
    try {
        program = parser.parseProgram();
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Parsing failed: {}", e.what());
        return;
    }

    // Interpret
    try {
        interpreter.executeProgram(program);
    }
    catch (const std::runtime_error &e) {
        interpreter.logLogger.error("Execution failed: {}", e.what());
    }
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
        try {
            auto logSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            auto lstSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(lstFile, true);

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }
    else {
        // Interactive or file mode: log and lst to console
        try {
            auto logSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            auto lstSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cerr << "Console log initialization failed: " << ex.what() << "\n";
            return 1;
        }
    }

    // Register loggers
    spdlog::register_logger(logLogger);
    spdlog::register_logger(lstLogger);
    logLogger->set_level(spdlog::level::info);
    lstLogger->set_level(spdlog::level::info);

    DataEnvironment env;
    Interpreter interpreter(env, *logLogger, *lstLogger);

    std::string sasCode;

    if (interactiveMode) {
        // Interactive mode: read code from stdin or a REPL-like interface
        logLogger->info("Running in interactive mode. Type SAS code line by line. End with 'run;' or type 'quit'/'exit' to exit.");
        std::string line;
        std::string codeBuffer;
        while (true) {
            std::cout << "SAS> ";
            if (!std::getline(std::cin, line)) {
                // End of input (Ctrl+D / Ctrl+Z)
                if (!codeBuffer.empty()) {
                    logLogger->info("Executing accumulated code:\n{}", codeBuffer);
                    runSasCode(codeBuffer, interpreter, true);
                }
                break;
            }

            // If user enters "quit" or "exit"
            if (line == "quit" || line == "exit") {
                break;
            }

            if (line == "help" || line == "?") {
                std::cout << "Supported Commands:\n";
                std::cout << "  options ... ;       - Set global options\n";
                std::cout << "  libname ... ;        - Assign a library reference\n";
                std::cout << "  title '...' ;        - Set the title for outputs\n";
                std::cout << "  data ... ; run;      - Define and execute a data step\n";
                std::cout << "  proc print data=...; - Print a dataset\n";
                std::cout << "  quit / exit          - Exit the interpreter\n";
                continue;
            }

            // Handle comments: skip lines starting with '*', or remove inline comments
            // Simplistic handling here; consider more robust comment parsing
            if (!line.empty() && line[0] == '*') {
                logLogger->info("Skipping comment: {}", line);
                continue;
            }

            // Append line to buffer
            codeBuffer += line + "\n";

            // Check if codeBuffer contains at least one semicolon indicating statement termination
            size_t semicolonPos = codeBuffer.find(';');
            while (semicolonPos != std::string::npos) {
                // Extract the statement up to the semicolon
                std::string statement = codeBuffer.substr(0, semicolonPos + 1);
                logLogger->info("Executing statement: {}", statement);

                runSasCode(statement, interpreter, true);

                // Remove the executed statement from the buffer
                codeBuffer.erase(0, semicolonPos + 1);

                // Check for another semicolon in the remaining buffer
                semicolonPos = codeBuffer.find(';');
            }
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

        runSasCode(sasCode, interpreter, false);
    }
    else if (batchMode) {
        // Batch mode: read code from sasFile, log and lst to files
        logLogger->info("Running in batch mode: SAS={} LOG={} LST={}", sasFile, logFile, lstFile);
        sasCode = readSasFile(sasFile);
        if (sasCode.empty()) {
            logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
            return 1;
        }

        runSasCode(sasCode, interpreter, false);
    }

    return 0;
}
