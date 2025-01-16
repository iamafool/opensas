#include <gtest/gtest.h>
#include "Interpreter.h"
#include "DataEnvironment.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace sass;

class SassTest : public testing::Test {
protected:
    DataEnvironment* env = nullptr;
    Interpreter* interpreter = nullptr;
    std::shared_ptr<spdlog::logger> logLogger;
    std::shared_ptr<spdlog::logger> lstLogger;

    SassTest() {
        try {
            auto logSink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
            auto lstSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            logLogger = std::make_shared<spdlog::logger>("log", logSink);
            lstLogger = std::make_shared<spdlog::logger>("lst", lstSink);
        }
        catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Console log initialization failed: " << ex.what() << "\n";
        }

        logLogger->set_level(spdlog::level::off);
        lstLogger->set_level(spdlog::level::off);
        env = new DataEnvironment(*logLogger, *lstLogger);
        interpreter = new Interpreter(*env);
    }

    ~SassTest() override {
        delete env;
        delete interpreter;
    }

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    void SetUp() override {
        // Code here will be called immediately after the constructor (right
        // before each test).
    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

    // Class members declared here can be used by all tests in the test suite
    // for Foo.
};
