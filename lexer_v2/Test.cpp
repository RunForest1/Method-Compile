#include <iostream>
#include <vector>
#include <iomanip>
#include "Lexer.h"
#include "Lexem.h"

void FixLocale();

/**
 * Вспомогательная функция для запуска теста и вывода результатов в виде таблицы
 */
void runTest(const std::string& testName, const std::string& source) {
    std::cout << "\n=== ЗАПУСК ТЕСТА: " << testName << " ===" << std::endl;
    std::cout << "ИСХОДНЫЙ ТЕКСТ:\n" << source << "\n" << std::endl;
    
    Lexer lexer(source);
    std::cout << std::left 
              << std::setw(15) << "ТИП" 
              << std::setw(20) << "ЗНАЧЕНИЕ" 
              << std::setw(10) << "СТР:ПОЗ" 
              << std::endl;
    std::cout << std::string(45, '-') << std::endl;

    while (true) {
        Lexem l = lexer.getNextLexem();
        
        std::cout << std::left 
                  << std::setw(15) << l.getTypeName() 
                  << std::setw(20) << ("'" + l.value + "'") 
                  << l.line << ":" << l.column 
                  << std::endl;

        if (l.type == LexemType::L_TERMINATOR) break;
        // Если хотим продолжать после ошибок, не выходим. 
        // Но обычно L_ERROR — это сигнал к остановке.
        if (l.type == LexemType::L_ERROR) {
            std::cout << ">> Остановка из-за критической ошибки." << std::endl;
            break;
        }
    }
}

int main() {
    FixLocale();
    // 0. Тест //
    std::string testSlashSlash = 
        "// haha";
    runTest("БАЗОВЫЕ ЛЕКСЕМЫ", testSlashSlash);

    // 1. Тест всех базовых лексем (идентификаторы, ключевые слова, числа)
    std::string testBasic = 
        "int a := 10;\n"
        "float b := 3.1415;\n"
        "if (a > b) then write('Greater');\n"
        "else write('Smaller');";
    
    runTest("БАЗОВЫЕ ЛЕКСЕМЫ", testBasic);

    // 2. Тест операций и сложных выражений (проверка приоритетов и склеивания)
    std::string testOps = 
        "result := (a + b) * 10 / 2.5;\n"
        "isEqual := (a == b) != (c <= d);\n"
        "check := a>b; // Склеенные операторы\n"
        "a:=b; c!=d; e<=f; g>=h;";
    
    runTest("ОПЕРАТОРЫ И СРАВНЕНИЯ", testOps);

    // 3. Тест строк и разделителей
    std::string testStrings = 
        "string s := 'Hello World';\n"
        "arr[1, 2] := [ (a + b) ];\n"
        "empty := ''; special := '!@#$%^&*()';";
    
    runTest("СТРОКИ И МАССИВЫ", testStrings);

    // 4. Тест комментариев (вложенность, переходы строк)
    std::string testComments = 
        "int x := 5; /* Это многострочный\n"
        "комментарий, который должен\n"
        "быть проигнорирован лексером */\n"
        "x := x + 1; /*** Еще один ***/ y := 10;";
    
    runTest("КОММЕНТАРИИ", testComments);

    // 5. ТЕСТ НА ОШИБКИ (самый важный для диагностики)
    // Здесь: недопустимый символ @, одиночный !, число с двумя точками, незакрытая строка
    std::string testErrors1 = 
        "int @badVariable := 100;\n";

    std::string testErrors2 = 
        "if a ! b then write('error');\n";

    std::string testErrors3 = 
        "float wrongNumber := 3.14.15;\n";

    std::string testErrors4 = 
        "string unclosed := 'This string never ends";
        

    runTest("ОШИБОЧНЫЕ СЦЕНАРИИ", testErrors1);
    runTest("ОШИБОЧНЫЕ СЦЕНАРИИ", testErrors2);
    runTest("ОШИБОЧНЫЕ СЦЕНАРИИ", testErrors3);
    runTest("ОШИБОЧНЫЕ СЦЕНАРИИ", testErrors4);

    // 6. Стресс-тест: все в одну кучу без пробелов
    std::string testDense = "if(a==b)then{write(10.5+x);}else[read(y)];";
    
    runTest("ПЛОТНЫЙ КОД БЕЗ ПРОБЕЛОВ", testDense);

    return 0;
}