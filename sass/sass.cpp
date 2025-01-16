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
#include "Repl.h"

using namespace sass;

// Function to read SAS code from a file
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

// Function to run SAS code
void runSasCode(const std::string& sasCode, Interpreter& interpreter, DataEnvironment& env, bool interactive) {
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
	catch (const std::runtime_error& e) {
		env.logLogger.error("Parsing failed: {}", e.what());
		return;
	}

	// Interpret
	try {
		interpreter.executeProgram(program);
	}
	catch (const std::runtime_error& e) {
		env.logLogger.error("Execution failed: {}", e.what());
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
		catch (const spdlog::spdlog_ex& ex) {
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
		catch (const spdlog::spdlog_ex& ex) {
			std::cerr << "Console log initialization failed: " << ex.what() << "\n";
			return 1;
		}
	}

	// Register loggers
	spdlog::register_logger(logLogger);
	spdlog::register_logger(lstLogger);
	logLogger->set_level(spdlog::level::info);
	logLogger->set_pattern("%v");
	lstLogger->set_level(spdlog::level::info);
	lstLogger->set_pattern("%v");

	DataEnvironment env(*logLogger, *lstLogger);
	Interpreter interpreter(env);

	std::string sasCode;

	if (interactiveMode) {
		// Initialize REPL
		Repl repl(interpreter);

		// Run REPL
		repl.run();
	}
	else if (fileMode) {
		// File mode: read code from sasFile, output to console
		logLogger->info("Running from SAS file: {}", sasFile);
		sasCode = readSasFile(sasFile);
		if (sasCode.empty()) {
			logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
			return 1;
		}

		runSasCode(sasCode, interpreter, env, false);
	}
	else if (batchMode) {
		// Batch mode: read code from sasFile, log and lst to files
		logLogger->info("Running in batch mode: SAS={} LOG={} LST={}", sasFile, logFile, lstFile);
		sasCode = readSasFile(sasFile);
		if (sasCode.empty()) {
			logLogger->error("Failed to read SAS file or file is empty: {}", sasFile);
			return 1;
		}

		runSasCode(sasCode, interpreter, env, false);
	}

	return 0;
}

