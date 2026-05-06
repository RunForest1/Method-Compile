#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer &lexer) : lexer(lexer)
{
    initSemanticTable();
    initMatrix();
}

int Parser::getExtendedType(const Lexem &l)
{
    if (l.type == LexemType::L_KEYWORD)
    {
        if (l.value == "if") return 1001;
        if (l.value == "then") return 1002;
        if (l.value == "else") return 1003;
        if (l.value == "while") return 1004;
        if (l.value == "do") return 1005;
        if (l.value == "read") return 1006;
        if (l.value == "write") return 1007;
    }
    if (l.type == LexemType::L_DELIMITER)
    {
        if (l.value == "(") return 2001;
        if (l.value == ")") return 2002;
        if (l.value == "[") return 2003;
        if (l.value == "]") return 2004;
        if (l.value == ";") return 2005;
        if (l.value == ",") return 2006;
    }
    return static_cast<int>(l.type);
}

void Parser::initMatrix()
{
    auto ID = static_cast<LexemType>(LexemType::L_ID);
    auto INT = static_cast<LexemType>(LexemType::L_INT);
    auto FLOAT = static_cast<LexemType>(LexemType::L_FLOAT);
    auto STRING = static_cast<LexemType>(LexemType::L_STRING);
    auto ASSIGN = static_cast<LexemType>(LexemType::L_ASSIGNMENT_OPERATOR);
    auto ADD_OP = static_cast<LexemType>(LexemType::L_ADDITIVE_OPERATOR);
    auto MULT_OP = static_cast<LexemType>(LexemType::L_MULTIPLICATIVE_OPERATOR);
    auto COMP_OP = static_cast<LexemType>(LexemType::L_COMPARISON_OPERATOR);
    auto TERM = static_cast<LexemType>(LexemType::L_TERMINATOR);
    
    auto IF = static_cast<LexemType>(1001);
    auto THEN = static_cast<LexemType>(1002);
    auto ELSE = static_cast<LexemType>(1003);
    auto WHILE = static_cast<LexemType>(1004);
    auto DO = static_cast<LexemType>(1005);
    auto READ = static_cast<LexemType>(1006);
    auto WRITE = static_cast<LexemType>(1007);
    auto LPAR = static_cast<LexemType>(2001);
    auto RPAR = static_cast<LexemType>(2002);
    auto LBRACK = static_cast<LexemType>(2003);
    auto RBRACK = static_cast<LexemType>(2004);
    auto SEMI = static_cast<LexemType>(2005);
    auto COMMA = static_cast<LexemType>(2006);

    // --- Program ---
    for (auto t : {ID, IF, WHILE, READ, WRITE})
    {
        M[NonTerm::Program][t] = {
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Statement)}, 
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Program)}    
        };
    }
    M[NonTerm::Program][TERM] = {}; 
    M[NonTerm::Program][ELSE] = {}; 

    // --- Statement ---
    
    // Write: write ( Expression ) ;
    M[NonTerm::Statement][WRITE] = {
        {SymbolType::TERMINAL, static_cast<int>(WRITE)},
        {SymbolType::TERMINAL, static_cast<int>(LPAR)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::SEMANTIC_ACTION, "w"}, 
        {SymbolType::TERMINAL, static_cast<int>(RPAR)},
        {SymbolType::TERMINAL, static_cast<int>(SEMI)}
    };

    // Read: read ( ID ArrayIndex ) ;
    M[NonTerm::Statement][READ] = {
        {SymbolType::TERMINAL, static_cast<int>(READ)},
        {SymbolType::TERMINAL, static_cast<int>(LPAR)},
        {SymbolType::TERMINAL, static_cast<int>(ID)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ArrayIndex)},
        {SymbolType::SEMANTIC_ACTION, "a"}, 
        {SymbolType::TERMINAL, static_cast<int>(RPAR)},
        {SymbolType::SEMANTIC_ACTION, "r"}, 
        {SymbolType::TERMINAL, static_cast<int>(SEMI)}
    };

    // If: if Condition then Statement ElsePart SemanticTrigger
    M[NonTerm::Statement][IF] = {
        {SymbolType::TERMINAL, static_cast<int>(IF)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Condition)},
        {SymbolType::SEMANTIC_ACTION, "1"}, // P1: JF
        {SymbolType::TERMINAL, static_cast<int>(THEN)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Statement)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ElsePart)},
        {SymbolType::SEMANTIC_ACTION, "3"}  // P3: Patch JF
    };

    // While: while Condition do Statement SemanticTrigger
    M[NonTerm::Statement][WHILE] = {
        {SymbolType::SEMANTIC_ACTION, "4"}, // P4: Start Loop Label
        {SymbolType::TERMINAL, static_cast<int>(WHILE)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Condition)},
        {SymbolType::SEMANTIC_ACTION, "1"}, // P1: JF (Exit)
        {SymbolType::TERMINAL, static_cast<int>(DO)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Statement)},
        {SymbolType::SEMANTIC_ACTION, "5"}  // P5: Jump to Start
    };

    // Assignment: ID ArrayIndex := Expression ;
    M[NonTerm::Statement][ID] = {
        {SymbolType::TERMINAL, static_cast<int>(ID)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ArrayIndex)},
        {SymbolType::SEMANTIC_ACTION, "a"}, 
        {SymbolType::TERMINAL, static_cast<int>(ASSIGN)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::SEMANTIC_ACTION, ":="}, 
        {SymbolType::TERMINAL, static_cast<int>(SEMI)}
    };

    // --- ElsePart ---
    M[NonTerm::ElsePart][ELSE] = {
        {SymbolType::TERMINAL, static_cast<int>(ELSE)},
        {SymbolType::SEMANTIC_ACTION, "2"}, // P2: Jump over else
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Statement)}
    };
    for (auto t : {SEMI, ELSE, TERM, RPAR}) 
    {
        M[NonTerm::ElsePart][t] = {};
    }

    // --- Condition ---
    // Condition -> Expression CompOp Expression SemanticTrigger
    auto condStarters = {ID, INT, FLOAT, STRING, LPAR, ADD_OP};
    for (auto t : condStarters)
    {
        M[NonTerm::Condition][t] = {
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
            {SymbolType::TERMINAL, static_cast<int>(COMP_OP)},
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
            {SymbolType::SEMANTIC_ACTION, "op_comp"} // Запись оператора сравнения
        };
    }

    // --- Expression & Term ---
    for (auto t : condStarters)
    {
        M[NonTerm::Expression][t] = {
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Term)},
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ExpressionTail)}
        };
    }

    M[NonTerm::ExpressionTail][ADD_OP] = {
        {SymbolType::TERMINAL, static_cast<int>(ADD_OP)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Term)},
        {SymbolType::SEMANTIC_ACTION, "op_add_sub"}, 
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ExpressionTail)}
    };
    for (auto t : {SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::ExpressionTail][t] = {};

    for (auto t : condStarters)
    {
        M[NonTerm::Term][t] = {
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Factor)},
            {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::TermTail)}
        };
    }

    M[NonTerm::TermTail][MULT_OP] = {
        {SymbolType::TERMINAL, static_cast<int>(MULT_OP)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Factor)},
        {SymbolType::SEMANTIC_ACTION, "op_mul_div"}, 
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::TermTail)}
    };
    for (auto t : {ADD_OP, SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::TermTail][t] = {};

    // --- Factor ---
    M[NonTerm::Factor][LPAR] = {
        {SymbolType::TERMINAL, static_cast<int>(LPAR)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::TERMINAL, static_cast<int>(RPAR)}
    };
    
    M[NonTerm::Factor][ADD_OP] = {
        {SymbolType::TERMINAL, static_cast<int>(ADD_OP)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::UnaryOperand)},
        {SymbolType::SEMANTIC_ACTION, "unary_op"} 
    };

    M[NonTerm::Factor][ID] = {
        {SymbolType::TERMINAL, static_cast<int>(ID)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ArrayIndex)},
        {SymbolType::SEMANTIC_ACTION, "a"}
    };

    M[NonTerm::Factor][INT] = {
        {SymbolType::TERMINAL, static_cast<int>(INT)},
        {SymbolType::SEMANTIC_ACTION, "k"}
    };
    M[NonTerm::Factor][FLOAT] = {
        {SymbolType::TERMINAL, static_cast<int>(FLOAT)},
        {SymbolType::SEMANTIC_ACTION, "k"}
    };
    M[NonTerm::Factor][STRING] = {
        {SymbolType::TERMINAL, static_cast<int>(STRING)},
        {SymbolType::SEMANTIC_ACTION, "k"}
    };

    // --- UnaryOperand ---
    M[NonTerm::UnaryOperand][LPAR] = {
        {SymbolType::TERMINAL, static_cast<int>(LPAR)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::TERMINAL, static_cast<int>(RPAR)}
    };
    M[NonTerm::UnaryOperand][ID] = {
        {SymbolType::TERMINAL, static_cast<int>(ID)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ArrayIndex)},
        {SymbolType::SEMANTIC_ACTION, "a"}
    };
    M[NonTerm::UnaryOperand][INT] = {
        {SymbolType::TERMINAL, static_cast<int>(INT)},
        {SymbolType::SEMANTIC_ACTION, "k"}
    };
    M[NonTerm::UnaryOperand][FLOAT] = {
        {SymbolType::TERMINAL, static_cast<int>(FLOAT)},
        {SymbolType::SEMANTIC_ACTION, "k"}
    };

    // --- ArrayIndex & ArrayTail ---
    M[NonTerm::ArrayIndex][LBRACK] = {
        {SymbolType::TERMINAL, static_cast<int>(LBRACK)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::ArrayTail)}
    };
    for (auto t : {ASSIGN, SEMI, ADD_OP, MULT_OP, COMP_OP, RPAR, COMMA, THEN, DO, ELSE, RBRACK})
    {
        M[NonTerm::ArrayIndex][t] = {};
    }

    M[NonTerm::ArrayTail][RBRACK] = {
        {SymbolType::TERMINAL, static_cast<int>(RBRACK)},
        {SymbolType::SEMANTIC_ACTION, "i"} 
    };
    M[NonTerm::ArrayTail][COMMA] = {
        {SymbolType::TERMINAL, static_cast<int>(COMMA)},
        {SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Expression)},
        {SymbolType::TERMINAL, static_cast<int>(RBRACK)},
        {SymbolType::SEMANTIC_ACTION, "i2"} 
    };

    // --- SemanticTrigger ---
    for (auto t : {SEMI, RPAR, RBRACK, COMMA, THEN, DO, ELSE, TERM})
    {
        M[NonTerm::SemanticTrigger][t] = {};
    }
}

void Parser::initSemanticTable()
{
    // Арифметика и сравнения используют prevLexem, так как действие вызывается сразу после чтения оператора
    semanticActions["op_add_sub"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, prevLexem.value}); 
    };
    semanticActions["op_mul_div"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, prevLexem.value}); 
    };
    semanticActions["op_comp"] = [this]() {
        rpn.push_back({RpnElementType::OPERATOR, prevLexem.value}); 
    };
    
    semanticActions["unary_op"] = [this]() { 
        if (prevLexem.value == "-") {
            rpn.push_back({RpnElementType::OPERATOR, "NEG"}); 
        }
    };

    semanticActions["a"] = [this]() { 
        rpn.push_back({RpnElementType::ADDR_VAR, prevLexem.value}); 
    };
    semanticActions["k"] = [this]() { 
        rpn.push_back({RpnElementType::CONST_VAL, prevLexem.value}); 
    };
    
    semanticActions[":="] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, ":="}); 
    };
    
    semanticActions["r"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, "READ"}); 
    };
    semanticActions["w"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, "WRITE"}); 
    };
    
    semanticActions["i"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, "INDEX1"}); 
    };
    semanticActions["i2"] = [this]() { 
        rpn.push_back({RpnElementType::OPERATOR, "INDEX2"}); 
    };

    // Метки P1-P5
    semanticActions["1"] = [this]() {
        labelStack.push(rpn.size());
        rpn.push_back({RpnElementType::LABEL, ""}); 
        rpn.push_back({RpnElementType::OPERATOR, "JF"});
    };
    
    semanticActions["2"] = [this]() {
        if (labelStack.empty()) {
            throw std::runtime_error("Семантическая ошибка: стек меток пуст при выполнении действия '2' (Else)");
        }
        int prevLabel = labelStack.top();
        labelStack.pop();
        rpn[prevLabel].value = std::to_string(rpn.size() + 2); 
        
        labelStack.push(rpn.size()); 
        rpn.push_back({RpnElementType::LABEL, ""}); 
        rpn.push_back({RpnElementType::OPERATOR, "J"});
    };
    
    semanticActions["3"] = [this]() {
        if (labelStack.empty()) {
            throw std::runtime_error("Семантическая ошибка: стек меток пуст при выполнении действия '3' (End If)");
        }
        int label = labelStack.top();
        labelStack.pop();
        rpn[label].value = std::to_string(rpn.size());
    };
    
    semanticActions["4"] = [this]() { 
        labelStack.push(rpn.size()); 
    };
    
    semanticActions["5"] = [this]() {
        if (labelStack.size() < 2) { 
            throw std::runtime_error("Семантическая ошибка: недостаточно меток в стеке для действия '5' (End While)");
        }
        int exitLabel = labelStack.top(); 
        labelStack.pop();
        int startLoop = labelStack.top(); 
        labelStack.pop();
        
        rpn[exitLabel].value = std::to_string(rpn.size() + 2);
        
        rpn.push_back({RpnElementType::LABEL, std::to_string(startLoop)});
        rpn.push_back({RpnElementType::OPERATOR, "J"});
    };
}

std::vector<RpnElement> Parser::parse()
{
    parseStack.push({SymbolType::NON_TERMINAL, static_cast<int>(NonTerm::Program)});
    currentLexem = lexer.getNextLexem();
    
    while (!parseStack.empty())
    {
        StackSymbol top = parseStack.top();
        parseStack.pop();

        if (top.type == SymbolType::TERMINAL)
        {
            int expectedType = top.id;
            int actualType = getExtendedType(currentLexem);
            
            if (actualType == expectedType)
            {
                prevLexem = currentLexem;
                currentLexem = lexer.getNextLexem();
            }
            else
            {
                std::string msg = "Синтаксическая ошибка: ожидался токен типа " + std::to_string(expectedType) +
                                  ", но найдено '" + currentLexem.value + "' (тип " + std::to_string(actualType) + ")";
                throw std::runtime_error(msg);
            }
        }
        else if (top.type == SymbolType::SEMANTIC_ACTION)
        {
            executeAction(top.action);
        }
        else if (top.type == SymbolType::NON_TERMINAL)
        {
            pushProduction(static_cast<NonTerm>(top.id), currentLexem.type);
        }
    }
    return rpn;
}

void Parser::executeAction(const std::string &action)
{
    if (semanticActions.count(action))
    {
        semanticActions[action]();
    }
    else
    {
        throw std::runtime_error("Ошибка выполнения: неизвестное семантическое действие '" + action + "'");
    }
}

void Parser::pushProduction(NonTerm nt, LexemType lt)
{
    int extType = getExtendedType(currentLexem);
    
    if (M.count(nt) && M[nt].count(static_cast<LexemType>(extType)))
    {
        const Production &prod = M[nt][static_cast<LexemType>(extType)];
        for (auto it = prod.rbegin(); it != prod.rend(); ++it)
        {
            parseStack.push(*it);
        }
    }
    else
    {
        if (M.count(nt) && M[nt].count(static_cast<LexemType>(extType)) && M[nt][static_cast<LexemType>(extType)].empty())
        {
             return;
        }

        std::string msg = "Ошибка матрицы разбора: нет правила для нетерминала " + std::to_string(static_cast<int>(nt)) +
                          " с текущей лексемой '" + currentLexem.value + "' (тип " + std::to_string(extType) + ")";
        throw std::runtime_error(msg);
    }
}