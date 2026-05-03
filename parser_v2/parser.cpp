#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <set>
#include <stdexcept>

void FixLocale();

// 1. Перечисление всех терминалов (лексем)
enum class Terminal {
    L_ID,
    L_INT,
    L_FLOAT,
    L_STRING,
    L_ADDITIVE_OPERATOR,
    L_MULTIPLICATIVE_OPERATOR,
    L_COMPARISON_OPERATOR,
    L_ASSIGNMENT_OPERATOR,
    KW_IF, KW_THEN, KW_ELSE, KW_WHILE, KW_DO, KW_READ, KW_WRITE,
    L_LPAREN, L_RPAREN, L_LBRACKET, L_RBRACKET, L_SEMICOLON, L_COMMA,
    L_EOF, // Конец файла / ⊥
    LAMBDA // λ-переход
};

// 2. Перечисление всех нетерминалов
enum class NonTerminal {
    Program, StmtList, StmtTail, Stmt, IfTail,
    Variable, VarExt, IndexTail, Expr, ExprTail,
    TermTail, Factor, Condition, WriteArg
};

// 3. Общий символ для магазинной памяти
struct Symbol {
    bool isTerminal;
    Terminal term;
    NonTerminal nonTerm;

    static Symbol fromTerminal(Terminal t) { return {true, t, NonTerminal::Program}; }
    static Symbol fromNonTerminal(NonTerminal nt) { return {false, Terminal::L_ID, nt}; }
};

// Структура токена из лексического анализатора
struct Token {
    Terminal type;
    std::string value;
    int line;
    int column;
};

// 4. Правила грамматики (Каждое правило — это вектор символов правой части)
using Production = std::vector<Symbol>;

// Идентификаторы правил для удобства построения таблицы
enum class RuleId {
    R_None,
    R_Prog,
    R_StmtList_ID, R_StmtList_IF, R_StmtList_WHILE, R_StmtList_READ, R_StmtList_WRITE,
    R_StmtTail_Semi, R_StmtTail_Lambda,
    R_Stmt_ID, R_Stmt_IF, R_Stmt_WHILE, R_Stmt_READ, R_Stmt_WRITE,
    R_IfTail_Else, R_IfTail_Lambda,
    R_Variable,
    R_VarExt_Bracket, R_VarExt_Lambda,
    R_IndexTail_Comma, R_IndexTail_Lambda,
    R_Expr_Paren, R_Expr_ID, R_Expr_INT, R_Expr_FLOAT,
    R_ExprTail_Op, R_ExprTail_Lambda,
    R_TermTail_Op, R_TermTail_Lambda,
    R_Factor_Paren, R_Factor_ID, R_Factor_INT, R_Factor_FLOAT,
    R_Condition,
    R_WriteArg_Paren, R_WriteArg_ID, R_WriteArg_INT, R_WriteArg_FLOAT, R_WriteArg_String
};

// Глобальная база правил
std::map<RuleId, Production> GrammarRules = {
    { RuleId::R_Prog, { Symbol::fromNonTerminal(NonTerminal::StmtList) } },

    // StmtList
    { RuleId::R_StmtList_ID,    { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt), Symbol::fromTerminal(Terminal::L_ASSIGNMENT_OPERATOR), Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromNonTerminal(NonTerminal::StmtTail) } },
    { RuleId::R_StmtList_IF,    { Symbol::fromTerminal(Terminal::KW_IF), Symbol::fromNonTerminal(NonTerminal::Condition), Symbol::fromTerminal(Terminal::KW_THEN), Symbol::fromNonTerminal(NonTerminal::Stmt), Symbol::fromNonTerminal(NonTerminal::IfTail), Symbol::fromNonTerminal(NonTerminal::StmtTail) } },
    { RuleId::R_StmtList_WHILE, { Symbol::fromTerminal(Terminal::KW_WHILE), Symbol::fromNonTerminal(NonTerminal::Condition), Symbol::fromTerminal(Terminal::KW_DO), Symbol::fromNonTerminal(NonTerminal::Stmt), Symbol::fromNonTerminal(NonTerminal::StmtTail) } },
    { RuleId::R_StmtList_READ,  { Symbol::fromTerminal(Terminal::KW_READ), Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::Variable), Symbol::fromTerminal(Terminal::L_RPAREN), Symbol::fromNonTerminal(NonTerminal::StmtTail) } },
    { RuleId::R_StmtList_WRITE, { Symbol::fromTerminal(Terminal::KW_WRITE), Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::WriteArg), Symbol::fromTerminal(Terminal::L_RPAREN), Symbol::fromNonTerminal(NonTerminal::StmtTail) } },

    // StmtTail
    { RuleId::R_StmtTail_Semi,   { Symbol::fromTerminal(Terminal::L_SEMICOLON), Symbol::fromNonTerminal(NonTerminal::StmtList) } },
    { RuleId::R_StmtTail_Lambda, { Symbol::fromTerminal(Terminal::LAMBDA) } },

    // Stmt
    { RuleId::R_Stmt_ID,    { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt), Symbol::fromTerminal(Terminal::L_ASSIGNMENT_OPERATOR), Symbol::fromNonTerminal(NonTerminal::Expr) } },
    { RuleId::R_Stmt_IF,    { Symbol::fromTerminal(Terminal::KW_IF), Symbol::fromNonTerminal(NonTerminal::Condition), Symbol::fromTerminal(Terminal::KW_THEN), Symbol::fromNonTerminal(NonTerminal::Stmt), Symbol::fromNonTerminal(NonTerminal::IfTail) } },
    { RuleId::R_Stmt_WHILE, { Symbol::fromTerminal(Terminal::KW_WHILE), Symbol::fromNonTerminal(NonTerminal::Condition), Symbol::fromTerminal(Terminal::KW_DO), Symbol::fromNonTerminal(NonTerminal::Stmt) } },
    { RuleId::R_Stmt_READ,  { Symbol::fromTerminal(Terminal::KW_READ), Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::Variable), Symbol::fromTerminal(Terminal::L_RPAREN) } },
    { RuleId::R_Stmt_WRITE, { Symbol::fromTerminal(Terminal::KW_WRITE), Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::WriteArg), Symbol::fromTerminal(Terminal::L_RPAREN) } },

    // IfTail
    { RuleId::R_IfTail_Else,   { Symbol::fromTerminal(Terminal::KW_ELSE), Symbol::fromNonTerminal(NonTerminal::Stmt) } },
    { RuleId::R_IfTail_Lambda, { Symbol::fromTerminal(Terminal::LAMBDA) } },

    // Variable & VarExt
    { RuleId::R_Variable,       { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt) } },
    { RuleId::R_VarExt_Bracket, { Symbol::fromTerminal(Terminal::L_LBRACKET), Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromNonTerminal(NonTerminal::IndexTail), Symbol::fromTerminal(Terminal::L_RBRACKET) } },
    { RuleId::R_VarExt_Lambda,  { Symbol::fromTerminal(Terminal::LAMBDA) } },
    { RuleId::R_IndexTail_Comma,{ Symbol::fromTerminal(Terminal::L_COMMA), Symbol::fromNonTerminal(NonTerminal::Expr) } },
    { RuleId::R_IndexTail_Lambda,{ Symbol::fromTerminal(Terminal::LAMBDA) } },

    // Expr
    { RuleId::R_Expr_Paren, { Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromTerminal(Terminal::L_RPAREN), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_Expr_ID,    { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_Expr_INT,   { Symbol::fromTerminal(Terminal::L_INT), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_Expr_FLOAT, { Symbol::fromTerminal(Terminal::L_FLOAT), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },

    // ExprTail & TermTail
    { RuleId::R_ExprTail_Op,     { Symbol::fromTerminal(Terminal::L_ADDITIVE_OPERATOR), Symbol::fromNonTerminal(NonTerminal::Expr) } },
    { RuleId::R_ExprTail_Lambda, { Symbol::fromTerminal(Terminal::LAMBDA) } },
    { RuleId::R_TermTail_Op,     { Symbol::fromTerminal(Terminal::L_MULTIPLICATIVE_OPERATOR), Symbol::fromNonTerminal(NonTerminal::Factor), Symbol::fromNonTerminal(NonTerminal::TermTail) } },
    { RuleId::R_TermTail_Lambda, { Symbol::fromTerminal(Terminal::LAMBDA) } },

    // Factor
    { RuleId::R_Factor_Paren, { Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromTerminal(Terminal::L_RPAREN) } },
    { RuleId::R_Factor_ID,    { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt) } },
    { RuleId::R_Factor_INT,   { Symbol::fromTerminal(Terminal::L_INT) } },
    { RuleId::R_Factor_FLOAT, { Symbol::fromTerminal(Terminal::L_FLOAT) } },

    // Condition
    { RuleId::R_Condition, { Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromTerminal(Terminal::L_COMPARISON_OPERATOR), Symbol::fromNonTerminal(NonTerminal::Expr) } },

    // WriteArg
    { RuleId::R_WriteArg_Paren, { Symbol::fromTerminal(Terminal::L_LPAREN), Symbol::fromNonTerminal(NonTerminal::Expr), Symbol::fromTerminal(Terminal::L_RPAREN), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_WriteArg_ID,    { Symbol::fromTerminal(Terminal::L_ID), Symbol::fromNonTerminal(NonTerminal::VarExt), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_WriteArg_INT,   { Symbol::fromTerminal(Terminal::L_INT), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_WriteArg_FLOAT, { Symbol::fromTerminal(Terminal::L_FLOAT), Symbol::fromNonTerminal(NonTerminal::TermTail), Symbol::fromNonTerminal(NonTerminal::ExprTail) } },
    { RuleId::R_WriteArg_String,{ Symbol::fromTerminal(Terminal::L_STRING) } }
};

// 5. Управляющая таблица LL(1) парсера
// Формат: [Нетерминал][Текущий Терминал] -> Идентификатор применяемого правила
using ParsingTable = std::map<NonTerminal, std::map<Terminal, RuleId>>;

ParsingTable LL1_Table = {
    { NonTerminal::Program, {
        { Terminal::L_ID,     RuleId::R_Prog }, { Terminal::KW_IF,    RuleId::R_Prog },
        { Terminal::KW_WHILE, RuleId::R_Prog }, { Terminal::KW_READ,  RuleId::R_Prog },
        { Terminal::KW_WRITE, RuleId::R_Prog }
    }},
    { NonTerminal::StmtList, {
        { Terminal::L_ID,     RuleId::R_StmtList_ID },    { Terminal::KW_IF,    RuleId::R_StmtList_IF },
        { Terminal::KW_WHILE, RuleId::R_StmtList_WHILE }, { Terminal::KW_READ,  RuleId::R_StmtList_READ },
        { Terminal::KW_WRITE, RuleId::R_StmtList_WRITE }
    }},
    { NonTerminal::StmtTail, {
        { Terminal::L_SEMICOLON, RuleId::R_StmtTail_Semi },
        { Terminal::L_EOF,       RuleId::R_StmtTail_Lambda },
        { Terminal::KW_ELSE,     RuleId::R_StmtTail_Lambda }
    }},
    { NonTerminal::Stmt, {
        { Terminal::L_ID,     RuleId::R_Stmt_ID },    { Terminal::KW_IF,    RuleId::R_Stmt_IF },
        { Terminal::KW_WHILE, RuleId::R_Stmt_WHILE }, { Terminal::KW_READ,  RuleId::R_Stmt_READ },
        { Terminal::KW_WRITE, RuleId::R_Stmt_WRITE }
    }},
    { NonTerminal::IfTail, {
        { Terminal::KW_ELSE,     RuleId::R_IfTail_Else },
        { Terminal::L_SEMICOLON, RuleId::R_IfTail_Lambda },
        { Terminal::L_EOF,       RuleId::R_IfTail_Lambda }
    }},
    { NonTerminal::VarExt, {
        { Terminal::L_LBRACKET,            RuleId::R_VarExt_Bracket },
        { Terminal::L_ASSIGNMENT_OPERATOR, RuleId::R_VarExt_Lambda },
        { Terminal::L_ADDITIVE_OPERATOR,   RuleId::R_VarExt_Lambda },
        { Terminal::L_MULTIPLICATIVE_OPERATOR, RuleId::R_VarExt_Lambda },
        { Terminal::L_COMPARISON_OPERATOR, RuleId::R_VarExt_Lambda },
        { Terminal::KW_THEN,               RuleId::R_VarExt_Lambda },
        { Terminal::KW_DO,                 RuleId::R_VarExt_Lambda },
        { Terminal::L_RPAREN,              RuleId::R_VarExt_Lambda },
        { Terminal::L_RBRACKET,            RuleId::R_VarExt_Lambda },
        { Terminal::L_SEMICOLON,           RuleId::R_VarExt_Lambda },
        { Terminal::L_COMMA,               RuleId::R_VarExt_Lambda },
        { Terminal::L_EOF,                 RuleId::R_VarExt_Lambda }
    }},
    { NonTerminal::IndexTail, {
        { Terminal::L_COMMA,    RuleId::R_IndexTail_Comma },
        { Terminal::L_RBRACKET, RuleId::R_IndexTail_Lambda }
    }},
    { NonTerminal::Expr, {
        { Terminal::L_LPAREN, RuleId::R_Expr_Paren }, { Terminal::L_ID,    RuleId::R_Expr_ID },
        { Terminal::L_INT,    RuleId::R_Expr_INT },   { Terminal::L_FLOAT, RuleId::R_Expr_FLOAT }
    }},
    { NonTerminal::ExprTail, {
        { Terminal::L_ADDITIVE_OPERATOR,   RuleId::R_ExprTail_Op },
        { Terminal::KW_THEN,               RuleId::R_ExprTail_Lambda },
        { Terminal::KW_DO,                 RuleId::R_ExprTail_Lambda },
        { Terminal::L_RPAREN,              RuleId::R_ExprTail_Lambda },
        { Terminal::L_RBRACKET,            RuleId::R_ExprTail_Lambda },
        { Terminal::L_COMPARISON_OPERATOR, RuleId::R_ExprTail_Lambda },
        { Terminal::L_SEMICOLON,           RuleId::R_ExprTail_Lambda },
        { Terminal::L_COMMA,               RuleId::R_ExprTail_Lambda },
        { Terminal::L_EOF,                 RuleId::R_ExprTail_Lambda }
    }},
    { NonTerminal::TermTail, {
        { Terminal::L_MULTIPLICATIVE_OPERATOR, RuleId::R_TermTail_Op },
        { Terminal::L_ADDITIVE_OPERATOR,       RuleId::R_TermTail_Lambda },
        { Terminal::KW_THEN,                   RuleId::R_TermTail_Lambda },
        { Terminal::KW_DO,                     RuleId::R_TermTail_Lambda },
        { Terminal::L_RPAREN,                  RuleId::R_TermTail_Lambda },
        { Terminal::L_RBRACKET,                RuleId::R_TermTail_Lambda },
        { Terminal::L_COMPARISON_OPERATOR,     RuleId::R_TermTail_Lambda },
        { Terminal::L_SEMICOLON,               RuleId::R_TermTail_Lambda },
        { Terminal::L_COMMA,                   RuleId::R_TermTail_Lambda },
        { Terminal::L_EOF,                     RuleId::R_TermTail_Lambda }
    }},
    { NonTerminal::Factor, {
        { Terminal::L_LPAREN, RuleId::R_Factor_Paren }, { Terminal::L_ID,    RuleId::R_Factor_ID },
        { Terminal::L_INT,    RuleId::R_Factor_INT },   { Terminal::L_FLOAT, RuleId::R_Factor_FLOAT }
    }},
    { NonTerminal::Condition, {
        { Terminal::L_LPAREN, RuleId::R_Condition }, { Terminal::L_ID,    RuleId::R_Condition },
        { Terminal::L_INT,    RuleId::R_Condition }, { Terminal::L_FLOAT, RuleId::R_Condition }
    }},
    { NonTerminal::WriteArg, {
        { Terminal::L_LPAREN, RuleId::R_WriteArg_Paren }, { Terminal::L_ID,    RuleId::R_WriteArg_ID },
        { Terminal::L_INT,    RuleId::R_WriteArg_INT },   { Terminal::L_FLOAT, RuleId::R_WriteArg_FLOAT },
        { Terminal::L_STRING, RuleId::R_WriteArg_String }
    }}
};

// 6. Класс табличного LL(1)-анализатора
class LL1Parser {
private:
    std::stack<Symbol> parseStack;
    std::vector<Token> tokens;
    size_t currentTokenIndex;

    Token getNextToken() {
        if (currentTokenIndex < tokens.size()) {
            return tokens[currentTokenIndex];
        }
        return { Terminal::L_EOF, "⊥", -1, -1 };
    }

public:
    LL1Parser(const std::vector<Token>& srcTokens) 
        : tokens(srcTokens), currentTokenIndex(0) {}

    bool parse() {
        // Инициализация стека: [ ⊥, Program ]
        parseStack.push(Symbol::fromTerminal(Terminal::L_EOF));
        parseStack.push(Symbol::fromNonTerminal(NonTerminal::Program));

        while (!parseStack.empty()) {
            Symbol top = parseStack.top();
            Token currentToken = getNextToken();

            if (top.isTerminal) {
                if (top.term == Terminal::LAMBDA) {
                    // Пустая цепочка: просто выталкиваем λ из стека
                    parseStack.pop();
                } 
                else if (top.term == currentToken.type) {
                    // Терминал совпал с входной лексемой
                    parseStack.pop();
                    currentTokenIndex++;
                } 
                else {
                    std::cerr << "Синтаксическая ошибка в строке " << currentToken.line 
                              << ", позиция " << currentToken.column 
                              << ": Ожидался терминал, но найдена лексема " << currentToken.value << "\n";
                    return false;
                }
            } 
            else {
                // Если на вершине стека нетерминал, ищем в таблице
                auto ntIt = LL1_Table.find(top.nonTerm);
                if (ntIt != LL1_Table.end()) {
                    auto tIt = ntIt->second.find(currentToken.type);
                    if (tIt != ntIt->second.end()) {
                        RuleId rId = tIt->second;
                        parseStack.pop(); // Снимаем текущий нетерминал

                        // Записываем правую часть правила в обратном порядке в магазин
                        const Production& prod = GrammarRules[rId];
                        for (auto it = prod.rbegin(); it != prod.rend(); ++it) {
                            parseStack.push(*it);
                        }
                    } else {
                        std::cerr << "Синтаксическая ошибка в строке " << currentToken.line 
                                  << ", позиция " << currentToken.column 
                                  << ": Неверный токен '" << currentToken.value << "' для раскрытия нетерминала.\n";
                        return false;
                    }
                } else {
                    std::cerr << "Критическая ошибка в таблице переходов парсера.\n";
                    return false;
                }
            }
        }

        std::cout << "Синтаксический анализ успешно завершен!\n";
        return true;
    }
};

// Пример точки входа
int main() {
    FixLocale();
    // Входной поток токенов (например: a := 5; )
    std::vector<Token> tokens = {
        { Terminal::L_ID, "a", 1, 1 },
        { Terminal::L_ASSIGNMENT_OPERATOR, ":=", 1, 3 },
        { Terminal::L_INT, "5", 1, 6 },
        { Terminal::L_EOF, "⊥", 1, 7 }
    };

    LL1Parser parser(tokens);
    if (parser.parse()) {
        std::cout << "Программа синтаксически корректна.\n";
    } else {
        std::cout << "Ошибка разбора.\n";
    }

    return 0;
}