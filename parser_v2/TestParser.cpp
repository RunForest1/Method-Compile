#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"

/**
 * Вспомогательная функция для вывода ОПЗ в виде строки.
 */
void printRpnAsString(const std::vector<RpnElement> &rpn)
{
    std::ostringstream oss;
    for (size_t i = 0; i < rpn.size(); ++i)
    {
        if (i > 0)
            oss << " ";

        switch (rpn[i].type)
        {
        case RpnElementType::ADDR_VAR:
            oss << "ADDR(" << rpn[i].value << ")";
            break;
        case RpnElementType::CONST_VAL:
            oss << "CONST(" << rpn[i].value << ")";
            break;
        case RpnElementType::OPERATOR:
            oss << "OP(" << rpn[i].value << ")";
            break;
        case RpnElementType::LABEL:
            oss << "LBL(" << rpn[i].value << ")";
            break;
        }
    }
    std::cout << "ОПЗ: " << oss.str() << std::endl;
}

/**
 * Запуск теста: лексический анализ -> синтаксический анализ -> генерация ОПЗ.
 */
void runParserTest(const std::string &testName, const std::string &source)
{
    std::cout << "\n=== ПАРСЕР ТЕСТ: " << testName << " ===" << std::endl;
    std::cout << "ИСХОДНЫЙ КОД:\n"
              << source << "\n"
              << std::endl;

    try
    {
        Lexer lexer(source);
        Parser parser(lexer);
        // Основной цикл разбора
        std::vector<RpnElement> rpn = parser.parse();

        std::cout << "Результат: УСПЕШНО" << std::endl;
        printRpnAsString(rpn);
    }
    catch (const std::exception &e)
    {
        std::cout << "Результат: ОШИБКА -> " << e.what() << std::endl;
    }

    std::cout << std::string(50, '=') << std::endl;
}

int main()
{
    // 1. Базовая арифметика и приоритеты
    runParserTest("АРИФМЕТИКА", "res := a + b * (c - d) / 2;");

    // 2. Условный оператор IF-THEN-ELSE
    runParserTest("УСЛОВНЫЙ ОПЕРАТОР (полный)",
                  "if x > 0 then\n"
                  "  y := 1;\n"
                  "else\n"
                  "  y := 0;");

    // 3. Условный оператор IF без ELSE
    runParserTest("УСЛОВНЫЙ ОПЕРАТОР (без else)",
                  "if flag == 1 then write(flag);");

    // 4. Цикл WHILE
    runParserTest("ЦИКЛ WHILE", "while i < 10 do i := i + 1;");

    // 5. Работа с массивами
    runParserTest("МАССИВЫ",
                  "val := arr[i];\n"
                  "matrix[i, j] := val;");

    // 6. Ввод данных
    runParserTest("ВВОД (READ)", "read(input_var); read(arr[k]);");

    // 7. Вложенные конструкции
    runParserTest("ВЛОЖЕННОСТЬ",
                  "while i < n do\n"
                  "  if arr[i] > max then\n"
                  "    max := arr[i];\n"
                  "  i := i + 1;");

    // --- ТЕСТЫ НА ОШИБКИ ---
    runParserTest("ОШИБКА: НЕТ 'THEN'", "if x > 0 res := 1;");
    runParserTest("ОШИБКА: ЛИШНЯЯ СКОБКА", "a := (b + c));");
    runParserTest("ОШИБКА: ИНДЕКС МАССИВА", "arr[i + ] := 10;");
    runParserTest("ОШИБКА: НЕИЗВЕСТНЫЙ ОПЕРАТОР", "for i := 1 to 10 do write(i);");

    return 0;
}