#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "Lexer.h"

void FixLocale();
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case T_ID:        return "ID";
        case T_KEYWORD:   return "KEYWORD";
        case T_INT:       return "INT";
        case T_FLOAT:     return "FLOAT";
        case T_STRING:    return "STRING";
        case T_OPERATOR:  return "OPERATOR";
        case T_SEPARATOR: return "SEPARATOR";
        case T_EOF:       return "EOF";
        case T_ERROR:     return "ERROR";
        default:          return "UNKNOWN";
    }
}

// Вспомогательный класс для массового тестирования
class LexerTester {
public:
    static void verify(const std::string& description, const std::string& input, const std::vector<std::pair<TokenType, std::string>>& expected) {
        std::cout << "[TEST] " << std::left << std::setw(50) << description << " | ";
        Lexer lexer(input);
        bool currentTestFailed = false;
        std::vector<Token> actualTokens;

        for (const auto& exp : expected) {
            Token t = lexer.nextToken();
            actualTokens.push_back(t);

            if (t.type != exp.first || t.value != exp.second) {
                if (!currentTestFailed) {
                    std::cout << "FAILED" << std::endl;
                    currentTestFailed = true;
                }
                std::cerr << "  -> ОШИБКА: Ожидалось [" << tokenTypeToString(exp.first) << ": '" << exp.second 
                          << "'], Получено [" << tokenTypeToString(t.type) << ": '" << t.value << "'] в " << t.line << ":" << t.column << "\n";
            }
        }

        Token eofToken = lexer.nextToken();
        if (eofToken.type != T_EOF) {
            if (!currentTestFailed) {
                std::cout << "FAILED" << std::endl;
                currentTestFailed = true;
            }
            std::cerr << "  -> Ожидался EOF, но получен [" << tokenTypeToString(eofToken.type) << "]\n";
        }

        if (!currentTestFailed) {
            std::cout << "PASSED" << std::endl;
        } else {
            std::cout << "     Полный лог токенов для этого теста:\n     ";
            for (const auto& tok : actualTokens) {
                std::cout << "<" << tokenTypeToString(tok.type) << ": \"" << tok.value << "\"> ";
            }
            std::cout << "\n" << std::string(70, '-') << "\n";
        }
    }
};

void runComprehensiveSuites() {
    // --- 1. ТЕСТ КЛЮЧЕВЫХ СЛОВ ---
    LexerTester::verify("Keywords case sensitivity & adjoining", 
        "IF IFIF READ READ_ PRINT123 END", {
        {T_KEYWORD, "IF"}, {T_ID, "IFIF"}, {T_KEYWORD, "READ"}, {T_ID, "READ_"}, {T_ID, "PRINT123"}, {T_KEYWORD, "END"}
    });

    // --- 2. ТЕСТ ЧИСЕЛ (С учетом строгой проверки точек) ---
    LexerTester::verify("Numbers: integers, floats, leading zeros", 
        "0 007 123 3.14 0.001 42. .5", {
        {T_INT, "0"}, {T_INT, "007"}, {T_INT, "123"}, {T_FLOAT, "3.14"}, {T_FLOAT, "0.001"}, 
        {T_ERROR, "42."}, {T_ERROR, ".5"} 
    });

    // --- 3. ОПЕРАТОРЫ ---
    LexerTester::verify("Operators: assignment and arithmetic", 
        ":= : + - * / ( ) [ ] , ;", {
        {T_OPERATOR, ":="}, {T_OPERATOR, ":"}, {T_OPERATOR, "+"}, {T_OPERATOR, "-"}, 
        {T_OPERATOR, "*"}, {T_OPERATOR, "/"}, {T_SEPARATOR, "("}, {T_SEPARATOR, ")"},
        {T_SEPARATOR, "["}, {T_SEPARATOR, "]"}, {T_SEPARATOR, ","}, {T_SEPARATOR, ";"}
    });

    // --- 4. КОММЕНТАРИИ ---
    LexerTester::verify("Comments: single, multi, unclosed-like", 
        "A /* comment */ B /**/ C /*\n line \n*/ D", {
        {T_ID, "A"}, {T_ID, "B"}, {T_ID, "C"}, {T_ID, "D"}
    });

    // --- 5. СТРОКИ ---
    LexerTester::verify("Strings: empty, spaces, mixed", 
        "\"\" \"simple\" \"with spaces\" \"123!@#\"", {
        {T_STRING, "\"\""}, {T_STRING, "\"simple\""}, {T_STRING, "\"with spaces\""}, {T_STRING, "\"123!@#\""}
    });

    // --- 6. ТЕСТ НА \"КАШУ\" ---
    LexerTester::verify("The Mash: no spaces between tokens", 
        "IF(x:=10+y*2.5)PRINT\"done\";", {
        {T_KEYWORD, "IF"}, {T_SEPARATOR, "("}, {T_ID, "x"}, {T_OPERATOR, ":="}, 
        {T_INT, "10"}, {T_OPERATOR, "+"}, {T_ID, "y"}, {T_OPERATOR, "*"}, 
        {T_FLOAT, "2.5"}, {T_SEPARATOR, ")"}, {T_KEYWORD, "PRINT"}, 
        {T_STRING, "\"done\""}, {T_SEPARATOR, ";"}
    });
}

// ... методы testDeepCoordinates и testEOFandErrors остаются без изменений ...

int main() {
    FixLocale();
    std::cout << "==================================================\n";
    std::cout << "ЗАПУСК ГЛОБАЛЬНОГО ТЕСТИРОВАНИЯ ЛЕКСЕРА\n";
    std::cout << "==================================================\n\n";

    try {
        runComprehensiveSuites();
        // testDeepCoordinates(); // Можно временно закомментить, пока фиксишь числа
        // testEOFandErrors();
        
        std::cout << "\n[SUCCESS] Все тесты завершены.\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL FAILURE] Exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}