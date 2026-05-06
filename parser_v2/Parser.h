#ifndef PARSER_H
#define PARSER_H

#include <stack>
#include <vector>
#include <functional>
#include <map>
#include <string>
#include "Lexer.h"
#include "Rpn.h"
#include "ParserTypes.h"

class Parser {
public:
    /**
     * Конструктор инициализирует лексер, семантическую таблицу и матрицу переходов.
     */
    Parser(Lexer& lexer);

    /**
     * Запуск синтаксического анализа. Возвращает вектор элементов ОПЗ[cite: 8].
     */
    std::vector<RpnElement> parse();

private:
    // --- Типы данных для матрицы переходов ---
    using Production = std::vector<StackSymbol>;

    // Управляющая матрица LL(1)[cite: 9].
    std::map<NonTerm, std::map<LexemType, Production>> M;

    // --- Состояние парсера ---
    Lexer& lexer;
    Lexem currentLexem; // Текущая лексема (lookahead)[cite: 8].
    
    /** 
     * ПРЕДЫДУЩАЯ ЛЕКСЕМА. 
     * Необходима для семантических действий, так как currentLexem обновляется 
     * сразу после сопоставления терминала.
     */
    Lexem prevLexem; 

    std::stack<StackSymbol> parseStack;
    std::stack<int> labelStack; // Стек для "прошивки" адресов меток (P1-P5)[cite: 8, 9].
    std::vector<RpnElement> rpn; // Результирующая последовательность ОПЗ[cite: 8, 9].

    // --- Таблица семантических действий ---
    std::map<std::string, std::function<void()>> semanticActions;

    // --- Внутренние методы управления ---
    void initMatrix();       // Заполнение матрицы M[cite: 9].
    void initSemanticTable(); // Регистрация процедур генерации ОПЗ[cite: 8].
    int getExtendedType(const Lexem& l); // Уникальные ID для ключевых слов[cite: 8].
    void executeAction(const std::string& action); // Вызов действия по ключу[cite: 8].
    void pushProduction(NonTerm nt, LexemType lt); // Замена нетерминала правилом[cite: 8].
};

#endif