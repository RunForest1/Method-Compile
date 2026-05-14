#include "Parser.h"
#include <stdexcept>

Parser::Parser(Lexer &lexer) : lexer(lexer)
{
    initSemanticTable(); // Инициализация таблицы действий
    initMatrix();        // Инициализация матрицы переходов
}

/**
 * Вспомогательный метод для разделения общих типов лексем (Keyword, Delimiter)
 * на уникальные терминалы для матрицы переходов.
 */
int Parser::getExtendedType(const Lexem &l)
{
    if (l.type == LexemType::L_KEYWORD)
    {
        if (l.value == "if")
            return 1001;
        if (l.value == "then")
            return 1002;
        if (l.value == "else")
            return 1003;
        if (l.value == "while")
            return 1004;
        if (l.value == "do")
            return 1005;
        if (l.value == "read")
            return 1006;
        if (l.value == "write")
            return 1007;
        if (l.value == "sin")
            return 1008;
        if (l.value == "cos")
            return 1009;
        if (l.value == "exp")
            return 1010;
        if (l.value == "power")
            return 1011;
        if (l.value == "log")
            return 1012;
        if (l.value == "tan")
            return 1013;
        if (l.value == "ctan")
            return 1014;
    }
    if (l.type == LexemType::L_DELIMITER)
    {
        if (l.value == "(")
            return 2001;
        if (l.value == ")")
            return 2002;
        if (l.value == "[")
            return 2003;
        if (l.value == "]")
            return 2004;
        if (l.value == ";")
            return 2005;
        if (l.value == ",")
            return 2006;
        if (l.value == "{")
            return 2007;
        if (l.value == "}")
            return 2008;
    }
    return (int)l.type;
}

void Parser::initMatrix()
{
    // --- 1. Сокращения для терминалов ---
    auto ID = (LexemType)LexemType::L_ID;
    auto INT = (LexemType)LexemType::L_INT;
    auto FLOAT = (LexemType)LexemType::L_FLOAT;
    auto STRING = (LexemType)LexemType::L_STRING;
    auto ASSIGN = (LexemType)LexemType::L_ASSIGNMENT_OPERATOR;
    auto ADD_OP = (LexemType)LexemType::L_ADDITIVE_OPERATOR;
    auto MULT_OP = (LexemType)LexemType::L_MULTIPLICATIVE_OPERATOR;
    auto COMP_OP = (LexemType)LexemType::L_COMPARISON_OPERATOR;
    auto TERM = (LexemType)LexemType::L_TERMINATOR;

    auto IF = (LexemType)1001;
    auto THEN = (LexemType)1002;
    auto ELSE = (LexemType)1003;
    auto WHILE = (LexemType)1004;
    auto DO = (LexemType)1005;
    auto READ = (LexemType)1006;
    auto WRITE = (LexemType)1007;
    auto SIN = (LexemType)1008;
    auto COS = (LexemType)1009;
    auto EXP = (LexemType)1010;
    auto POW = (LexemType)1011;
    auto LOG = (LexemType)1012;
    auto TAN = (LexemType)1013;
    auto CTAN = (LexemType)1014;

    auto LPAR = (LexemType)2001;
    auto RPAR = (LexemType)2002;
    auto LBRACK = (LexemType)2003;
    auto RBRACK = (LexemType)2004;
    auto SEMI = (LexemType)2005;
    auto COMMA = (LexemType)2006;
    auto LBRACE = (LexemType)2007;
    auto RBRACE = (LexemType)2008;

    // Список токенов, с которых может начинаться Factor (для инициализации Expression/Term)
    std::vector<LexemType> firstFactor = {
        ID, INT, FLOAT, STRING, LPAR, ADD_OP, SIN, COS, EXP, POW, LOG, TAN, CTAN};

    // --- 2. Program & Statement ---
    for (auto t : {ID, IF, WHILE, READ, WRITE, LBRACE})
    {
        M[NonTerm::Program][t] = {{SymbolType::NON_TERMINAL, (int)NonTerm::Statement}, {SymbolType::NON_TERMINAL, (int)NonTerm::Program}};
    }
    for (auto t : {TERM, ELSE, RBRACE, SEMI})
        M[NonTerm::Program][t] = {};

    M[NonTerm::Statement][WRITE] = {{SymbolType::TERMINAL, (int)WRITE}, {SymbolType::TERMINAL, (int)LPAR}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::SEMANTIC_ACTION, "w"}, {SymbolType::TERMINAL, (int)RPAR}, {SymbolType::TERMINAL, (int)SEMI}};
    M[NonTerm::Statement][READ] = {{SymbolType::TERMINAL, (int)READ}, {SymbolType::TERMINAL, (int)LPAR}, {SymbolType::TERMINAL, (int)ID}, {SymbolType::SEMANTIC_ACTION, "a"}, {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex}, {SymbolType::TERMINAL, (int)RPAR}, {SymbolType::SEMANTIC_ACTION, "r"}, {SymbolType::TERMINAL, (int)SEMI}};
    M[NonTerm::Statement][IF] = {{SymbolType::TERMINAL, (int)IF}, {SymbolType::NON_TERMINAL, (int)NonTerm::Condition}, {SymbolType::SEMANTIC_ACTION, "1"}, {SymbolType::TERMINAL, (int)THEN}, {SymbolType::NON_TERMINAL, (int)NonTerm::Statement}, {SymbolType::NON_TERMINAL, (int)NonTerm::ElsePart}, {SymbolType::SEMANTIC_ACTION, "3"}};
    M[NonTerm::Statement][WHILE] = {{SymbolType::SEMANTIC_ACTION, "4"}, {SymbolType::TERMINAL, (int)WHILE}, {SymbolType::NON_TERMINAL, (int)NonTerm::Condition}, {SymbolType::SEMANTIC_ACTION, "1"}, {SymbolType::TERMINAL, (int)DO}, {SymbolType::NON_TERMINAL, (int)NonTerm::Statement}, {SymbolType::SEMANTIC_ACTION, "5"}};
    M[NonTerm::Statement][ID] = {{SymbolType::TERMINAL, (int)ID}, {SymbolType::SEMANTIC_ACTION, "a"}, {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex}, {SymbolType::TERMINAL, (int)ASSIGN}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::SEMANTIC_ACTION, ":="}, {SymbolType::TERMINAL, (int)SEMI}};
    M[NonTerm::Statement][LBRACE] = {{SymbolType::TERMINAL, (int)LBRACE}, {SymbolType::NON_TERMINAL, (int)NonTerm::Program}, {SymbolType::TERMINAL, (int)RBRACE}};

    M[NonTerm::ElsePart][ELSE] = {{SymbolType::TERMINAL, (int)ELSE}, {SymbolType::SEMANTIC_ACTION, "2"}, {SymbolType::NON_TERMINAL, (int)NonTerm::Statement}};
    for (auto t : {ID, IF, WHILE, READ, WRITE, TERM, RBRACE, SEMI})
        M[NonTerm::ElsePart][t] = {};

    // --- 3. Expression Structure ---
    M[NonTerm::Condition][ID] = M[NonTerm::Condition][INT] = M[NonTerm::Condition][FLOAT] = M[NonTerm::Condition][LPAR] = M[NonTerm::Condition][ADD_OP] =
        {{SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)COMP_OP}, {SymbolType::SEMANTIC_ACTION, "save_comp_op"}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::SEMANTIC_ACTION, "apply_comp_op"}};

    for (auto t : firstFactor)
    {
        M[NonTerm::Expression][t] = {{SymbolType::NON_TERMINAL, (int)NonTerm::Term}, {SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail}};
        M[NonTerm::Term][t] = {{SymbolType::NON_TERMINAL, (int)NonTerm::Factor}, {SymbolType::NON_TERMINAL, (int)NonTerm::TermTail}};
    }

    M[NonTerm::ExpressionTail][ADD_OP] = {{SymbolType::TERMINAL, (int)ADD_OP}, {SymbolType::SEMANTIC_ACTION, "save_add_op"}, {SymbolType::NON_TERMINAL, (int)NonTerm::Term}, {SymbolType::SEMANTIC_ACTION, "apply_add_op"}, {SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail}};
    for (auto t : {SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::ExpressionTail][t] = {};

    M[NonTerm::TermTail][MULT_OP] = {{SymbolType::TERMINAL, (int)MULT_OP}, {SymbolType::SEMANTIC_ACTION, "save_mult_op"}, {SymbolType::NON_TERMINAL, (int)NonTerm::Factor}, {SymbolType::SEMANTIC_ACTION, "apply_mult_op"}, {SymbolType::NON_TERMINAL, (int)NonTerm::TermTail}};
    for (auto t : {ADD_OP, SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::TermTail][t] = {};

    // --- 4. Factor & Unary Logic ---
    M[NonTerm::Factor][LPAR] = {{SymbolType::TERMINAL, (int)LPAR}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)RPAR}};
    M[NonTerm::Factor][ID] = {{SymbolType::TERMINAL, (int)ID}, {SymbolType::SEMANTIC_ACTION, "a"}, {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex}};
    M[NonTerm::Factor][INT] = {{SymbolType::TERMINAL, (int)INT}, {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::Factor][FLOAT] = {{SymbolType::TERMINAL, (int)FLOAT}, {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::Factor][STRING] = {{SymbolType::TERMINAL, (int)STRING}, {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::Factor][ADD_OP] = {
        {SymbolType::TERMINAL, (int)ADD_OP},
        {SymbolType::SEMANTIC_ACTION, "unary_op"}, // Сохраняем знак сразу
        {SymbolType::NON_TERMINAL, (int)NonTerm::UnaryOperand},
        {SymbolType::SEMANTIC_ACTION, "apply_unary"} // Применяем NEG после операнда
    };
    // Функции в Factor
    for (auto f : {SIN, COS, EXP, POW, LOG, TAN, CTAN})
    {
        M[NonTerm::Factor][f] = {{SymbolType::NON_TERMINAL, (int)NonTerm::MathFunction}};
    }

    // UnaryOperand (что может идти после унарного минуса)
    M[NonTerm::UnaryOperand][LPAR] = {{SymbolType::TERMINAL, (int)LPAR}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)RPAR}};
    M[NonTerm::UnaryOperand][ID] = {{SymbolType::TERMINAL, (int)ID}, {SymbolType::SEMANTIC_ACTION, "a"}, {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex}};
    M[NonTerm::UnaryOperand][INT] = {{SymbolType::TERMINAL, (int)INT}, {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::UnaryOperand][FLOAT] = {{SymbolType::TERMINAL, (int)FLOAT}, {SymbolType::SEMANTIC_ACTION, "k"}};
    for (auto f : {SIN, COS, EXP, POW, LOG, TAN, CTAN})
    {
        M[NonTerm::UnaryOperand][f] = {{SymbolType::NON_TERMINAL, (int)NonTerm::MathFunction}};
    }

    // --- 5. Arrays ---
    M[NonTerm::ArrayIndex][LBRACK] = {{SymbolType::TERMINAL, (int)LBRACK}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayTail}};
    for (auto t : {ASSIGN, SEMI, ADD_OP, MULT_OP, COMP_OP, RPAR, COMMA, THEN, DO, ELSE, RBRACK})
        M[NonTerm::ArrayIndex][t] = {};

    M[NonTerm::ArrayTail][RBRACK] = {{SymbolType::SEMANTIC_ACTION, "INDEX1"}, {SymbolType::TERMINAL, (int)RBRACK}};
    M[NonTerm::ArrayTail][COMMA] = {{SymbolType::TERMINAL, (int)COMMA}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::SEMANTIC_ACTION, "INDEX2"}, {SymbolType::TERMINAL, (int)RBRACK}};

    // --- 6. Math Functions ---
    // Для функций с 1 аргументом
    for (auto f : {std::make_pair(SIN, "SIN"), std::make_pair(COS, "COS"), std::make_pair(EXP, "EXP"), std::make_pair(LOG, "LOG"), std::make_pair(TAN, "TAN"), std::make_pair(CTAN, "CTAN")})
    {
        M[NonTerm::MathFunction][f.first] = {{SymbolType::TERMINAL, (int)f.first}, {SymbolType::TERMINAL, (int)LPAR}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)RPAR}, {SymbolType::SEMANTIC_ACTION, f.second}};
    }
    // Для power (2 аргумента)
    M[NonTerm::MathFunction][POW] = {{SymbolType::TERMINAL, (int)POW}, {SymbolType::TERMINAL, (int)LPAR}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)COMMA}, {SymbolType::NON_TERMINAL, (int)NonTerm::Expression}, {SymbolType::TERMINAL, (int)RPAR}, {SymbolType::SEMANTIC_ACTION, "POW"}};
}

void Parser::initSemanticTable()
{
    // Сохранение сравнения
    semanticActions["save_comp_op"] = [this]()
    {
        operatorStack.push(prevLexem.value);
    };

    semanticActions["apply_comp_op"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };
    // Сохранение аддитивного оператора (+ или -) в стек
    semanticActions["save_add_op"] = [this]()
    { operatorStack.push(prevLexem.value); };

    // Применение сохранённого аддитивного оператора из стека
    semanticActions["apply_add_op"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };

    // Сохранение мультипликативного оператора (* или /) в стек
    semanticActions["save_mult_op"] = [this]()
    { operatorStack.push(prevLexem.value); };

    // Применение сохранённого мультипликативного оператора из стека
    semanticActions["apply_mult_op"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };

    // Бинарные операции - используют сохраненный оператор (оставлены для обратной совместимости)
    semanticActions["+"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };
    semanticActions["-"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };
    semanticActions["*"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };
    semanticActions["/"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };

    // Универсальные действия для операторов, зависящих от контекста
    semanticActions["add_op"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };
    semanticActions["mult_op"] = [this]()
    {
        if (!operatorStack.empty())
        {
            rpn.push_back({RpnElementType::OPERATOR, operatorStack.top()});
            operatorStack.pop();
        }
    };

    semanticActions["unary_op"] = [this]()
    {
        // Мы вызываем это действие СРАЗУ после считывания знака
        operatorStack.push(prevLexem.value);
    };

    semanticActions["apply_unary"] = [this]()
    {
        // Мы вызываем это после операнда
        if (!operatorStack.empty())
        {
            if (operatorStack.top() == "-")
            {
                rpn.push_back({RpnElementType::OPERATOR, "NEG"});
            }
            operatorStack.pop();
        }
    };

    // Operand actions
    semanticActions["a"] = [this]()
    { rpn.push_back({RpnElementType::ADDR_VAR, prevLexem.value}); };
    semanticActions["k"] = [this]()
    { rpn.push_back({RpnElementType::CONST_VAL, prevLexem.value}); };

    // Assignment
    semanticActions[":="] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, ":="}); };

    // IO
    semanticActions["r"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "READ"}); };
    semanticActions["w"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "WRITE"}); };

    // Arrays
    semanticActions["i"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "INDEX1"}); };
    semanticActions["i2"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "INDEX2"}); };

    // Comparison
    semanticActions["="] = [this]()
    {
        rpn.push_back({RpnElementType::OPERATOR, prevLexem.value}); // ==, !=, <, >, etc.
    };

    semanticActions["SIN"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "SIN"}); };
    semanticActions["COS"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "COS"}); };
    semanticActions["EXP"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "EXP"}); };
    semanticActions["LOG"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "LOG"}); };
    semanticActions["POW"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "POW"}); };
    semanticActions["TAN"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "TAN"}); };
    semanticActions["CTAN"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, "CTAN"}); };

    semanticActions["INDEX1"] = [this]()
    {
        rpn.push_back({RpnElementType::OPERATOR, "INDEX1"});
    };
    semanticActions["INDEX2"] = [this]()
    {
        rpn.push_back({RpnElementType::OPERATOR, "INDEX2"});
    };

    // Label Programs P1-P5
    semanticActions["1"] = [this]()
    {
        labelStack.push(rpn.size());
        rpn.push_back({RpnElementType::LABEL, ""}); // Placeholder
        rpn.push_back({RpnElementType::OPERATOR, "JF"});
    };

    semanticActions["2"] = [this]()
    {
        if (labelStack.empty())
        {
            throw std::runtime_error("Семантическая ошибка: стек меток пуст при выполнении действия '2' (Else)");
        }
        int prevLabel = labelStack.top();
        labelStack.pop();

        labelStack.push(rpn.size()); // Сохраняем позицию для нового J
        rpn.push_back({RpnElementType::LABEL, ""});
        rpn.push_back({RpnElementType::OPERATOR, "J"});

        // Прошиваем предыдущую метку (адрес следующего элемента после J)
        rpn[prevLabel].value = std::to_string(rpn.size());
    };

    semanticActions["3"] = [this]()
    {
        if (labelStack.empty())
        {
            throw std::runtime_error("Семантическая ошибка: стек меток пуст при выполнении действия '3' (End If)");
        }
        int label = labelStack.top();
        labelStack.pop();
        // Прошиваем метку (адрес текущего элемента)
        rpn[label].value = std::to_string(rpn.size());
    };

    semanticActions["4"] = [this]()
    {
        labelStack.push(rpn.size());
    };

    semanticActions["5"] = [this]()
    {
        if (labelStack.size() < 2)
        {
            throw std::runtime_error("Семантическая ошибка: недостаточно меток в стеке для действия '5' (End While)");
        }
        int exitLabel = labelStack.top(); // Метка для выхода из цикла (из P1)
        labelStack.pop();
        int startLoop = labelStack.top(); // Метка начала цикла (из P4)
        labelStack.pop();

        // Генерируем переход на начало
        rpn.push_back({RpnElementType::LABEL, std::to_string(startLoop)});
        rpn.push_back({RpnElementType::OPERATOR, "J"});

        // Прошиваем выход: JF должен вести на элемент после J
        rpn[exitLabel].value = std::to_string(rpn.size());
    };
}

std::vector<RpnElement> Parser::parse()
{
    parseStack.push({SymbolType::NON_TERMINAL, (int)NonTerm::Program});
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
            pushProduction((NonTerm)top.id, currentLexem.type);
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

    // Проверка наличия правила
    if (M.count(nt) && M[nt].count((LexemType)extType))
    {
        const Production &prod = M[nt][(LexemType)extType];
        for (auto it = prod.rbegin(); it != prod.rend(); ++it)
        {
            parseStack.push(*it);
        }
    }
    else
    {
        // Обработка ошибок или lambda, если правило пустое
        if (M.count(nt) && M[nt].count((LexemType)extType) && M[nt][(LexemType)extType].empty())
        {
            // Lambda transition, do nothing
            return;
        }

        if (!(nt == NonTerm::Program && (lt == LexemType::L_TERMINATOR || lt == LexemType::L_ERROR)))
        {
            throw std::runtime_error("Ошибка матрицы разбора: нет правила для нетерминала " + std::to_string(static_cast<int>(nt)) +
                                     " с текущей лексемой '" + currentLexem.value + "' (тип " + std::to_string(extType) + ")");
        }
    }
}