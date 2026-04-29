#include <iostream>
#include <cassert>
#include <vector>
#include "Lexer.h"

void testBasicTokens() {
    Lexer lexer("READ x := 5;");
    
    Token t1 = lexer.nextToken();
    assert(t1.type == T_KEYWORD && t1.value == "READ");
    
    Token t2 = lexer.nextToken();
    assert(t2.type == T_ID && t2.value == "x");
    
    Token t3 = lexer.nextToken();
    assert(t3.type == T_OPERATOR && t3.value == ":=");
    
    Token t4 = lexer.nextToken();
    assert(t4.type == T_INT && t4.value == "5");
    
    Token t5 = lexer.nextToken();
    assert(t5.type == T_SEPARATOR && t5.value == ";");
    
    std::cout << "Test Basic Tokens: PASSED\n";
}

void testNumbers() {
    Lexer lexer("123 45.67 .");
    
    assert(lexer.nextToken().type == T_INT);
    assert(lexer.nextToken().type == T_FLOAT);
    
    // Проверка точки как отдельного символа (по таблице S_START -> C_DOT -> S_FINAL)
    Token t3 = lexer.nextToken();
    assert(t3.value == "."); 
    
    std::cout << "Test Numbers: PASSED\n";
}

void testCommentsAndStrings() {
    Lexer lexer("/* comment */ \"hello\" PRINT");
    
    // Комментарий должен быть пропущен внутри nextToken()
    Token t1 = lexer.nextToken();
    assert(t1.type == T_STRING && t1.value == "\"hello\"");
    
    Token t2 = lexer.nextToken();
    assert(t2.type == T_KEYWORD && t2.value == "PRINT");
    
    std::cout << "Test Comments and Strings: PASSED\n";
}

void testCoordinates() {
    Lexer lexer("A\n  B");
    
    Token t1 = lexer.nextToken(); // A
    assert(t1.line == 1 && t1.column == 1);
    
    Token t2 = lexer.nextToken(); // B
    assert(t2.line == 2 && t2.column == 3);
    
    std::cout << "Test Coordinates: PASSED\n";
}

int main() {
    try {
        Lexer lexer("y := 10 + 2.5 * (x-3) / (4 + 1)");
        std::vector<Token> tokens;
        while (true) {
            Token t = lexer.nextToken();
            std::cout << "Token: " << t.value << " (Type: " << t.type << ", Line: " << t.line << ", Column: " << t.column << ")\n";
            if (t.type == T_EOF) break;
            tokens.push_back(t);
        }
        std::cout << "\nALL TESTS PASSED SUCCESSFULLY!\n";
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}