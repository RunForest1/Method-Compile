ТРАНСЛЯТОР-ИНТЕРПРЕТАТОР ДЛЯ ЯЗЫКА ПРОГРАММИРОВАНИЯ
====================================================

## 1. ЛЕКСЕМЫ (Tokens)

### Типы токенов и их коды:

| Код | Название      | Примеры                              |
|-----|---------------|--------------------------------------|
| 0   | T_INT         | 42, 0, 123                          |
| 1   | T_FLOAT       | 3.14, 0.5, 2.0                      |
| 2   | T_ID          | x, var1, count                      |
| 3   | T_KEYWORD     | READ, PRINT, IF, THEN, ELSE, WHILE  |
| 4   | T_OPERATOR    | +, -, *, /, =, <, >, <=, >=, ==, != |
| 5   | T_SEPARATOR   | (, ), ,, ;, [, ], {, }              |
| 6   | T_STRING      | "hello", "text"                     |
| 7   | T_EOF         | (конец файла)                       |
| 8   | T_ERROR       | (ошибочный токен)                   |

### Таблица переходов лексического автомата:

Состояния:
- S_START (начальное)
- S_ID (идентификатор)
- S_INT (целое число)
- S_FLOAT (вещественное число)
- S_STRING (строка)
- S_OPERATOR (оператор)

Классы символов:
- C_LETTER (буквы, _)
- C_DIGIT (цифры)
- C_DOT (точка)
- C_SPACE (пробелы)
- C_OP (операторы)
- C_SEP (разделители)
- C_QUOTE (кавычки)
- C_OTHER (остальные)

Переходы:
{S_START, C_LETTER} → S_ID
{S_START, C_DIGIT} → S_INT
{S_START, C_QUOTE} → S_STRING
{S_START, C_OP}    → S_OPERATOR
{S_ID, C_LETTER}   → S_ID
{S_ID, C_DIGIT}    → S_ID
{S_INT, C_DIGIT}   → S_INT
{S_INT, C_DOT}     → S_FLOAT
{S_FLOAT, C_DIGIT} → S_FLOAT

---

## 2. КС-ГРАММАТИКА ЯЗЫКА

### Исходная КС-грамматика:

```
Program      ::= StatementList
StatementList ::= Statement | Statement StatementList
Statement    ::= Assignment | IfStatement | WhileStatement | 
                 ReadStatement | PrintStatement

Assignment   ::= ID '=' Expression ';'
IfStatement  ::= 'IF' Expression 'THEN' StatementList 'ELSE' StatementList 'END' ';'
               | 'IF' Expression 'THEN' StatementList 'END' ';'
WhileStatement ::= 'WHILE' Expression 'DO' StatementList 'END' ';'
ReadStatement  ::= 'READ' ID ';'
PrintStatement ::= 'PRINT' Expression ';'

Expression   ::= LogicalOr
LogicalOr    ::= LogicalAnd ('||' LogicalAnd)*
LogicalAnd   ::= Comparison ('&&' Comparison)*
Comparison   ::= Additive (('<'|'>'|'<='|'>='|'=='|'!=') Additive)*
Additive     ::= Multiplicative (('+'|'-') Multiplicative)*
Multiplicative ::= Unary (('*'|'/') Unary)*
Unary        ::= ('!'|'-')? Primary
Primary      ::= INT | FLOAT | STRING | ID | '(' Expression ')'
```

---

## 3. НОРМАЛЬНАЯ ФОРМА ГРЕЙБАХ (Упрощенная)

Преобразование грамматики в форму Грейбах достигает того, что каждое правило либо:
- Начинается с терминала
- Является пустым (ε)

Упрощенная версия для этого языка:

```
Program      → S EOF
S            → Assignment S | IfStmt S | WhileStmt S | ReadStmt S | PrintStmt S | ε

Assignment   → ID = E ;
IfStmt       → IF E THEN S Else END ;
Else         → ELSE S | ε
WhileStmt    → WHILE E DO S END ;
ReadStmt     → READ ID ;
PrintStmt    → PRINT E ;

E            → LogOr
LogOr        → LogAnd LogOr'
LogOr'       → || LogAnd LogOr' | ε
LogAnd       → Cmp LogAnd'
LogAnd'      → && Cmp LogAnd' | ε
Cmp          → Add Cmp'
Cmp'         → <  Add Cmp' | >  Add Cmp' | <= Add Cmp' | 
               >= Add Cmp' | == Add Cmp' | != Add Cmp' | ε
Add          → Mul Add'
Add'         → + Mul Add' | - Mul Add' | ε
Mul          → Unary Mul'
Mul'         → * Unary Mul' | / Unary Mul' | ε
Unary        → ! Unary | - Unary | Primary
Primary      → INT | FLOAT | STRING | ID | ( E )
```

---

## 4. СЕМАНТИЧЕСКИЕ ДЕЙСТВИЯ ДЛЯ ГЕНЕРАЦИИ ОПС

### Правила генерации кода:

#### Числа и переменные:
```
Primary → INT       { emit(PUSH_INT, value) }
Primary → FLOAT     { emit(PUSH_FLOAT, value) }
Primary → STRING    { emit(PUSH_STRING, value) }
Primary → ID        { emit(PUSH_VAR, id) }
Primary → ( E )     { E.code }
```

#### Арифметические операции:
```
Mul → Unary Mul'
Mul' → * Unary Mul'    { emit(MUL); Mul'.code }
Mul' → / Unary Mul'    { emit(DIV); Mul'.code }
Mul' → ε

Add → Mul Add'
Add' → + Mul Add'      { emit(ADD); Add'.code }
Add' → - Mul Add'      { emit(SUB); Add'.code }
Add' → ε
```

#### Сравнения:
```
Cmp → Add Cmp'
Cmp' → < Add Cmp'      { emit(LT); Cmp'.code }
Cmp' → > Add Cmp'      { emit(GT); Cmp'.code }
Cmp' → <= Add Cmp'     { emit(LE); Cmp'.code }
Cmp' → >= Add Cmp'     { emit(GE); Cmp'.code }
Cmp' → == Add Cmp'     { emit(EQ); Cmp'.code }
Cmp' → != Add Cmp'     { emit(NE); Cmp'.code }
Cmp' → ε
```

#### Логические операции:
```
LogAnd' → && Cmp LogAnd'  { emit(AND); LogAnd'.code }
LogOr'  → || LogAnd LogOr' { emit(OR); LogOr'.code }
```

#### Унарные операции:
```
Unary → - Unary           { Unary.code; emit(NEG) }
Unary → ! Unary           { Unary.code; emit(NOT) }
```

#### Присваивание:
```
Assignment → ID = E ;     { E.code; emit(POP_VAR, id) }
```

#### Условный оператор:
```
IfStmt → IF E THEN S Else END ;
{
    E.code;
    emit(JMP_FALSE, label_else);
    S.code;
    emit(JMP, label_end);
    label_else: (...)
    label_end: (...)
}
```

#### Цикл:
```
WhileStmt → WHILE E DO S END ;
{
    label_start: E.code;
    emit(JMP_FALSE, label_end);
    S.code;
    emit(JMP, label_start);
    label_end: (...)
}
```

#### Ввод-вывод:
```
ReadStmt → READ ID ;      { emit(READ); emit(POP_VAR, id) }
PrintStmt → PRINT E ;     { E.code; emit(PRINT) }
```

---

## 5. ОПЕРАЦИИ ОПС (Обратная Польская Нотация)

### Арифметические операции:
- **OP_ADD**: сложение (a + b)
- **OP_SUB**: вычитание (a - b)
- **OP_MUL**: умножение (a * b)
- **OP_DIV**: деление (a / b)
- **OP_NEG**: унарный минус (-a)

### Операции сравнения:
- **OP_LT**: меньше (<)
- **OP_GT**: больше (>)
- **OP_LE**: меньше или равно (<=)
- **OP_GE**: больше или равно (>=)
- **OP_EQ**: равно (==)
- **OP_NE**: не равно (!=)

### Логические операции:
- **OP_AND**: логическое И (&&)
- **OP_OR**: логическое ИЛИ (||)
- **OP_NOT**: логическое НЕ (!)

### Операции со стеком:
- **OP_PUSH_INT** arg: поместить целое число на стек
- **OP_PUSH_FLOAT** arg: поместить вещественное число на стек
- **OP_PUSH_STRING** arg: поместить строку на стек
- **OP_PUSH_VAR** arg: поместить значение переменной на стек
- **OP_POP_VAR** arg: снять со стека и присвоить переменной

### Управление потоком:
- **OP_JMP** target: безусловный прыжок на инструкцию target
- **OP_JMP_FALSE** target: условный прыжок (если top = 0)
- **OP_LABEL**: метка для прыжков

### Работа с массивами:
- **OP_ARRAY_DECL** name: объявить массив
- **OP_ARRAY_GET** name: получить элемент массива
- **OP_ARRAY_SET** name: установить элемент массива

### Ввод-вывод:
- **OP_READ**: прочитать значение со входа и поместить на стек
- **OP_PRINT**: вывести top стека

### Служебные операции:
- **OP_END**: конец программы
- **OP_NOP**: пустая операция

---

## 6. ФОРМАТ ОПС

Программа представляет собой последовательность инструкций.

### Формат инструкции:

```cpp
struct Instruction {
    OpType op;           // тип операции
    std::string arg;     // строковый аргумент (число, имя переменной)
    int jumpTarget;      // адрес прыжка (для JMP, JMP_FALSE)
};
```

### Пример ОПС для выражения: a = 10 + 3.14 * 2;

```
[0] PUSH_INT 10
[1] PUSH_FLOAT 3.14
[2] PUSH_INT 2
[3] MUL
[4] ADD
[5] POP_VAR a
```

### Пример ОПС для условного оператора:

```
IF x > 5 THEN
    y = 10;
ELSE
    y = 0;
END;
```

```
[0] PUSH_VAR x
[1] PUSH_INT 5
[2] GT
[3] JMP_FALSE 7
[4] PUSH_INT 10
[5] POP_VAR y
[6] JMP 8
[7] PUSH_INT 0
[8] POP_VAR y
```

### Пример ОПС для цикла:

```
WHILE i < 10 DO
    PRINT i;
END;
```

```
[0] PUSH_VAR i
[1] PUSH_INT 10
[2] LT
[3] JMP_FALSE 6
[4] PUSH_VAR i
[5] PRINT
[6] JMP 0
```

---

## 7. ИНТЕРПРЕТАТОР ОПС

Интерпретатор работает по следующему принципу:

1. Инициализирует счетчик команд (pc) в 0
2. Для каждой инструкции:
   - Декодирует операцию
   - Выполняет действие (работа со стеком, переменными, управление потоком)
3. Заканчивает при достижении OP_END

### Стек:
- **Type**: std::vector<Value>
- **Purpose**: хранение промежуточных результатов вычисления выражений

### Таблица переменных (Environment):
- **Type**: std::map<std::string, Value>
- **Purpose**: хранение значений переменных

### Значение (Value):
```cpp
struct Value {
    ValueType type;           // INT, FLOAT, STRING, ARRAY_INT, ARRAY_FLOAT
    double numValue;          // числовое значение
    std::string strValue;     // строковое значение
    std::vector<double> arrayValue;  // массив
};
```

---

## 8. ДИАГНОСТИКА ОШИБОК

### Лексические ошибки:
- T_ERROR генерируется для неизвестных символов
- Сообщение: "Error at line N, column M: Unexpected character 'X'"

### Синтаксические ошибки:
- Генерируются при нарушении грамматики
- Сообщение: "Error at line N, column M: Expected token X but got Y"

### Семантические ошибки (время выполнения):
- Division by zero
- Stack underflow
- Invalid variable access
- Array index out of bounds

---

## 9. ПРИМЕРЫ ПРОГРАММ

### Пример 1: Простая формула

```
READ a;
READ b;
c = a + b * 2;
PRINT c;
```

### Пример 2: Условный оператор

```
READ x;
IF x > 0 THEN
    PRINT x;
ELSE
    PRINT -x;
END;
```

### Пример 3: Цикл

```
i = 0;
WHILE i < 5 DO
    PRINT i;
    i = i + 1;
END;
```

### Пример 4: Сложный пример с массивом

```
READ n;
i = 0;
WHILE i < n DO
    READ arr[i];
    i = i + 1;
END;
```
