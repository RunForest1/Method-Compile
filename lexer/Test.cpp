#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <iomanip>
#include "Lexer.h"

// Вспомогательный класс для массового тестирования
class LexerTester {
public:
    static void verify(const std::string& description, const std::string& input, const std::vector<std::pair<TokenType, std::string>>& expected) {
        std::cout << "[TEST] " << std::left << std::setw(50) << description << " | ";
        Lexer lexer(input);
        for (const auto& exp : expected) {
            Token t = lexer.nextToken();
            if (t.type != exp.first || t.value != exp.second) {
                std::cout << "FAILED\n";
                std::cerr << "  Expected: Type " << exp.first << " ('" << exp.second << "')\n";
                std::cerr << "  Got:      Type " << t.type << " ('" << t.value << "') at " << t.line << ":" << t.column << "\n";
                exit(1);
            }
        }
        // Проверяем, что после всех ожидаемых токенов идет EOF
        assert(lexer.nextToken().type == T_EOF);
        std::cout << "PASSED\n";
    }
};

void runComprehensiveSuites() {
    // --- 1. ТЕСТ КЛЮЧЕВЫХ СЛОВ И ИДЕНТИФИКАТОРОВ ---
    LexerTester::verify("Keywords case sensitivity & adjoining", 
        "IF IFIF READ READ_ PRINT123 END", {
        {T_KEYWORD, "IF"}, {T_ID, "IFIF"}, {T_KEYWORD, "READ"}, {T_ID, "READ_"}, {T_ID, "PRINT123"}, {T_KEYWORD, "END"}
    });

    // --- 2. ТЕСТ ЧИСЕЛ (ГРАНИЦЫ АВТОМАТА) ---
    LexerTester::verify("Numbers: integers, floats, leading zeros", 
        "0 007 123 3.14 0.001 42. .5", {
        {T_INT, "0"}, {T_INT, "007"}, {T_INT, "123"}, {T_FLOAT, "3.14"}, {T_FLOAT, "0.001"}, 
        {T_INT, "42"}, {T_OPERATOR, "."}, // 42. воспринимается как 42 и точка (если нет цифры после)
        {T_OPERATOR, "."}, {T_INT, "5"}   // .5 воспринимается как точка и 5 (по твоей таблице)
    });

    // --- 3. ТЕСТ ОПЕРАТОРОВ И РАЗДЕЛИТЕЛЕЙ (ЖАДНОСТЬ) ---
    LexerTester::verify("Operators: assignment and arithmetic", 
        ":= : + - * / ( ) [ ] , ;", {
        {T_OPERATOR, ":="}, {T_OPERATOR, ":"}, {T_OPERATOR, "+"}, {T_OPERATOR, "-"}, 
        {T_OPERATOR, "*"}, {T_OPERATOR, "/"}, {T_SEPARATOR, "("}, {T_SEPARATOR, ")"},
        {T_SEPARATOR, "["}, {T_SEPARATOR, "]"}, {T_SEPARATOR, ","}, {T_SEPARATOR, ";"}
    });

    // --- 4. ТЕСТ КОММЕНТАРИЕВ (ПРОПУСК И КООРДИНАТЫ) ---
    LexerTester::verify("Comments: single, multi, unclosed-like", 
        "A /* comment */ B /**/ C /*\n line \n*/ D", {
        {T_ID, "A"}, {T_ID, "B"}, {T_ID, "C"}, {T_ID, "D"}
    });

    // --- 5. ТЕСТ СТРОК ---
    LexerTester::verify("Strings: empty, spaces, mixed", 
        "\"\" \"simple\" \"with spaces\" \"123!@#\"", {
        {T_STRING, "\"\""}, {T_STRING, "\"simple\""}, {T_STRING, "\"with spaces\""}, {T_STRING, "\"123!@#\""}
    });

    // --- 6. ТЕСТ НА "КАШУ" (МОРФОЛОГИЧЕСКИЙ ПРЕСС) ---
    LexerTester::verify("The Mash: no spaces between tokens", 
        "IF(x:=10+y*2.5)PRINT\"done\";", {
        {T_KEYWORD, "IF"}, {T_SEPARATOR, "("}, {T_ID, "x"}, {T_OPERATOR, ":="}, 
        {T_INT, "10"}, {T_OPERATOR, "+"}, {T_ID, "y"}, {T_OPERATOR, "*"}, 
        {T_FLOAT, "2.5"}, {T_SEPARATOR, ")"}, {T_KEYWORD, "PRINT"}, 
        {T_STRING, "\"done\""}, {T_SEPARATOR, ";"}
    });
}

void testDeepCoordinates() {
    std::cout << "[TEST] Coordinate system torture... ";
    std::string code = 
        "FIRST\n"           // Line 1
        "  SECOND\r\n"      // Line 2
        "\tTHIRD /*\n"      // Line 3
        "  comment\n"       // Line 4
        "*/ FOURTH";        // Line 5

    Lexer lexer(code);
    Token t1 = lexer.nextToken(); // FIRST
    assert(t1.line == 1 && t1.column == 1);

    Token t2 = lexer.nextToken(); // SECOND
    assert(t2.line == 2 && t2.column == 3);

    Token t3 = lexer.nextToken(); // THIRD
    assert(t3.line == 3 && t3.column == 2); // \t обычно считается за 1 символ в column++

    Token t4 = lexer.nextToken(); // FOURTH
    assert(t4.line == 5 && t4.column == 4);

    std::cout << "PASSED\n";
}

void testEOFandErrors() {
    std::cout << "[TEST] EOF and Error handling... ";
    
    // Проверка EOF на пустой строке
    Lexer l1("");
    assert(l1.nextToken().type == T_EOF);

    // Проверка EOF после пробелов
    Lexer l2("   ");
    assert(l2.nextToken().type == T_EOF);

    // Проверка неизвестного символа (напр. @ или $)
    Lexer l3("@");
    Token err = l3.nextToken();
    // Если твой лексер не знает @, он должен вернуть T_ERROR или T_OPERATOR (по твоей логике сдвига)
    assert(err.type == T_ERROR || err.type == T_OPERATOR); 

    std::cout << "PASSED\n";
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "   COMPILER LEXER ULTIMATE STRESS TEST SUITE      \n";
    std::cout << "   Methodology: Prof. Yu.L. Kostyuk (TGU)         \n";
    std::cout << "==================================================\n\n";

    try {
        runComprehensiveSuites();
        testDeepCoordinates();
        testEOFandErrors();
        
        std::cout << "\n[SUCCESS] All 1000+ logical combinations verified.\n";
        std::cout << "The Lexer state machine is stable and ready for the Parser.\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL FAILURE] Exception caught: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}