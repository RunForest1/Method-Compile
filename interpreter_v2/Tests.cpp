#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

// Подключаем все компоненты твоего транслятора
#include "Interpreter.h"       // Этот лежит в той же папке
#include "value.h"             // И этот тоже
#include "../lexer_v2/Lexer.h"
#include "../parser_v2/Parser.h"
#include "../parser_v2/Rpn.h"

#ifdef _WIN32
    #include <windows.h>
#endif
void FixLocale(){
    SetConsoleOutputCP(CP_UTF8);  // Enable UTF-8 output
    SetConsoleCP(CP_UTF8);         // Enable UTF-8 input (optional)
}

/**
 * Вспомогательная функция для красивого вывода ОПЗ (для отладки)
 */
void printRpnDebug(const std::vector<RpnElement> &rpn) {
    std::cout << "--- Сгенерированная ОПЗ ---" << std::endl;
    for (size_t i = 0; i < rpn.size(); ++i) {
        std::cout << std::setw(3) << i << ": ";
        switch (rpn[i].type) {
            case RpnElementType::ADDR_VAR:  std::cout << "ADDR(" << rpn[i].value << ")"; break;
            case RpnElementType::CONST_VAL: std::cout << "CONST(" << rpn[i].value << ")"; break;
            case RpnElementType::OPERATOR:  std::cout << "OP(" << rpn[i].value << ")"; break;
            case RpnElementType::LABEL:     std::cout << "LBL(" << rpn[i].value << ")"; break;
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------------" << std::endl;
}

/**
 * Функция запуска полной цепочки: Код -> Лексер -> Парсер -> Интерпретатор
 */
void runFullChainTest(const std::string &testName, const std::string &source) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "ТЕСТ: " << testName << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "ИСХОДНЫЙ КОД:\n" << source << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    try {
        // 1. Лексический и синтаксический анализ + генерация ОПЗ
        std::cout << "[1/3] Компиляция (Лексер + Парсер)..." << std::endl;
        Lexer lexer(source);
        Parser parser(lexer);
        std::vector<RpnElement> rpn = parser.parse();
        std::cout << ">>> Компиляция завершена успешно." << std::endl;

        // Вывод ОПЗ для контроля
        printRpnDebug(rpn);

        // 2. Инициализация интерпретатора
        std::cout << "[2/3] Инициализация интерпретатора..." << std::endl;
        Interpreter interpreter(rpn);

        // 3. Выполнение
        std::cout << "[3/3] Запуск выполнения..." << std::endl;
        std::cout << "--- ВЫВОД ПРОГРАММЫ ---" << std::endl;
        
        interpreter.run();
        
        std::cout << "--- КОНЕЦ ВЫВОДА ---" << std::endl;
        std::cout << ">>> Выполнение успешно завершено." << std::endl;

    } catch (const std::exception &e) {
        // Ловим ошибки на любом из этапов
        std::cerr << "\n[!] КРИТИЧЕСКАЯ ОШИБКА: " << e.what() << std::endl;
    }

    std::cout << std::string(60, '=') << std::endl;
}

int main() {
    FixLocale();

    // --- ТЕСТ 1: Базовые математические функции ---
    std::string mathBasicCode = 
        "a := sin(0);\n"          // Ожидаем 0.0
        "b := cos(0);\n"          // Ожидаем 1.0
        "c := exp(1);\n"          // Ожидаем ~2.71828
        "d := log(2.718281828);\n" // Ожидаем ~1.0
        "e := power(2, 3);\n"     // Ожидаем 8.0
        "write('--- Basic Math ---');\n"
        "write(a); write(b); write(c); write(d); write(e);\n";

    // --- ТЕСТ 2: Тригонометрическое тождество (sin^2 + cos^2 = 1) ---
    // Проверяем точность рядов Тейлора и работу вложенных вызовов
    std::string mathIdentityCode = 
        "angle := 0.5;\n"
        "s := sin(angle);\n"
        "c := cos(angle);\n"
        "res := power(s, 2) + power(c, 2);\n"
        "write('--- Identity (sin^2 + cos^2) ---');\n"
        "write(res);\n"; // Должно быть максимально близко к 1.0

    // --- ТЕСТ 3: Сложные вложенные вычисления и унарный минус ---
    // Вычисляем: -exp(sin(PI/2)) -> -exp(1) -> -2.71828
    std::string mathComplexCode = 
        "pi := 3.14159265;\n"
        "val := -exp(sin(pi / 2));\n" 
        "write('--- Complex Nesting & NEG ---');\n"
        "write(val);\n";

    // --- ТЕСТ 4: Работа с отрицательными числами и типами ---
    std::string mathTypeTest = 
        "x := 10.5 - 5;\n"       // Float - Int
        "y := power(2.0, -2);\n" // 2^-2 = 0.25 (если реализовал логарифм для отрицательных e)
        "write('--- Types & Fractions ---');\n"
        "write(x);\n"
        "write(y);\n";

    // --- ТЕСТ 5: fizz Buzz ---
    std::string fizzBuzzCode = 
        "n := 15;\n"
        "i := 1;\n"
        "while i <= n do {\n"
        "  if (i / 3) * 3 == i then {\n"
        "    if (i / 5) * 5 == i then {\n"
        "      write('FizzBuzz');\n"
        "    } else {\n"
        "      write('Fizz');\n"
        "    }\n"
        "  } else {\n"
        "    if (i / 5) * 5 == i then {\n"
        "      write('Buzz');\n"
        "    } else {\n"
        "      write(i);\n"
        "    }\n"
        "  }\n"
        "  i := i + 1;\n"
        "}\n";
    
    // Решето Эратосфена
    std::string sieveCode = 
        "limit := 20;\n"
        "n := 2;\n"
        "while n <= limit do {\n"
        "  isprime[n] := 1;\n"
        "  n := n + 1;\n"
        "}\n"
        "p := 2;\n"
        "while p * p <= limit do {\n"
        "  if isprime[p] == 1 then {\n"
        "    step := p * p;\n"
        "    while step <= limit do {\n"
        "      isprime[step] := 0;\n"
        "      step := step + p;\n"
        "    }\n"
        "  }\n"
        "  p := p + 1;\n"
        "}\n"
        "write('Primes up to 20 (expected: 2, 3, 5, 7, 11, 13, 17, 19):');\n"
        "k := 2;\n"
        "while k <= limit do {\n"
        "  if isprime[k] == 1 then { write(k); }\n"
        "  k := k + 1;\n"
        "}\n";  
        
    // Приоритет вычислений в индексах
    std::string complexIndexCode = 
        "base := 1;\n"
        "arr[base + 1] := 999;\n" // arr[2] := 999
        "idx := sin(0) + 2;\n"    // idx := 2.0
        "write('Complex Index (expected 999):');\n"
        "write(arr[idx]);\n";

    // Тест на вложенные массивы
    std::string nestedMatrixCode = 
        "i := 0;\n"
        "while i < 2 do {\n"
        "    j := 0;\n"
        "    while j < 2 do {\n"
        "        matrix[i, j] := i + j;\n"
        "        j := j + 1;\n"
        "    }\n"
        "    i := i + 1;\n"
        "}\n"
        "write('Matrix element [1, 1] (expected 2):');\n"
        "write(matrix[1, 1]);\n"
        "indices[0] := 1;\n"
        "indices[1] := 0;\n"
        "val := matrix[indices[0], indices[1]];\n"
        "write('Complex nested access [1, 0] (expected 1):');\n"
        "write(val);\n"
        "a := 1;\n"
        "matrix[matrix[0, a], indices[0]] := 888;\n"
        "write('After recursive write [1, 1] (expected 888):');\n"
        "write(matrix[1, 1]);\n";

    runFullChainTest("FIZZBUZZ (Вложенные IF)", fizzBuzzCode);
    runFullChainTest("БАЗОВАЯ МАТЕМАТИКА", mathBasicCode);
    runFullChainTest("ТОЖДЕСТВО И ТОЧНОСТЬ", mathIdentityCode);
    runFullChainTest("ВЛОЖЕННОСТЬ И УНАРНЫЙ МИНУС", mathComplexCode);
    runFullChainTest("ТИПЫ ДАННЫХ", mathTypeTest);

    runFullChainTest("Решето Эратосфена", sieveCode);
    runFullChainTest("Приоритет вычислений в индексах", complexIndexCode);
    runFullChainTest("Вложенные массивы", nestedMatrixCode);

}

//g++ interpreter_v2/Tests.cpp interpreter_v2/Interpreter.cpp interpreter_v2/value.cpp lexer_v2/Lexer.cpp lexer_v2/Lexem.cpp parser_v2/Parser.cpp -I./lexer_v2 -I./parser_v2 -I./interpreter_v2 -o interpreter_v2\compiler_test 