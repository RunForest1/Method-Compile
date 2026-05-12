# 4. Семантические действия для генерации ОПС (Таблица генератора)

В данной таблице каждому элементу правой части правила соответствует семантическое действие. Действия выполняются синхронно с работой LL(1)-анализатора.

### Обозначения действий:
*   $\Box$ — пустое действие;
*   $a$ — запись в ОПС адреса переменной (или ссылки на таблицу идентификаторов);
*   $k$ — запись в ОПС константы;
*   $1, 2, 3, 4, 5$ — семантические программы управления метками (P1–P5);
*   $+$, $-$, $*$, $/$ — запись соответствующей бинарной арифметической операции;
*   $=$ — запись операции сравнения (равно, не равно, меньше и т.д.);
*   $-'$ — запись операции унарного минуса;
*   $i, i2$ — операции индексации одномерного и двумерного массива;
*   $:=$ — запись операции присваивания;
*   $r, w$ — операции чтения и вывода.
*   $POW, LOG, EXP, SIN, COS, TANH, CTANH$ — запись соответствующих математических операций;

| Нетерминал | Порождающее правило в нестрогой форме Грейбах | Семантические действия |
|---|---|---|
| `<Program>` | `L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';') <Program>` | a $\Box$ $\Box$ $\Box$ := $\Box$ $\Box$ |
| `<Program>` | `L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger> <Program>` | $\Box$ $\Box$ 1 $\Box$ $\Box$ 3 $\Box$ |
| `<Program>` | `L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger> <Program>` | 4 $\Box$ 1 $\Box$ 5 $\Box$ |
| `<Program>` | `L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';') <Program>` | $\Box$ $\Box$ a $\Box$ r $\Box$ $\Box$ |
| `<Program>` | `L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';') <Program>` | $\Box$ $\Box$ $\Box$ w $\Box$ $\Box$ |
| `<Program>` | `λ` |  |
| `<Program>` | `L_TERMINATOR` | |
| `<Statement>` | `L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')` | a $\Box$ $\Box$ $\Box$ := $\Box$ |
| `<Statement>` | `L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>` | $\Box$ $\Box$ 1  $\Box$ $\Box$ 3 |
| `<Statement>` | `L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>` | 4 $\Box$ 1 $\Box$ 5 |
| `<Statement>` | `L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')` | $\Box$ $\Box$ a $\Box$ r $\Box$ |
| `<Statement>` | `L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')` | $\Box$ $\Box$ $\Box$ w $\Box$ |
| `<Assignment>` | `L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')` | a $\Box$ $\Box$ $\Box$ := $\Box$ |
| `<IfStatement>` | `L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>` | $\Box$ $\Box$ 1 $\Box$ $\Box$ 3 |
| `<ElsePart>` | `L_KEYWORD('else') <Statement>` | 2 $\Box$ |
| `<ElsePart>` | `λ` |  |
| `<WhileStatement>` | `L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>` | 4 $\Box$ 1  $\Box$ 5 |
| `<ReadStatement>` | `L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')` | $\Box$ $\Box$ a $\Box$ r $\Box$ |
| `<WriteStatement>` | `L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')` | $\Box$ $\Box$ $\Box$ w $\Box$ |
| `<Condition>` | `L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ -' $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('power') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ POW $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('log') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ LOG $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('exp') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ EXP $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('sin') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ SIN $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('cos') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ COS $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('tanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ TANH $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_KEYWORD('ctanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ CTANH $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_ID <ArrayIndex> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | a $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_INT <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | k $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_FLOAT <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | k $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Condition>` | `L_STRING <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>` | k $\Box$ $\Box$ $\Box$ $\Box$ = |
| `<Expression>` | `L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ |
| `<Expression>` | `L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ |
| `<Expression>` | `L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ -' $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('power') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ POW $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('log') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ LOG $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('exp') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ EXP $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('sin') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ SIN $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('cos') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ COS $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('tanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ TANH $\Box$ $\Box$ |
| `<Expression>` | `L_KEYWORD('ctanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail> <ExpressionTail>` | $\Box$ $\Box$ $\Box$ $\Box$ CTANH $\Box$ $\Box$ |
| `<Expression>` | `L_ID <ArrayIndex> <TermTail> <ExpressionTail>` | a $\Box$ $\Box$ $\Box$ |
| `<Expression>` | `L_INT <TermTail> <ExpressionTail>` | k $\Box$ $\Box$ |
| `<Expression>` | `L_FLOAT <TermTail> <ExpressionTail>` | k $\Box$ $\Box$ |
| `<Expression>` | `L_STRING <TermTail> <ExpressionTail>` | k $\Box$ $\Box$ |
| `<ExpressionTail>` | `L_ADDITIVE_OPERATOR <Term> <ExpressionTail>` | $\Box$ $\Box$ + |
| `<ExpressionTail>` | `λ` |  |
| `<Term>` | `L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ |
| `<Term>` | `L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail>` | $\Box$ $\Box$ $\Box$ |
| `<Term>` | `L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ -' $\Box$ |
| `<Term>` | `L_KEYWORD('power') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ POW $\Box$ |
| `<Term>` | `L_KEYWORD('log') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ LOG $\Box$ |
| `<Term>` | `L_KEYWORD('exp') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ EXP $\Box$ |
| `<Term>` | `L_KEYWORD('sin') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ SIN $\Box$ |
| `<Term>` | `L_KEYWORD('cos') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ COS $\Box$ |
| `<Term>` | `L_KEYWORD('tanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ TANH $\Box$ |
| `<Term>` | `L_KEYWORD('ctanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> <TermTail>` | $\Box$ $\Box$ $\Box$ $\Box$ CTANH $\Box$ |
| `<Term>` | `L_ID <ArrayIndex> <TermTail>` | a $\Box$ $\Box$ |
| `<Term>` | `L_INT <TermTail>` | k $\Box$ |
| `<Term>` | `L_FLOAT <TermTail>` | k $\Box$ |
| `<Term>` | `L_STRING <TermTail>` | k $\Box$ |
| `<TermTail>` | `L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail>` | $\Box$ $\Box$ * |
| `<TermTail>` | `λ` |  |
| `<Factor>` | `L_DELIMITER('(') <Expression> L_DELIMITER(')')` | $\Box$ $\Box$ $\Box$ |
| `<Factor>` | `L_ADDITIVE_OPERATOR('+') <UnaryOperand>` | $\Box$ $\Box$ |
| `<Factor>` | `L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger>` | $\Box$ $\Box$ -' |
| `<Factor>` | `L_KEYWORD('power') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ POW |
| `<Factor>` | `L_KEYWORD('log') L_DELIMITER('(') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ LOG |
| `<Factor>` | `L_KEYWORD('exp') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ EXP |
| `<Factor>` | `L_KEYWORD('sin') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ SIN |
| `<Factor>` | `L_KEYWORD('cos') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ COS |
| `<Factor>` | `L_KEYWORD('tanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ TANH |
| `<Factor>` | `L_KEYWORD('ctanh') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger>` | $\Box$ $\Box$ $\Box$ $\Box$ CTANH |
| `<Factor>` | `L_ID <ArrayIndex>` | a $\Box$ |
| `<Factor>` | `L_INT` | k |
| `<Factor>` | `L_FLOAT` | k |
| `<Factor>` | `L_STRING` | k |
| `<UnaryOperand>` | `L_DELIMITER('(') <Expression> L_DELIMITER(')')` | $\Box$ $\Box$ $\Box$ |
| `<UnaryOperand>` | `L_ID <ArrayIndex>` | a $\Box$ |
| `<UnaryOperand>` | `L_INT` | k |
| `<UnaryOperand>` | `L_FLOAT` | k |
| `<ArrayIndex>` | `L_DELIMITER('[') <Expression> <ArrayTail>` | $\Box$ $\Box$ $\Box$ |
| `<ArrayTail>` | `L_DELIMITER(']')` | i |
| `<ArrayTail>` | `L_DELIMITER(',') <Expression> L_DELIMITER(']')` | $\Box$ $\Box$ i2 |
| `<ArrayIndex>` | `λ` | |
| `<SemanticTrigger>` | `λ` | |

---

## Семантические программы
Семантические программы используют счетчик k – номер очередного генерируемого элемента ОПС , а также еще один магазин – магазин меток.

Программа 1. 

1. В магазин меток записывается k. 
2. В ОПС записывается пустой элемент – место для будущей метки.
3. В ОПС записывается операция jf – переход при условии false.

Программа 2. 

1. Через верхний элемент магазина меток, как ссылку на ранее заготовленное место для метки, записывается k + 2. 
2. В магазин меток записывается k. 
3. В ОПС записывается пустой элемент – место для будущей метки.
4. В ОПС записывается операция j – безусловный переход. 5

Программа 3. 

1. Через верхний элемент магазина меток, как ссылку на ранее заготовленное место для метки, записывается k. 

Программа 4.

1. В магазин меток записывается k. 

Программа 5. 

1. Через верхний элемент магазина меток, как ссылку на ранее заготовленное место для метки, записывается k + 2. 
2. В ОПС записывается метка, значение для которой читается из магазина меток.
3. В ОПС записывается операция j – безусловный переход.

# 5. Список операций ОПС

| Мнемоника | Тип | Описание |
| :--- | :--- | :--- |
| `+` | Бинарная | Сложение двух верхних элементов стека |
| `-` | Бинарная | Вычитание (верхний из второго сверху) |
| `*` | Бинарная | Умножение |
| `/` | Бинарная | Деление |
| `NEG` | Унарная | Смена знака верхнего элемента |
| `==` | Сравнение | Проверка на равенство |
| `!=` | Сравнение | Проверка на неравенство |
| `<` | Сравнение | Меньше |
| `>` | Сравнение | Больше |
| `<=` | Сравнение | Меньше или равно |
| `>=` | Сравнение | Больше или равно |
| `:=` | Присваивание | Запись значения (верхний) по адресу (второй сверху) |
| `J` | Переход | Безусловный переход по адресу в стеке |
| `JF` | Переход | Переход по адресу (2-й сверху), если флаг (1-й сверху) ложен |
| `READ` | Ввод/Вывод | Чтение значения в переменную (адрес в стеке) |
| `WRITE` | Ввод/Вывод | Печать значения (верхний элемент стека) |
| `INDEX1` | Массивы | Вычисление адреса элемента `A[i]` |
| `INDEX2` | Массивы | Вычисление адреса элемента `A[i, j]` |
| `POW` | Бинарная | Возведение второго сверху элемента в степень верхнего |
| `LOG` | Бинарная | Логарифм второго сверху элемента по основанию верхнего |
| `EXP` | Унарная | Экспонента (e^x) |
| `SIN` | Унарная | Синус |
| `COS` | Унарная | Косинус |
| `TANH` | Унарная | Гиперболический тангенс |
| `CTANH` | Унарная | Гиперболический котангенс |

---

# 6. Формат ОПС

ОПС представляет собой одномерный массив (вектор) элементов. Каждый элемент — это структура, содержащая тип и значение.

### Типы элементов ОПС:
1.  **ADDR_VAR:** Указатель на запись в таблице идентификаторов. При интерпретации в стек кладется именно адрес (ссылка).
2.  **CONST_VAL:** Непосредственное значение (целое, вещественное или строка).
3.  **OPERATOR:** Код операции из таблицы в разделе 5.
4.  **LABEL:** Целое число — индекс (адрес) элемента в массиве ОПС. Используется как операнд для команд `J` и `JF`.

