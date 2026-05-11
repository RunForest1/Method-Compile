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
    // Полный тест: Сортировка пузырьком с выводом
    runParserTest("СОРТИРОВКА ПУЗЫРЬКОМ (ПОЛНЫЙ ЦИКЛ)",
                  // 1. Чтение размера массива и его элементов
                  "read(n);\n"
                  "i := 0;\n"
                  "while i < n do {\n"
                  "  write('Greater');\n"
                  "  read(arr[i]);\n"
                  "  i := i + 1;\n"
                  "}\n"
                  "arr[1] := 123;\n"
                  "write(arr[2]);\n"

                  //   // 2. Алгоритм сортировки пузырьком (вложенные циклы и условия)
                  //   "i := 0;\n"
                  //   "while i < n - 1 do {\n"
                  //   "  j := 0;\n"
                  //   "  while j < n - i - 1 do {\n"
                  //   "    if arr[j] > arr[j + 1] then {\n"
                  //   "      temp := arr[j];\n"
                  //   "      arr[j] := arr[j + 1];\n"
                  //   "      arr[j + 1] := temp;\n"
                  //   "    }\n"
                  //   "    j := j + 1;\n"
                  //   "  }\n"
                  //   "  i := i + 1;\n"
                  //   "}\n"

                  //   // 3. Вывод отсортированного массива
                  //   "i := 0;\n"
                  //   "while i < n do {\n"
                  //   "  write(arr[i]);\n"
                  //   "  i := i + 1;\n"
                  //   "}\n"
    );

    return 0;
}