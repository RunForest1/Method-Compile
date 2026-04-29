#pragma once
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "../lexer/Lexer.h"
#include "../interpreter/Interpreter.h"
#include "AST.h"

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::vector<std::string> errors;
    Program opsProgram;
    int labelCounter;
    std::map<std::string, int> labels;  // метки для прыжков
    int nextLabelId;

    // Вспомогательные методы
    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    void error(const std::string& message);

    // Парсинг выражений (по приоритету операторов)
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseLogicalOr();
    std::shared_ptr<ASTNode> parseLogicalAnd();
    std::shared_ptr<ASTNode> parseComparison();
    std::shared_ptr<ASTNode> parseAdditive();
    std::shared_ptr<ASTNode> parseMultiplicative();
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePrimary();

    // Генерация кода для выражений
    void generateExpressionCode(std::shared_ptr<ASTNode> node);

    // Парсинг операторов (statements)
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseReadStatement();
    std::shared_ptr<ASTNode> parsePrintStatement();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseArrayDeclaration();

    // Парсинг блоков
    std::shared_ptr<ProgramNode> parseProgram();
    std::vector<std::shared_ptr<ASTNode>> parseBlock();

    // Генерация ОПС
    void emitInstruction(OpType op, const std::string& arg = "");
    void emitJump(OpType op, int& target);
    int getNextLabel();

public:
    Parser(const std::string& code);
    std::shared_ptr<ProgramNode> parse();
    Program getOPS() const { return opsProgram; }
    std::vector<std::string> getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
};
