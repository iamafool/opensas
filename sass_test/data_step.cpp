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

TEST_F(SassTest, DataStepChar1) {
    std::string code = R"(
data out;
    length name $40;
    name="Alice"; output;
    name="  Bob  "; output;
    name="Charlie  "; output;
    name="Dana"; output;
run;
    )";

    // 1) Lex
    Lexer lexer(code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2) Parse
    Parser parser(tokens);
    auto parseResult = parser.parseProgram();
    ASSERT_TRUE(parseResult->statements.size() == 1);

    // 3) Interpret
    interpreter->executeProgram(parseResult);

    string libPath = env->getLibrary("WORK")->getPath();
    string filename = "out.sas7bdat";
    std::string filePath = (fs::path(libPath) / fs::path(filename)).string();
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    int rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 1);
    EXPECT_EQ(sasdoc1.getRowCount(), 4);
    EXPECT_EQ(sasdoc1.getColumnNames()[0], "name");

    Row row;
    row.columns = { {"name", "Alice"} }; 
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"name", "  Bob  "} }; 
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"name", "Charlie  "} };
    EXPECT_EQ(sasdoc1.rows[2], row);
    row.columns = { {"name", "Dana"} };
    EXPECT_EQ(sasdoc1.rows[3], row);
}

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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 1);
    EXPECT_EQ(sasdoc1.getRowCount(), 1);

    EXPECT_EQ(std::get<double>(sasdoc1.rows[0].columns["a"]), 10.0);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 2);
    EXPECT_EQ(sasdoc1.getRowCount(), 2);

    Row row;
    row.columns = { {"a", 10.0}, {"b", ""} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"a", 10.0}, {"b", "This is a string variable!"} };
    EXPECT_EQ(sasdoc1.rows[1], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 2);
    EXPECT_EQ(sasdoc1.getRowCount(), 2);

    Row row;
    row.columns = { {"name", "john"}, {"age", 23.0}};
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"name", "mary"}, {"age", 30.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 16);
    EXPECT_EQ(sasdoc1.getRowCount(), 5);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 5);
    EXPECT_EQ(sasdoc1.getRowCount(), 1);

    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "y");
    EXPECT_EQ(var_names[2], "sqrt_x");
    EXPECT_EQ(var_names[3], "abs_diff");
    EXPECT_EQ(var_names[4], "log_y");

    Row row;
    row.columns = { {"x", 16.0}, {"y", 30.0}, {"sqrt_x", 4.0}, {"abs_diff", 5.0}, {"log_y", 3.4011973817} };
    EXPECT_EQ(sasdoc1.rows[0], row);
}

TEST_F(SassTest, DataStepFunction2) {
    std::string code = R"(
data in;
    length name $40;
    name="Alice"; output;
    name="  Bob  "; output;
    name="Charlie  "; output;
    name="Dana"; output;
run;

data out; 
    set in; 
    first_part = substr(name, 1, 3);
    trimmed = trim(name);
    upper_name = upcase(name);
    lower_name = lowcase(name);
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
    EXPECT_TRUE(fs::exists(filePath)) << "Expected file does not exist at path: " << filePath;

    SasDoc sasdoc1;
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 5);
    EXPECT_EQ(sasdoc1.getRowCount(), 4);

    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "name");
    EXPECT_EQ(var_names[1], "first_part");
    EXPECT_EQ(var_names[2], "trimmed");
    EXPECT_EQ(var_names[3], "upper_name");
    EXPECT_EQ(var_names[4], "lower_name");

    Row row;
    row.columns = { {"name", "Alice"}, {"first_part", "Ali"}, {"trimmed", "Alice"}, {"upper_name", "ALICE"}, {"lower_name", "alice"} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"name", "  Bob  "}, {"first_part", "  B"}, {"trimmed", "  Bob"}, {"upper_name", "  BOB  "}, {"lower_name", "  bob  "} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"name", "Charlie  "}, {"first_part", "Cha"}, {"trimmed", "Charlie"}, {"upper_name", "CHARLIE  "}, {"lower_name", "charlie  "} };
    EXPECT_EQ(sasdoc1.rows[2], row);
    row.columns = { {"name", "Dana"}, {"first_part", "Dan"}, {"trimmed", "Dana"}, {"upper_name", "DANA"}, {"lower_name", "dana"} };
    EXPECT_EQ(sasdoc1.rows[3], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 3);
    EXPECT_EQ(sasdoc1.getRowCount(), 4);

    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "y");
    EXPECT_EQ(var_names[2], "status");

    Row row;
    row.columns = { {"x", 5.0}, {"y", 15.0}, {"status", "Low"} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 15.0}, {"y", 40.0}, {"status", "High"} };
    EXPECT_EQ(sasdoc1.rows[1], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 3);
    EXPECT_EQ(sasdoc1.getRowCount(), 4);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "y");
    EXPECT_EQ(var_names[2], "category");

    Row row;
    row.columns = { {"x", 3.0}, {"y", 15.0}, {"category", "Low"} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 7.0}, {"y", 25.0}, {"category", "Medium"} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 12.0}, {"y", 40.0}, {"category", "High"} };
    EXPECT_EQ(sasdoc1.rows[2], row);
    row.columns = { {"x", 18.0}, {"y", 75.0}, {"category", "Very High"} };
    EXPECT_EQ(sasdoc1.rows[3], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 4);
    EXPECT_EQ(sasdoc1.getRowCount(), 5);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "y");
    EXPECT_EQ(var_names[2], "category");
    EXPECT_EQ(var_names[3], "status");

    Row row;
    row.columns = { {"x", 3.0}, {"y", 10.0}, {"category", "Low"}, {"status", "Poor"} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 7.0}, {"y", 15.0}, {"category", "Medium"}, {"status", "Average"} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 12.0}, {"y", 18.0}, {"category", "High"}, {"status", "Fair"} };
    EXPECT_EQ(sasdoc1.rows[2], row);
    row.columns = { {"x", 16.0}, {"y", 35.0}, {"category", "Very High"}, {"status", "Excellent"} };
    EXPECT_EQ(sasdoc1.rows[3], row);
    row.columns = { {"x", 9.0}, {"y", 20.0}, {"category", "Medium"}, {"status", "Average"} };
    EXPECT_EQ(sasdoc1.rows[4], row);
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 2);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "num1");
    EXPECT_EQ(var_names[1], "num3");

    Row row;
    row.columns = { {"num1", 5.0}, {"num3", 15.0}};
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"num1", 10.0}, {"num3", 20.0}};
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"num1", 15.0}, {"num3", 25.0}};
    EXPECT_EQ(sasdoc1.rows[2], row);
}

TEST_F(SassTest, DataStepKeep1) {
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
    keep x num2;
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 2);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num2");

    Row row;
    row.columns = { {"x", 1.0}, {"num2", 10.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num2", 15.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num2", 20.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
}

TEST_F(SassTest, DataStepRetain1) {
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
    if num2 = 15 then sum = 1;
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 5);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");
    EXPECT_EQ(var_names[4], "sum");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 5.0}, {"num2", 10.0}, {"num3", 15.0}, {"sum", 0.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 10.0}, {"num2", 15.0}, {"num3", 20.0}, {"sum", 1.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 15.0}, {"num2", 20.0}, {"num3", 25.0}, {"sum", 1.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
}

TEST_F(SassTest, DataStepArray1) {
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
    array nums {3} num1 num2 num3;
    nums{1} = nums{2} + 10;
    sum = nums{1} + nums{2} + nums{3};
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 5);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");
    EXPECT_EQ(var_names[4], "sum");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 20.0}, {"num2", 10.0}, {"num3", 15.0}, {"sum", 45.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 25.0}, {"num2", 15.0}, {"num3", 20.0}, {"sum", 60.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 30.0}, {"num2", 20.0}, {"num3", 25.0}, {"sum", 75.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
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
    do i = 1 to 3;
        num1 = num1 + i;
        num2 = num2 + i * 2;
        num3 = num3 + i * 3;
        sum = num1 + num2 + num3;
    end;
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 6);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");
    EXPECT_EQ(var_names[4], "i");
    EXPECT_EQ(var_names[5], "sum");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 11.0}, {"num2", 22.0}, {"num3", 33.0}, {"sum", 66.0}, {"i", 4.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 16.0}, {"num2", 27.0}, {"num3", 38.0}, {"sum", 81.0}, {"i", 4.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 21.0}, {"num2", 32.0}, {"num3", 43.0}, {"sum", 96.0}, {"i", 4.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
}

TEST_F(SassTest, DataStepDo2) {
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
    auto rc = sasdoc1.load(wstring(filePath.begin(), filePath.end()));
    EXPECT_EQ(rc, 0) << "read_sas7bdat() failed for path: " << filePath;

    EXPECT_EQ(sasdoc1.getColumnCount(), 5);
    EXPECT_EQ(sasdoc1.getRowCount(), 3);
    std::vector<std::string> var_names = sasdoc1.getColumnNames();
    EXPECT_EQ(var_names[0], "x");
    EXPECT_EQ(var_names[1], "num1");
    EXPECT_EQ(var_names[2], "num2");
    EXPECT_EQ(var_names[3], "num3");
    EXPECT_EQ(var_names[4], "sum");

    Row row;
    row.columns = { {"x", 1.0}, {"num1", 15.0}, {"num2", 20.0}, {"num3", 25.0}, {"sum", 60.0} };
    EXPECT_EQ(sasdoc1.rows[0], row);
    row.columns = { {"x", 2.0}, {"num1", 20.0}, {"num2", 25.0}, {"num3", 30.0}, {"sum", 135.0} };
    EXPECT_EQ(sasdoc1.rows[1], row);
    row.columns = { {"x", 3.0}, {"num1", 25.0}, {"num2", 30.0}, {"num3", 35.0}, {"sum", 225.0} };
    EXPECT_EQ(sasdoc1.rows[2], row);
}
