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
    }
    return (int)l.type;
}

void Parser::initMatrix()
{
    // Сокращения для терминалов
    auto ID = (LexemType)LexemType::L_ID;
    auto INT = (LexemType)LexemType::L_INT;
    auto FLOAT = (LexemType)LexemType::L_FLOAT;
    auto STRING = (LexemType)LexemType::L_STRING;
    auto ASSIGN = (LexemType)LexemType::L_ASSIGNMENT_OPERATOR;
    auto ADD_OP = (LexemType)LexemType::L_ADDITIVE_OPERATOR;
    auto MULT_OP = (LexemType)LexemType::L_MULTIPLICATIVE_OPERATOR;
    auto COMP_OP = (LexemType)LexemType::L_COMPARISON_OPERATOR;
    auto TERM = (LexemType)LexemType::L_TERMINATOR;

    // Виртуальные типы (согласовано с getExtendedType)
    auto IF = (LexemType)1001;
    auto THEN = (LexemType)1002;
    auto ELSE = (LexemType)1003;
    auto WHILE = (LexemType)1004;
    auto DO = (LexemType)1005;
    auto READ = (LexemType)1006;
    auto WRITE = (LexemType)1007;
    auto LPAR = (LexemType)2001;
    auto RPAR = (LexemType)2002;
    auto LBRACK = (LexemType)2003;
    auto RBRACK = (LexemType)2004;
    auto SEMI = (LexemType)2005;
    auto COMMA = (LexemType)2006;

    // --- 1. Program ---
    // Program -> Statement Program | lambda
    // Если видим начало оператора, разворачиваем Statement, затем рекурсивно Program
    for (auto t : {ID, IF, WHILE, READ, WRITE})
    {
        M[NonTerm::Program][t] = {
            {SymbolType::NON_TERMINAL, (int)NonTerm::Statement},
            {SymbolType::NON_TERMINAL, (int)NonTerm::Program}};
    }
    // Lambda переходы для Program (конец ввода или конец блока else)
    M[NonTerm::Program][TERM] = {};
    M[NonTerm::Program][ELSE] = {};

    M[NonTerm::Program][SEMI] = {};

    // --- 2. Statement ---
    M[NonTerm::Statement][WRITE] = {
        {SymbolType::TERMINAL, (int)WRITE},
        {SymbolType::TERMINAL, (int)LPAR},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::SEMANTIC_ACTION, "w"}, // Выполнится после Expression
        {SymbolType::TERMINAL, (int)RPAR},
        {SymbolType::TERMINAL, (int)SEMI}};

    M[NonTerm::Statement][READ] = {
        {SymbolType::TERMINAL, (int)READ},
        {SymbolType::TERMINAL, (int)LPAR},
        {SymbolType::TERMINAL, (int)ID},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex},
        {SymbolType::SEMANTIC_ACTION, "a"}, // Адрес переменной
        {SymbolType::TERMINAL, (int)RPAR},
        {SymbolType::SEMANTIC_ACTION, "r"}, // Операция чтения
        {SymbolType::TERMINAL, (int)SEMI}};

    M[NonTerm::Statement][IF] = {
        {SymbolType::TERMINAL, (int)IF},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Condition},
        {SymbolType::SEMANTIC_ACTION, "1"}, // P1: Генерация JF
        {SymbolType::TERMINAL, (int)THEN},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Statement},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ElsePart},
        {SymbolType::SEMANTIC_ACTION, "3"} // P3: Прошивка адреса для JF (и J из else если был)
    };

    M[NonTerm::Statement][WHILE] = {
        {SymbolType::SEMANTIC_ACTION, "4"}, // P4: Запомнить начало цикла
        {SymbolType::TERMINAL, (int)WHILE},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Condition},
        {SymbolType::SEMANTIC_ACTION, "1"}, // P1: Генерация JF (выход из цикла)
        {SymbolType::TERMINAL, (int)DO},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Statement},
        {SymbolType::SEMANTIC_ACTION, "5"} // P5: Прошивка перехода назад на начало
    };

    M[NonTerm::Statement][ID] = {
        {SymbolType::TERMINAL, (int)ID},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex},
        {SymbolType::SEMANTIC_ACTION, "a"}, // Адрес левой части
        {SymbolType::TERMINAL, (int)ASSIGN},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::SEMANTIC_ACTION, ":="}, // Оператор присваивания
        {SymbolType::TERMINAL, (int)SEMI}};

    // --- 3. ElsePart ---
    M[NonTerm::ElsePart][ELSE] = {
        {SymbolType::TERMINAL, (int)ELSE},
        {SymbolType::SEMANTIC_ACTION, "2"}, // P2: Генерация J (переход через else)
        {SymbolType::NON_TERMINAL, (int)NonTerm::Statement}};
    // Lambda
    for (auto t : {SEMI, ELSE, TERM, RPAR}) // Добавил RPAR на всякий случай, если внутри скобок
    {
        M[NonTerm::ElsePart][t] = {};
    }

    // --- 4. Condition ---
    M[NonTerm::Condition][ID] =
        M[NonTerm::Condition][INT] =
            M[NonTerm::Condition][LPAR] = {
                {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
                {SymbolType::TERMINAL, (int)COMP_OP},
                {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
                {SymbolType::SEMANTIC_ACTION, "="} // Запись оператора сравнения
            };

    // --- 5. Expression & Term (Eliminated Left Recursion) ---
    for (auto t : {ID, INT, FLOAT, STRING, LPAR, ADD_OP}) // ADD_OP для унарного минуса/плюса в Factor
    {
        M[NonTerm::Expression][t] = {
            {SymbolType::NON_TERMINAL, (int)NonTerm::Term},
            {SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail}};
    }

    // ExpressionTail -> + Term ExpressionTail | - Term ExpressionTail | lambda
    M[NonTerm::ExpressionTail][ADD_OP] = {
        {SymbolType::TERMINAL, (int)ADD_OP},
        {SymbolType::SEMANTIC_ACTION, "save_op"}, // Сохраняем оператор перед обработкой Term
        {SymbolType::NON_TERMINAL, (int)NonTerm::Term},
        {SymbolType::SEMANTIC_ACTION, "+"}, // Используем сохраненный оператор
        {SymbolType::NON_TERMINAL, (int)NonTerm::ExpressionTail}};

    // Lambda for ExpressionTail
    for (auto t : {SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::ExpressionTail][t] = {};

    // Term -> Factor TermTail
    for (auto t : {ID, INT, FLOAT, STRING, LPAR, ADD_OP})
    {
        M[NonTerm::Term][t] = {
            {SymbolType::NON_TERMINAL, (int)NonTerm::Factor},
            {SymbolType::NON_TERMINAL, (int)NonTerm::TermTail}};
    }

    // TermTail -> * Factor TermTail | / Factor TermTail | lambda
    M[NonTerm::TermTail][MULT_OP] = {
        {SymbolType::TERMINAL, (int)MULT_OP},
        {SymbolType::SEMANTIC_ACTION, "save_op"}, // Сохраняем оператор перед обработкой Factor
        {SymbolType::NON_TERMINAL, (int)NonTerm::Factor},
        {SymbolType::SEMANTIC_ACTION, "mult_op"}, // Используем сохраненный оператор
        {SymbolType::NON_TERMINAL, (int)NonTerm::TermTail}};
    // Lambda for TermTail
    for (auto t : {ADD_OP, SEMI, THEN, DO, RPAR, RBRACK, COMMA, COMP_OP, ELSE})
        M[NonTerm::TermTail][t] = {};

    // --- 6. Factor ---
    M[NonTerm::Factor][LPAR] = {
        {SymbolType::TERMINAL, (int)LPAR},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::TERMINAL, (int)RPAR}};

    // Factor -> + UnaryOperand | - UnaryOperand
    M[NonTerm::Factor][ADD_OP] = {
        {SymbolType::TERMINAL, (int)ADD_OP},
        {SymbolType::NON_TERMINAL, (int)NonTerm::UnaryOperand},
        {SymbolType::SEMANTIC_ACTION, "unary_op"} // Действие определит знак через prevLexem
    };

    // Factor -> ID ArrayIndex
    M[NonTerm::Factor][ID] = {
        {SymbolType::TERMINAL, (int)ID},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex},
        {SymbolType::SEMANTIC_ACTION, "a"}};

    // Factor -> INT | FLOAT | STRING
    M[NonTerm::Factor][INT] = {
        {SymbolType::TERMINAL, (int)INT},
        {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::Factor][FLOAT] = {
        {SymbolType::TERMINAL, (int)FLOAT},
        {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::Factor][STRING] = {
        {SymbolType::TERMINAL, (int)STRING},
        {SymbolType::SEMANTIC_ACTION, "k"}};

    // --- 7. UnaryOperand ---
    M[NonTerm::UnaryOperand][LPAR] = {
        {SymbolType::TERMINAL, (int)LPAR},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::TERMINAL, (int)RPAR}};
    M[NonTerm::UnaryOperand][ID] = {
        {SymbolType::TERMINAL, (int)ID},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayIndex},
        {SymbolType::SEMANTIC_ACTION, "a"}};
    M[NonTerm::UnaryOperand][INT] = {
        {SymbolType::TERMINAL, (int)INT},
        {SymbolType::SEMANTIC_ACTION, "k"}};
    M[NonTerm::UnaryOperand][FLOAT] = {
        {SymbolType::TERMINAL, (int)FLOAT},
        {SymbolType::SEMANTIC_ACTION, "k"}};

    // --- 8. ArrayIndex & ArrayTail ---
    M[NonTerm::ArrayIndex][LBRACK] = {
        {SymbolType::TERMINAL, (int)LBRACK},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::NON_TERMINAL, (int)NonTerm::ArrayTail}};
    // Lambda for ArrayIndex
    for (auto t : {ASSIGN, SEMI, ADD_OP, MULT_OP, COMP_OP, RPAR, COMMA, THEN, DO, ELSE, RBRACK})
    {
        M[NonTerm::ArrayIndex][t] = {};
    }

    // ArrayTail -> ] | , Expression ]
    M[NonTerm::ArrayTail][RBRACK] = {
        {SymbolType::SEMANTIC_ACTION, "i"}, // INDEX1 - ДО закрывающей скобки
        {SymbolType::TERMINAL, (int)RBRACK}};
    M[NonTerm::ArrayTail][COMMA] = {
        {SymbolType::TERMINAL, (int)COMMA},
        {SymbolType::NON_TERMINAL, (int)NonTerm::Expression},
        {SymbolType::SEMANTIC_ACTION, "i2"}, // INDEX2 - перед закрывающей скобкой
        {SymbolType::TERMINAL, (int)RBRACK}};

    // --- 9. SemanticTrigger ---
    for (auto t : {SEMI, RPAR, RBRACK, COMMA, THEN, DO, ELSE, TERM})
    {
        M[NonTerm::SemanticTrigger][t] = {};
    }
}
void Parser::initSemanticTable()
{
    // Сохранение оператора для последующего использования
    semanticActions["save_op"] = [this]()
    { savedOperator = prevLexem.value; };

    // Бинарные операции - используют сохраненный оператор
    semanticActions["+"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };
    semanticActions["-"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };
    semanticActions["*"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };
    semanticActions["/"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };

    // Универсальные действия для операторов, зависящих от контекста
    semanticActions["add_op"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };
    semanticActions["mult_op"] = [this]()
    { rpn.push_back({RpnElementType::OPERATOR, savedOperator}); };
    semanticActions["unary_op"] = [this]()
    {
        if (prevLexem.value == "-")
        {
            rpn.push_back({RpnElementType::OPERATOR, "NEG"});
        }
        // Если "+", то ничего не делаем или игнорируем
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
        // Прошиваем предыдущую метку (адрес следующего элемента после J)
        rpn[prevLabel].value = std::to_string(rpn.size() + 2);

        labelStack.push(rpn.size());                // Сохраняем позицию для нового J
        rpn.push_back({RpnElementType::LABEL, ""}); // Placeholder
        rpn.push_back({RpnElementType::OPERATOR, "J"});
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

        // Прошиваем выход: JF должен вести на элемент после J
        rpn[exitLabel].value = std::to_string(rpn.size() + 2);

        // Генерируем переход на начало
        rpn.push_back({RpnElementType::LABEL, std::to_string(startLoop)});
        rpn.push_back({RpnElementType::OPERATOR, "J"});
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