#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    initSemanticTable(); // Инициализируем таблицу при создании
}

void Parser::initSemanticTable() {
    // Простые операнды и операции[cite: 15]
    semanticActions["a"]  = [this]() { rpn.push_back({RpnElementType::ADDR_VAR, currentLexem.value}); };
    semanticActions["k"]  = [this]() { rpn.push_back({RpnElementType::CONST_VAL, currentLexem.value}); };
    semanticActions["+"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "+"}); };
    semanticActions["-"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "-"}); };
    semanticActions["*"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "*"}); };
    semanticActions["/"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "/"}); };
    semanticActions[":="] = [this]() { rpn.push_back({RpnElementType::OPERATOR, ":="}); };
    semanticActions["w"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "WRITE"}); };
    semanticActions["r"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "READ"}); };
    semanticActions["i"]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, "INDEX1"}); };
    semanticActions["i2"] = [this]() { rpn.push_back({RpnElementType::OPERATOR, "INDEX2"}); };
    semanticActions["-'"] = [this]() { rpn.push_back({RpnElementType::OPERATOR, "NEG"}); };
    semanticActions["="]  = [this]() { rpn.push_back({RpnElementType::OPERATOR, currentLexem.value}); };

    // Семантические программы P1-P5 для управления метками[cite: 13, 15]
    semanticActions["1"] = [this]() { 
        labelStack.push(rpn.size());
        rpn.push_back({RpnElementType::LABEL, ""}); // Резерв под адрес
        rpn.push_back({RpnElementType::OPERATOR, "JF"}); 
    };

    semanticActions["2"] = [this]() {
        int prevLabel = labelStack.top(); labelStack.pop();
        rpn[prevLabel].value = std::to_string(rpn.size() + 2); // Прошивка JF[cite: 13]
        labelStack.push(rpn.size());
        rpn.push_back({RpnElementType::LABEL, ""}); // Резерв под J
        rpn.push_back({RpnElementType::OPERATOR, "J"});
    };

    semanticActions["3"] = [this]() {
        int label = labelStack.top(); labelStack.pop();
        rpn[label].value = std::to_string(rpn.size()); // Прошивка адреса конца[cite: 13]
    };

    semanticActions["4"] = [this]() {
        labelStack.push(rpn.size()); // Запоминаем начало цикла[cite: 15]
    };

    semanticActions["5"] = [this]() {
        int exitLabel = labelStack.top(); labelStack.pop();
        int startLoop = labelStack.top(); labelStack.pop();
        rpn[exitLabel].value = std::to_string(rpn.size() + 2); // Прошивка выхода из цикла[cite: 13]
        rpn.push_back({RpnElementType::LABEL, std::to_string(startLoop)});
        rpn.push_back({RpnElementType::OPERATOR, "J"});
    };
}

std::vector<RpnElement> Parser::parse() {
    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Program});
    currentLexem = lexer.getNextLexem();

    while (!parseStack.empty()) {
        StackSymbol top = parseStack.top();
        parseStack.pop();

        if (top.type == SymbolType::TERMINAL) {
            if ((int)currentLexem.type == top.id) {
                currentLexem = lexer.getNextLexem(); 
            } else {
                throw std::runtime_error("Syntax Error: Unexpected token " + currentLexem.value);
            }
        } 
        else if (top.type == SymbolType::SEMANTIC_ACTION) {
            executeAction(top.action); // Вызов через таблицу[cite: 13]
        } 
        else if (top.type == SymbolType::NON_TERMINAL) {
            pushProduction((NonTerm)top.id, currentLexem.type); 
        }
    }
    return rpn;
}

void Parser::executeAction(const std::string& action) {
    // Вместо if-else используем поиск в таблице действий[cite: 15]
    if (semanticActions.count(action)) {
        semanticActions[action]();
    }
}

/**
 * Реализация метода выбора порождающих правил (Управляющая таблица)
 * nt - нетерминал на вершине стека
 * lt - тип текущей лексемы (lookahead)
 */
void Parser::pushProduction(NonTerm nt, LexemType lt) {
    switch (nt) {
        case NonTerm::Program:
            if (lt == LexemType::L_TERMINATOR || lt == LexemType::L_ERROR) {
                break; // Конец разбора
            }
            // <Program> -> <Statement> <Program>
            parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Program});
            parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Statement});
            break;

        case NonTerm::Statement:
            if (lt == LexemType::L_ID) {
                // <Assignment> -> L_ID <ArrayIndex> := <Expression> {:=} ';'
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // ';'
                parseStack.push({SymbolType::SEMANTIC_ACTION, ":="});
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::SemanticTrigger}); // λ
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Expression});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_ASSIGNMENT_OPERATOR});
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex});
                parseStack.push({SymbolType::SEMANTIC_ACTION, "a"});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_ID});
            } 
            else if (lt == LexemType::L_KEYWORD) {
                if (currentLexem.value == "if") {
                    // <IfStatement> -> if <Cond> then <St> <Else> {3}
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "3"});
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ElsePart});
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Statement});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // then
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "1"});
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Condition});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // if
                }
                else if (currentLexem.value == "while") {
                    // <WhileStatement> -> while {4} <Cond> do <St> {5}[cite: 10]
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "5"});
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Statement});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // do
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "1"});
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Condition});
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "4"});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // while
                }
                else if (currentLexem.value == "read") {
                    // read ( id <ArrayIndex> ) ; {r}[cite: 10]
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // ';'
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "r"});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // ')'
                    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex});
                    parseStack.push({SymbolType::SEMANTIC_ACTION, "a"});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_ID});
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // '('
                    parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // read
                }
            }
            break;

        case NonTerm::ElsePart:
            if (lt == LexemType::L_KEYWORD && currentLexem.value == "else") {
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Statement});
                parseStack.push({SymbolType::SEMANTIC_ACTION, "2"});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_KEYWORD}); // else
            }
            // λ (пустая ветка)[cite: 9]
            break;

        case NonTerm::Expression:
            // S -> T S'[cite: 9]
            parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail});
            parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Term});
            break;

        case NonTerm::ExpressionTail:
            if (lt == LexemType::L_ADDITIVE_OPERATOR) {
                // S' -> + T {+} S'[cite: 10]
                std::string op = currentLexem.value;
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail});
                parseStack.push({SymbolType::SEMANTIC_ACTION, op});
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Term});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_ADDITIVE_OPERATOR});
            }
            break;

        case NonTerm::ArrayIndex:
            if (lt == LexemType::L_DELIMITER && currentLexem.value == "[") {
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::ArrayTail});
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Expression});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // '['
            }
            break;

        case NonTerm::ArrayTail:
            if (currentLexem.value == "]") {
                parseStack.push({SymbolType::SEMANTIC_ACTION, "i"}); // INDEX1[cite: 10]
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER});
            } else if (currentLexem.value == ",") {
                parseStack.push({SymbolType::SEMANTIC_ACTION, "i2"}); // INDEX2[cite: 10]
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // ']'
                parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Expression});
                parseStack.push({SymbolType::TERMINAL, (int)LexemType::L_DELIMITER}); // ','
            }
            break;

        case NonTerm::SemanticTrigger:
            // λ (просто удаляется из стека, вызывая действие если нужно)[cite: 9]
            break;

        default:
            throw std::runtime_error("No rule for NonTerm ID: " + std::to_string((int)nt));
    }
}