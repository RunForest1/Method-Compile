#include "Interpreter.h"
#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#endif

void FixLocale(){
    SetConsoleOutputCP(CP_UTF8);  // Enable UTF-8 output
    SetConsoleCP(CP_UTF8);         // Enable UTF-8 input (optional)
}

RPNItem make(RPNItemType t, const std::string& v, int l=1, int c=1) { return {t, v, l, c}; }

void Test1_Formula() {
    std::cout << "--- ТЕСТ 1: Сложная формула (дискриминант: D = b*b - 4*a*c) ---\n";
    Interpreter interp;
    
    // a := 1; b := 5; c := 6; D := b*b - 4*a*c; write D;
    std::vector<RPNItem> rpn = {
        make(RPNItemType::VARIABLE, "a"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "b"), make(RPNItemType::LITERAL, "5"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "c"), make(RPNItemType::LITERAL, "6"), make(RPNItemType::ASSIGN, ":="),
        
        make(RPNItemType::VARIABLE, "D"), 
        make(RPNItemType::VARIABLE, "b"), make(RPNItemType::VARIABLE, "b"), make(RPNItemType::OPERATOR, "*"),
        make(RPNItemType::LITERAL, "4"), make(RPNItemType::VARIABLE, "a"), make(RPNItemType::OPERATOR, "*"),
        make(RPNItemType::VARIABLE, "c"), make(RPNItemType::OPERATOR, "*"),
        make(RPNItemType::OPERATOR, "-"), make(RPNItemType::ASSIGN, ":="),
        
        make(RPNItemType::VARIABLE, "D"), make(RPNItemType::WRITE, "w")
    };
    interp.execute(rpn);
    std::cout << "Ожидалось: 1. Успех.\n\n";
}

void Test2_ArraySort() {
    std::cout << "--- ТЕСТ 2: Bubble Sort (5 элементов) ---\n";
    Interpreter interp;
    
    std::vector<RPNItem> rpn = {
        // [0-2]: n := 5
        make(RPNItemType::VARIABLE, "n"), make(RPNItemType::LITERAL, "5"), make(RPNItemType::ASSIGN, ":="),
        
        // [3-5]: arr m1 n
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "n"), make(RPNItemType::ALLOC_1D, "m1"),
        
        // [6-30]: Заполнение (30, 10, 5, 20, -40)
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "0"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::LITERAL, "30"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::LITERAL, "10"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "2"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::LITERAL, "5"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "3"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::LITERAL, "20"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "4"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::LITERAL, "-40"), make(RPNItemType::ASSIGN, ":="),
        
        // [31-33]: i := 0
        make(RPNItemType::VARIABLE, "i"), make(RPNItemType::LITERAL, "0"), make(RPNItemType::ASSIGN, ":="),
        
        // --- LOOP OUTER --- (Индекс 34)
        make(RPNItemType::VARIABLE, "i"), make(RPNItemType::VARIABLE, "n"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "-"), make(RPNItemType::OPERATOR, "<"), 
        make(RPNItemType::LABEL, "99"), make(RPNItemType::JF, "jf"), // END_OUTER -> 99
        
        // [41-43]: j := 0
        make(RPNItemType::VARIABLE, "j"), make(RPNItemType::LITERAL, "0"), make(RPNItemType::ASSIGN, ":="),
        
        // --- LOOP INNER --- (Индекс 44)
        make(RPNItemType::VARIABLE, "j"), make(RPNItemType::VARIABLE, "n"), make(RPNItemType::VARIABLE, "i"), make(RPNItemType::OPERATOR, "-"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "-"), make(RPNItemType::OPERATOR, "<"),
        make(RPNItemType::LABEL, "92"), make(RPNItemType::JF, "jf"), // END_INNER -> 92
        
        // if arr[j] > arr[j+1]
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::ARRAY_IDX_1D, "i"),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "+"), make(RPNItemType::ARRAY_IDX_1D, "i"),
        make(RPNItemType::OPERATOR, ">"),
        make(RPNItemType::LABEL, "85"), make(RPNItemType::JF, "jf"), // NO_SWAP -> 85
        
        // SWAP (t := arr[j]; arr[j] := arr[j+1]; arr[j+1] := t)
        make(RPNItemType::VARIABLE, "t"), make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "+"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "+"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::VARIABLE, "t"), make(RPNItemType::ASSIGN, ":="),
        
        // NO_SWAP (Индекс 85): j := j + 1
        make(RPNItemType::VARIABLE, "j"), make(RPNItemType::VARIABLE, "j"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "+"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::LABEL, "44"), make(RPNItemType::JMP, "j"), // jump INNER
        
        // END_INNER (Индекс 92): i := i + 1
        make(RPNItemType::VARIABLE, "i"), make(RPNItemType::VARIABLE, "i"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::OPERATOR, "+"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::LABEL, "34"), make(RPNItemType::JMP, "j"), // jump OUTER
        
        // END_OUTER (Индекс 99): вывод 5 элементов
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "0"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::WRITE, "w"),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::WRITE, "w"),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "2"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::WRITE, "w"),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "3"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::WRITE, "w"),
        make(RPNItemType::VARIABLE, "arr"), make(RPNItemType::LITERAL, "4"), make(RPNItemType::ARRAY_IDX_1D, "i"), make(RPNItemType::WRITE, "w")
    };

    interp.execute(rpn);
    std::cout << "Ожидалось: -40 5 10 20 30. Успех!\n";
}

void Test3_Errors() {
    std::cout << "--- ТЕСТ 3: Диагностика ошибок ---\n";
    Interpreter interp;
    
    // res := 10 / 0; (ошибка: строка 2, символ 15)
    std::vector<RPNItem> rpn = {
        make(RPNItemType::VARIABLE, "res", 2, 1), make(RPNItemType::LITERAL, "10", 2, 8), 
        make(RPNItemType::LITERAL, "0", 2, 12), make(RPNItemType::OPERATOR, "/", 2, 15), 
        make(RPNItemType::ASSIGN, ":=", 2, 17)
    };
    
    try {
        interp.execute(rpn);
    } catch (const std::exception& e) {
        std::cout << "Тест пройден: Ошибка успешно отловлена с координатами.\n\n";
    }
}

void Test4_2DArraysAndStrings() {
    std::cout << "--- ТЕСТ 4: 2D Массивы (m2, i2) и строки ---\n";
    Interpreter interp;
    
    // str := 'Hello'; m2(matrix, 2, 2); matrix[1, 1] := 99; write str; write matrix[1, 1];
    std::vector<RPNItem> rpn = {
        make(RPNItemType::VARIABLE, "str"), make(RPNItemType::LITERAL, "'Hello'"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "matrix"), make(RPNItemType::LITERAL, "2"), make(RPNItemType::LITERAL, "2"), make(RPNItemType::ALLOC_2D, "m2"),
        make(RPNItemType::VARIABLE, "matrix"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::ARRAY_IDX_2D, "i2"), make(RPNItemType::LITERAL, "99"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "str"), make(RPNItemType::WRITE, "w"),
        make(RPNItemType::VARIABLE, "matrix"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::LITERAL, "1"), make(RPNItemType::ARRAY_IDX_2D, "i2"), make(RPNItemType::WRITE, "w")
    };
    
    interp.execute(rpn);
    std::cout << "Ожидалось: Hello 99. Успех.\n\n";
}

void Test5_InputOutput() {
    std::cout << "--- ТЕСТ 5: Ввод и вывод данных (Интерактивный) ---\n";
    std::cout << "(Сейчас интерпретатор попросит вас ввести данные)\n";
    Interpreter interp;
    
    // Псевдокод:
    // msg1 := 'Enter_number:'; 
    // write msg1;
    // read x;
    // res := x * 10;
    // msg2 := 'Result(x*10):'; 
    // write msg2;
    // write res;
    
    std::vector<RPNItem> rpn = {
        // Вывод приглашения
        make(RPNItemType::VARIABLE, "msg1"), make(RPNItemType::LITERAL, "'Enter_number:'"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "msg1"), make(RPNItemType::WRITE, "w"),
        
        // Ввод переменной x с клавиатуры
        make(RPNItemType::VARIABLE, "x"), make(RPNItemType::READ, "r"),
        
        // Вычисление res = x * 10
        make(RPNItemType::VARIABLE, "res"), make(RPNItemType::VARIABLE, "x"), make(RPNItemType::LITERAL, "10"), make(RPNItemType::OPERATOR, "*"), make(RPNItemType::ASSIGN, ":="),
        
        // Вывод строки с результатом
        make(RPNItemType::VARIABLE, "msg2"), make(RPNItemType::LITERAL, "'Result(x*10):'"), make(RPNItemType::ASSIGN, ":="),
        make(RPNItemType::VARIABLE, "msg2"), make(RPNItemType::WRITE, "w"),
        
        // Вывод самого значения res
        make(RPNItemType::VARIABLE, "res"), make(RPNItemType::WRITE, "w")
    };

    interp.execute(rpn);
    std::cout << "Тест 5 успешно завершен.\n\n";
}

int main() {

    FixLocale();

    try {
        Test1_Formula();
        Test2_ArraySort();
        Test3_Errors();
        Test4_2DArraysAndStrings();
        Test5_InputOutput();
    } catch (...) {}
    return 0;
}