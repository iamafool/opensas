#include <gtest/gtest.h>
#include <memory>
#include "fixture.h"
#include "Lexer.h"
#include "Parser.h"
#include <filesystem>
#include <boost/flyweight.hpp>

using namespace std;
using namespace sass;
namespace fs = std::filesystem;

using flyweight_string = boost::flyweight<std::string>;

TEST_F(SassTest, DataStepOutput1) {
    std::string code = R"(
        data a;
           a = 10;
           output;
        run;
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 11) << "Expected 11 tokens.";

    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_DATA);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].text, "a");
    EXPECT_EQ(tokens[2].type, TokenType::SEMICOLON);

    // Make sure we see 'output' and 'run'
    bool foundOutput = false;
    bool foundRun = false;
    for (auto& t : tokens) {
        if (t.type == TokenType::KEYWORD_OUTPUT) foundOutput = true;
        if (t.type == TokenType::KEYWORD_RUN) foundRun = true;
    }
    EXPECT_TRUE(foundOutput);
    EXPECT_TRUE(foundRun);

    Parser parser(tokens);
    ParseResult parseResult;
    parseResult = parser.parseStatement();
    ASSERT_EQ(parseResult.status, ParseStatus::PARSE_SUCCESS);

    interpreter->execute(parseResult.node.get());

    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "a.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 1);
    EXPECT_EQ(sasdoc1.obs_count, 1);

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 10.0);
}

TEST_F(SassTest, DataStepOutput2) {
    std::string code = R"(
        data a;
           a = 10;
           output;
           b = "This is a string variable!";
           output;
        run;
    )";

    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 17u) << "Expected 14 tokens.";

    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_DATA);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].text, "a");
    EXPECT_EQ(tokens[2].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[3].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[4].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[5].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[6].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[7].type, TokenType::KEYWORD_OUTPUT);
    EXPECT_EQ(tokens[8].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[9].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[10].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[11].type, TokenType::STRING);
    EXPECT_EQ(tokens[12].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[13].type, TokenType::KEYWORD_OUTPUT);
    EXPECT_EQ(tokens[14].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[15].type, TokenType::KEYWORD_RUN);
    EXPECT_EQ(tokens[16].type, TokenType::SEMICOLON);

    Parser parser(tokens);
    ParseResult parseResult;
    parseResult = parser.parseStatement();
    ASSERT_EQ(parseResult.status, ParseStatus::PARSE_SUCCESS);

    interpreter->execute(parseResult.node.get());

    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "a.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 2);
    EXPECT_EQ(sasdoc1.obs_count, 2);

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 10.0);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[1]).get(), "");
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 10.0);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[3]).get(), "This is a string variable!");

}


TEST_F(SassTest, DataStepInput1) {
    std::string code = R"(
        data employees;
          input name $ age;
          datalines;
john 23
mary 30
;
        run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseStatement();
    ASSERT_TRUE(parseResult.status == ParseStatus::PARSE_SUCCESS);

    // 3) Interpret
    interpreter->execute(parseResult.node.get());

    // 4) Check dataset WORK.employees => 2 obs, 2 vars
    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "employees.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 2);
    EXPECT_EQ(sasdoc1.obs_count, 2);

    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[0]).get(), "john");
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 23.0);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[2]).get(), "mary");
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 30.0);
}

TEST_F(SassTest, DataStepSet1) {
    std::string code = R"(
        libname test "c:\workspace\c++\sass\test\data\";
        data dm;
            set test.dm; 
        run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 2);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    // 4) Check dataset WORK.employees => 2 obs, 2 vars
    // Verify that the dataset 'a' exists with correct values
    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "dm.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 16);
    EXPECT_EQ(sasdoc1.obs_count, 5);
}

TEST_F(SassTest, DataStepFunction1) {
    std::string code = R"(
data in;
    input x y;
    datalines;
4 20
16 30
9 15
25 40
;
run;

data out; 
    set in; 
    sqrt_x = sqrt(x); 
    abs_diff = abs(y - 25); 
    log_y = log(y); 
    if sqrt_x > 3 and abs_diff < 10 then output; 
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 5);
    EXPECT_EQ(sasdoc1.obs_count, 1);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "y");
    EXPECT_EQ(sasdoc1.var_names[2], "sqrt_x");
    EXPECT_EQ(sasdoc1.var_names[3], "abs_diff");
    EXPECT_EQ(sasdoc1.var_names[4], "log_y");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 16.0);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 30.0);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 4.0);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 5.0);
    EXPECT_NEAR(std::get<double>(sasdoc1.values[4]), 3.4011973817, 1e-7);
}

TEST_F(SassTest, DataStepIfElse1) {
    std::string code = R"(
data in;
    input x y;
    datalines;
5 10
15 20
10 15
20 25
;
run;

data out; 
    set in; 
    if x > 10 then do;
        status = 'High';
        y = y * 2;
    end;
    else do;
        status = 'Low';
        y = y + 5;
    end;
    output; 
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 3);
    EXPECT_EQ(sasdoc1.obs_count, 4);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "y");
    EXPECT_EQ(sasdoc1.var_names[2], "status");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 5);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 15);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[2]).get(), "Low");
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]),40);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[5]).get(), "High");
}

TEST_F(SassTest, DataStepIfElse2) {
    std::string code = R"(
data in;
    input x y;
    datalines;
3 10
7 15
12 20
18 25
;
run;

data out; 
    set in; 
    if x > 15 then do;
        category = 'Very High';
        y = y * 3;
    end;
    else if x > 10 then do;
        category = 'High';
        y = y * 2;
    end;
    else if x > 5 then do;
        category = 'Medium';
        y = y + 10;
    end;
    else do;
        category = 'Low';
        y = y + 5;
    end;
    output; 
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 3);
    EXPECT_EQ(sasdoc1.obs_count, 4);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "y");
    EXPECT_EQ(sasdoc1.var_names[2], "category");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 15);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[2]).get(), "Low");
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 7);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 25);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[5]).get(), "Medium");
    EXPECT_EQ(std::get<double>(sasdoc1.values[6]), 12);
    EXPECT_EQ(std::get<double>(sasdoc1.values[7]), 40);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[8]).get(), "High");
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 18);
    EXPECT_EQ(std::get<double>(sasdoc1.values[10]), 75);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[11]).get(), "Very High");

}

TEST_F(SassTest, DataStepIfElse3) {
    std::string code = R"(
data in;
    input x y;
    datalines;
3 10
7 15
12 18
16 35
9 20
;
run;

data out; 
    set in; 
    if x > 15 then do;
        category = 'Very High';
        if y > 30 then status = 'Excellent';
        else status = 'Good';
    end;
    else if x > 10 then do;
        category = 'High';
        if y > 20 then status = 'Good';
        else status = 'Fair';
    end;
    else if x > 5 then do;
        category = 'Medium';
        status = 'Average';
    end;
    else do;
        category = 'Low';
        status = 'Poor';
    end;
    output; 
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 4);
    EXPECT_EQ(sasdoc1.obs_count, 5);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "y");
    EXPECT_EQ(sasdoc1.var_names[2], "category");
    EXPECT_EQ(sasdoc1.var_names[3], "status");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 10);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[2]).get(), "Low");
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[3]).get(), "Poor");
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 7);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 15);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[6]).get(), "Medium");
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[7]).get(), "Average");
    EXPECT_EQ(std::get<double>(sasdoc1.values[8]), 12);
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 18);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[10]).get(), "High");
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[11]).get(), "Fair");
    EXPECT_EQ(std::get<double>(sasdoc1.values[12]), 16);
    EXPECT_EQ(std::get<double>(sasdoc1.values[13]), 35);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[14]).get(), "Very High");
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[15]).get(), "Excellent");
    EXPECT_EQ(std::get<double>(sasdoc1.values[16]), 9);
    EXPECT_EQ(std::get<double>(sasdoc1.values[17]), 20);
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[18]).get(), "Medium");
    EXPECT_EQ(std::get<flyweight_string>(sasdoc1.values[19]).get(), "Average");

}

TEST_F(SassTest, DataStepDrop1) {
    std::string code = R"(
data in;
    input x num1 num2 num3;
    datalines;
1 5 10 15
2 10 15 20
3 15 20 25
;
run;

data out; 
    set in; 
    drop x num2;
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 2);
    EXPECT_EQ(sasdoc1.obs_count, 3);
    EXPECT_EQ(sasdoc1.var_names[0], "num1");
    EXPECT_EQ(sasdoc1.var_names[1], "num3");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 5);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 10);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 25);

}

TEST_F(SassTest, DataStepDo1) {
    std::string code = R"(
data in;
    input x num1 num2 num3;
    datalines;
1 5 10 15
2 10 15 20
3 15 20 25
;
run;

data out; 
    set in; 
    retain sum 0;
    array nums {3} num1 num2 num3;
    do i = 1 to 3;
        nums{i} = nums{i} + 10;
        sum = sum + nums{i};
    end;
    drop i;
    keep x sum num1 num2 num3;
    if sum > 25 then output; 
run;

proc print data=out;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 3);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    ASSERT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = SasDoc::read_sas7bdat(wstring(filePath.begin(), filePath.end()), &sasdoc1);
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.var_count, 5);
    EXPECT_EQ(sasdoc1.obs_count, 3);
    EXPECT_EQ(sasdoc1.var_names[0], "x");
    EXPECT_EQ(sasdoc1.var_names[1], "num1");
    EXPECT_EQ(sasdoc1.var_names[1], "num2");
    EXPECT_EQ(sasdoc1.var_names[2], "num3");
    EXPECT_EQ(sasdoc1.var_names[3], "sum");

    EXPECT_EQ(std::get<double>(sasdoc1.values[0]), 1);
    EXPECT_EQ(std::get<double>(sasdoc1.values[1]), 15);
    EXPECT_EQ(std::get<double>(sasdoc1.values[2]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[3]), 25);
    EXPECT_EQ(std::get<double>(sasdoc1.values[4]), 60);
    EXPECT_EQ(std::get<double>(sasdoc1.values[5]), 2);
    EXPECT_EQ(std::get<double>(sasdoc1.values[6]), 20);
    EXPECT_EQ(std::get<double>(sasdoc1.values[7]), 25);
    EXPECT_EQ(std::get<double>(sasdoc1.values[8]), 30);
    EXPECT_EQ(std::get<double>(sasdoc1.values[9]), 135);
    EXPECT_EQ(std::get<double>(sasdoc1.values[10]), 3);
    EXPECT_EQ(std::get<double>(sasdoc1.values[11]), 25);
    EXPECT_EQ(std::get<double>(sasdoc1.values[12]), 30);
    EXPECT_EQ(std::get<double>(sasdoc1.values[13]), 35);
    EXPECT_EQ(std::get<double>(sasdoc1.values[14]), 225);

}