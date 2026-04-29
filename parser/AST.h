#pragma once
#include <memory>
#include <string>
#include <vector>

// Базовый класс для всех узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

// Узел программы (корень)
class ProgramNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;
    ProgramNode(const std::vector<std::shared_ptr<ASTNode>>& stmts) 
        : statements(stmts) {}
};

// ===== ВЫРАЖЕНИЯ =====

class NumberNode : public ASTNode {
public:
    double value;
    NumberNode(double val) : value(val) {}
};

class StringNode : public ASTNode {
public:
    std::string value;
    StringNode(const std::string& val) : value(val) {}
};

class VariableNode : public ASTNode {
public:
    std::string name;
    VariableNode(const std::string& n) : name(n) {}
};

class BinaryOpNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> left;
    std::string op;
    std::shared_ptr<ASTNode> right;
    
    BinaryOpNode(std::shared_ptr<ASTNode> l, const std::string& o, std::shared_ptr<ASTNode> r)
        : left(l), op(o), right(r) {}
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> operand;
    
    UnaryOpNode(const std::string& o, std::shared_ptr<ASTNode> opd)
        : op(o), operand(opd) {}
};

// ===== ОПЕРАТОРЫ =====

class AssignmentNode : public ASTNode {
public:
    std::string variable;
    std::shared_ptr<ASTNode> value;
    
    AssignmentNode(const std::string& var, std::shared_ptr<ASTNode> val)
        : variable(var), value(val) {}
};

class ReadNode : public ASTNode {
public:
    std::string variable;
    ReadNode(const std::string& var) : variable(var) {}
};

class PrintNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> expression;
    PrintNode(std::shared_ptr<ASTNode> expr) : expression(expr) {}
};

class IfNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> thenBranch;
    std::vector<std::shared_ptr<ASTNode>> elseBranch;
    
    IfNode(std::shared_ptr<ASTNode> cond, 
           const std::vector<std::shared_ptr<ASTNode>>& then,
           const std::vector<std::shared_ptr<ASTNode>>& els)
        : condition(cond), thenBranch(then), elseBranch(els) {}
};

class WhileNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> body;
    
    WhileNode(std::shared_ptr<ASTNode> cond, const std::vector<std::shared_ptr<ASTNode>>& b)
        : condition(cond), body(b) {}
};

class BlockNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;
    BlockNode(const std::vector<std::shared_ptr<ASTNode>>& stmts) 
        : statements(stmts) {}
};
