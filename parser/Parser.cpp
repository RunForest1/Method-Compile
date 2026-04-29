#include "Parser.h"
#include <sstream>

// ================= ИНИЦИАЛИЗАЦИЯ =================

Parser::Parser(const std::string& code) 
    : lexer(code), labelCounter(0), nextLabelId(0) {
    advance(); // Загружаем первый токен
}

void Parser::advance() {
    currentToken = lexer.nextToken();
}

// ================= ВСПОМОГАТЕЛЬНЫЕ МЕТОДЫ =================

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) {
    return currentToken.type == type;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = currentToken;
        advance();
        return token;
    }
    error(message + " at line " + std::to_string(currentToken.line));
    return Token(T_ERROR, "", currentToken.line, currentToken.column);
}

void Parser::error(const std::string& message) {
    std::stringstream ss;
    ss << "Error at line " << currentToken.line 
       << ", column " << currentToken.column << ": " << message;
    errors.push_back(ss.str());
}

// ================= ГЕНЕРАЦИЯ ОПС =================

void Parser::emitInstruction(OpType op, const std::string& arg) {
    opsProgram.push_back(Instruction(op, arg));
}

void Parser::emitJump(OpType op, int& target) {
    opsProgram.push_back(Instruction(op, "", target));
}

int Parser::getNextLabel() {
    return nextLabelId++;
}

// ================= ПАРСИНГ ПРОГРАММЫ =================

std::shared_ptr<ProgramNode> Parser::parseProgram() {
    std::vector<std::shared_ptr<ASTNode>> statements;
    
    while (!check(T_EOF) && !hasErrors()) {
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
    }
    
    emitInstruction(OpType::OP_END);
    return std::make_shared<ProgramNode>(statements);
}

std::shared_ptr<ProgramNode> Parser::parse() {
    return parseProgram();
}

// ================= ПАРСИНГ БЛОКОВ =================

std::vector<std::shared_ptr<ASTNode>> Parser::parseBlock() {
    std::vector<std::shared_ptr<ASTNode>> statements;
    
    while (!check(T_EOF) && !hasErrors()) {
        if (currentToken.type == T_KEYWORD && 
            (currentToken.value == "END" || currentToken.value == "ELSE")) {
            break;
        }
        
        auto stmt = parseStatement();
        if (stmt) {
            statements.push_back(stmt);
        }
    }
    
    return statements;
}

// ================= ПАРСИНГ ОПЕРАТОРОВ (STATEMENTS) =================

std::shared_ptr<ASTNode> Parser::parseStatement() {
    if (check(T_KEYWORD)) {
        if (currentToken.value == "READ") {
            return parseReadStatement();
        }
        if (currentToken.value == "PRINT") {
            return parsePrintStatement();
        }
        if (currentToken.value == "IF") {
            return parseIfStatement();
        }
        if (currentToken.value == "WHILE") {
            return parseWhileStatement();
        }
    }
    
    if (check(T_ID)) {
        return parseAssignment();
    }
    
    if (check(T_SEPARATOR) && currentToken.value == ";") {
        advance();
        return nullptr;
    }
    
    error("Unexpected token: " + currentToken.value);
    advance();
    return nullptr;
}

// READ переменная ;
std::shared_ptr<ASTNode> Parser::parseReadStatement() {
    consume(T_KEYWORD, "Expected READ");
    Token varToken = consume(T_ID, "Expected variable name after READ");
    consume(T_SEPARATOR, "Expected ';' after READ statement");
    
    if (varToken.type == T_ID) {
        emitInstruction(OpType::OP_READ);
        emitInstruction(OpType::OP_POP_VAR, varToken.value);
        return std::make_shared<ReadNode>(varToken.value);
    }
    return nullptr;
}

// PRINT выражение ;
std::shared_ptr<ASTNode> Parser::parsePrintStatement() {
    consume(T_KEYWORD, "Expected PRINT");
    auto expr = parseExpression();
    consume(T_SEPARATOR, "Expected ';' after PRINT statement");
    
    emitInstruction(OpType::OP_PRINT);
    
    return std::make_shared<PrintNode>(expr);
}

// IF условие THEN блок [ELSE блок] END;
std::shared_ptr<ASTNode> Parser::parseIfStatement() {
    consume(T_KEYWORD, "Expected IF");
    auto condition = parseExpression();
    consume(T_KEYWORD, "Expected THEN after IF condition");
    
    // Генерируем условный прыжок
    int ifLabel = getNextLabel();
    int elseLabel = getNextLabel();
    
    emitInstruction(OpType::OP_JMP_FALSE, "");
    int jumpIfFalse = opsProgram.size() - 1;
    
    auto thenBlock = parseBlock();
    
    emitInstruction(OpType::OP_JMP, "");
    int jumpToEnd = opsProgram.size() - 1;
    
    int elseLabelPos = opsProgram.size();
    
    std::vector<std::shared_ptr<ASTNode>> elseBlock;
    if (check(T_KEYWORD) && currentToken.value == "ELSE") {
        advance();
        elseBlock = parseBlock();
    }
    
    opsProgram[jumpIfFalse].jumpTarget = elseLabelPos;
    opsProgram[jumpToEnd].jumpTarget = opsProgram.size();
    
    consume(T_KEYWORD, "Expected END to close IF");
    consume(T_SEPARATOR, "Expected ';' after IF statement");
    
    return std::make_shared<IfNode>(condition, thenBlock, elseBlock);
}

// WHILE условие DO блок END;
std::shared_ptr<ASTNode> Parser::parseWhileStatement() {
    consume(T_KEYWORD, "Expected WHILE");
    
    int loopStart = opsProgram.size();
    
    auto condition = parseExpression();
    consume(T_KEYWORD, "Expected DO after WHILE condition");
    
    emitInstruction(OpType::OP_JMP_FALSE, "");
    int jumpIfFalse = opsProgram.size() - 1;
    
    auto body = parseBlock();
    
    emitInstruction(OpType::OP_JMP, loopStart);
    
    opsProgram[jumpIfFalse].jumpTarget = opsProgram.size();
    
    consume(T_KEYWORD, "Expected END to close WHILE");
    consume(T_SEPARATOR, "Expected ';' after WHILE statement");
    
    return std::make_shared<WhileNode>(condition, body);
}

// переменная = выражение ;
std::shared_ptr<ASTNode> Parser::parseAssignment() {
    Token varToken = consume(T_ID, "Expected variable name");
    consume(T_OPERATOR, "Expected '=' in assignment");
    auto expr = parseExpression();
    consume(T_SEPARATOR, "Expected ';' after assignment");
    
    emitInstruction(OpType::OP_POP_VAR, varToken.value);
    
    if (varToken.type == T_ID) {
        return std::make_shared<AssignmentNode>(varToken.value, expr);
    }
    return nullptr;
}

// ================= ПАРСИНГ ВЫРАЖЕНИЙ =================

std::shared_ptr<ASTNode> Parser::parseExpression() {
    return parseLogicalOr();
}

std::shared_ptr<ASTNode> Parser::parseLogicalOr() {
    auto left = parseLogicalAnd();
    
    while (check(T_OPERATOR) && currentToken.value == "||") {
        advance();
        auto right = parseLogicalAnd();
        emitInstruction(OpType::OP_OR);
        left = std::make_shared<BinaryOpNode>(left, "||", right);
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto left = parseComparison();
    
    while (check(T_OPERATOR) && currentToken.value == "&&") {
        advance();
        auto right = parseComparison();
        emitInstruction(OpType::OP_AND);
        left = std::make_shared<BinaryOpNode>(left, "&&", right);
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::parseComparison() {
    auto left = parseAdditive();
    
    while (check(T_OPERATOR) && 
           (currentToken.value == "<" || currentToken.value == ">" ||
            currentToken.value == "<=" || currentToken.value == ">=" ||
            currentToken.value == "==" || currentToken.value == "!=")) {
        std::string op = currentToken.value;
        advance();
        auto right = parseAdditive();
        
        if (op == "<") emitInstruction(OpType::OP_LT);
        else if (op == ">") emitInstruction(OpType::OP_GT);
        else if (op == "<=") emitInstruction(OpType::OP_LE);
        else if (op == ">=") emitInstruction(OpType::OP_GE);
        else if (op == "==") emitInstruction(OpType::OP_EQ);
        else if (op == "!=") emitInstruction(OpType::OP_NE);
        
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::parseAdditive() {
    auto left = parseMultiplicative();
    
    while (check(T_OPERATOR) && 
           (currentToken.value == "+" || currentToken.value == "-")) {
        std::string op = currentToken.value;
        advance();
        auto right = parseMultiplicative();
        
        if (op == "+") emitInstruction(OpType::OP_ADD);
        else if (op == "-") emitInstruction(OpType::OP_SUB);
        
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::parseMultiplicative() {
    auto left = parseUnary();
    
    while (check(T_OPERATOR) && 
           (currentToken.value == "*" || currentToken.value == "/")) {
        std::string op = currentToken.value;
        advance();
        auto right = parseUnary();
        
        if (op == "*") emitInstruction(OpType::OP_MUL);
        else if (op == "/") emitInstruction(OpType::OP_DIV);
        
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    
    return left;
}

std::shared_ptr<ASTNode> Parser::parseUnary() {
    if (check(T_OPERATOR) && 
        (currentToken.value == "!" || currentToken.value == "-")) {
        std::string op = currentToken.value;
        advance();
        auto operand = parseUnary();
        
        if (op == "!") emitInstruction(OpType::OP_NOT);
        else if (op == "-") emitInstruction(OpType::OP_NEG);
        
        return std::make_shared<UnaryOpNode>(op, operand);
    }
    
    return parsePrimary();
}

std::shared_ptr<ASTNode> Parser::parsePrimary() {
    // Целые и дробные числа
    if (check(T_INT) || check(T_FLOAT)) {
        std::string value = currentToken.value;
        bool isFloat = check(T_FLOAT);
        advance();
        
        if (isFloat || value.find('.') != std::string::npos) {
            emitInstruction(OpType::OP_PUSH_FLOAT, value);
        } else {
            emitInstruction(OpType::OP_PUSH_INT, value);
        }
        
        return std::make_shared<NumberNode>(std::stod(value));
    }
    
    // Строки
    if (check(T_STRING)) {
        std::string value = currentToken.value;
        advance();
        emitInstruction(OpType::OP_PUSH_STRING, value);
        return std::make_shared<StringNode>(value);
    }
    
    // Переменные
    if (check(T_ID)) {
        std::string name = currentToken.value;
        advance();
        emitInstruction(OpType::OP_PUSH_VAR, name);
        return std::make_shared<VariableNode>(name);
    }
    
    // Выражения в скобках
    if (check(T_SEPARATOR) && currentToken.value == "(") {
        advance();
        auto expr = parseExpression();
        consume(T_SEPARATOR, "Expected ')' after expression");
        return expr;
    }
    
    error("Unexpected token in expression: " + currentToken.value);
    advance();
    return nullptr;
}
