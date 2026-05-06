#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "Parser.h"

void printRpnTable(const std::vector<RpnElement>& rpn) {
    std::cout << std::left << std::setw(5) << "idx" << std::setw(10) << "Type" << "Value" << std::endl;
    for(size_t i=0; i<rpn.size(); ++i) {
        std::cout << std::setw(5) << i << std::setw(10) << (int)rpn[i].type << rpn[i].value << std::endl;
    }
}

void test(const std::string& name, const std::string& code) {
    std::cout << "--- TEST: " << name << " ---\nCode: " << code << std::endl;
    try {
        Lexer lexer(code);
        Parser parser(lexer);
        auto rpn = parser.parse();
        printRpnTable(rpn);
        std::cout << "Result: OK\n" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Result: ERROR -> " << e.what() << "\n" << std::endl;
    }
}

int main() {
    // Тест 1: Последовательность операторов (Program)[cite: 9]
    test("Sequence", "read(a); b := a + 1; write(b);");

    // Тест 2: Условный оператор с Else[cite: 10]
    test("If-Else", "if x > 0 then write(1); else write(0);");

    // Тест 3: Массивы и циклы[cite: 10]
    test("Arrays", "while i < 10 do { arr[i] := i; i := i + 1; }"); 

    return 0;
}