# 2. КС-грамматика языка

В данной грамматике терминалами являются лексемы, определенные в лексическом анализаторе.

**Нетерминалы:**
* `<Program>` — корень программы (последовательность операторов).
* `<Statement>` — отдельный оператор (присваивание, условие, цикл, ввод/вывод).
* `<Expression>` — арифметическое выражение (низший приоритет: +, -).
* `<ExpressionTail>` — хвост выражения для устранения левой рекурсии.
* `<Term>` — слагаемое в выражении (высший приоритет: *, /).
* `<TermTail>` — хвост слагаемого.
* `<Factor>` — элементарный множитель (число, переменная, скобки, унарный минус).
* `<UnaryOperand>` — операнд для унарной операции (соответствует `G` в примерах).
* `<Condition>` — логическое выражение для `if` и `while`.
* `<ComparisonPart>` — вторая часть операции сравнения.
* `<ArrayIndex>` — индекс массива (поддержка 1D и 2D).
* `<IndexTail>` — завершение индексации (различие между `[x]` и `[x, y]`).
* `<ElsePart>` — необязательная ветвь `else`.
* `<Epsilon>` — пустая цепочка (λ) для завершения правил.

**Порождающие правила (стандартная форма):**

```
<Program> ::= <Statement> <Program> | λ | L_TERMINATOR

<Statement> ::= <Assignment>
              | <IfStatement>
              | <WhileStatement>
              | <ReadStatement>
              | <WriteStatement>

<Assignment> ::= L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> L_DELIMITER(';')

<IfStatement> ::= L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart>

<ElsePart> ::= L_KEYWORD('else') <Statement> | λ

<WhileStatement> ::= L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement>

<ReadStatement> ::= L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')

<WriteStatement> ::= L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<Condition> ::= <Expression> L_COMPARISON_OPERATOR <Expression>

<Expression> ::= <Expression> L_ADDITIVE_OPERATOR <Term> | <Term>

<Term> ::= <Term> L_MULTIPLICATIVE_OPERATOR <Factor> | <Factor>

<Factor> ::= L_DELIMITER('(') <Expression> L_DELIMITER(')')
           | L_ADDITIVE_OPERATOR <UnaryOperand>
           | L_ID <ArrayIndex>
           | L_INT
           | L_FLOAT
           | L_STRING

<UnaryOperand> ::= L_DELIMITER('(') <Expression> L_DELIMITER(')')
                | L_ID <ArrayIndex>
                | L_INT
                | L_FLOAT

<ArrayIndex> ::= L_DELIMITER('[') <Expression> <IndexTail> | λ

<IndexTail> ::= L_DELIMITER(',') <Expression> L_DELIMITER(']') | L_DELIMITER(']')
```

# 3. КС-грамматика в нестрогой нормальной форме Грейбах

Для обеспечения возможности LL(1)-анализа грамматика преобразована путем устранения левой рекурсии и подстановки нетерминалов в начало правил.

```
<Program> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <Epsilon> <Program>
           | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <Program>
           | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <Program>
           | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') <Program>
           | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') <Program>
           | λ

<Statement> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <Epsilon>
             | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart>
             | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement>
             | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')')
             | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')')

<Expression> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail>
              | L_ID <ArrayIndex> <TermTail> <ExpressionTail>
              | L_INT <TermTail> <ExpressionTail>
              | L_FLOAT <TermTail> <ExpressionTail>
              | L_STRING <TermTail> <ExpressionTail>
              | L_ADDITIVE_OPERATOR <UnaryOperand> <TermTail> <ExpressionTail>

<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail> | λ

<Term> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail>
        | L_ID <ArrayIndex> <TermTail>
        | L_INT <TermTail>
        | L_FLOAT <TermTail>
        | L_STRING <TermTail>
        | L_ADDITIVE_OPERATOR <UnaryOperand> <TermTail>

<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
          | L_ID <ArrayIndex>
          | L_INT
          | L_FLOAT
          | L_STRING
          | L_ADDITIVE_OPERATOR <UnaryOperand>

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
               | L_ID <ArrayIndex>
               | L_INT
               | L_FLOAT

<Condition> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail> <ComparisonPart>
             | L_ID <ArrayIndex> <TermTail> <ExpressionTail> <ComparisonPart>
             | L_INT <TermTail> <ExpressionTail> <ComparisonPart>
             | L_FLOAT <TermTail> <ExpressionTail> <ComparisonPart>
             | L_STRING <TermTail> <ExpressionTail> <ComparisonPart>
             | L_ADDITIVE_OPERATOR <UnaryOperand> <TermTail> <ExpressionTail> <ComparisonPart>

<ComparisonPart> -> L_COMPARISON_OPERATOR <Expression>

<ArrayIndex> -> L_DELIMITER('[') <Expression> <IndexTail> | λ

<IndexTail> -> L_DELIMITER(']') | L_DELIMITER(',') <Expression> L_DELIMITER(']')

<ElsePart> -> L_KEYWORD('else') <Statement> | λ

<Epsilon> -> λ
```

# 4. Семантические действия для генерации ОПС

Генерация ОПС происходит синхронно с работой магазинного автомата. Каждому символу правой части правила соответствует семантическое действие.

**Обозначения действий:**
* $\Box$ — пустое действие (ничего не записывается в ОПС).
* `a` — запись в ОПС операнда (ссылка на переменную `L_ID` или значение константы `L_INT`, `L_FLOAT`, `L_STRING`).
* `+`, `-`, `*`, `/` — запись соответствующей бинарной операции.
* `-'` — запись унарного минуса.
* `cmp` — запись операции сравнения (берется из текущей лексемы `L_COMPARISON_OPERATOR`).
* `:=` — запись операции присваивания.
* `i` — операция индексации 1D массива.
* `i2` — операция индексации 2D массива.
* `r` — операция чтения (`read`).
* `w` — операция записи (`write`).
* `1, 2, 3, 4, 5` — семантические программы для работы с метками переходов (см. ниже).

### Таблица семантических действий

| Нетерминал | Правило | Семантические действия |
| :--- | :--- | :--- |
| **Statement** | `L_ID ArrayIndex L_ASSIGNMENT_OPERATOR Expression L_DELIM(';')` | a $\Box$ $\Box$ $\Box$ := |
| | `L_KEYWORD('if') Condition L_KEYWORD('then') Statement ElsePart` | $\Box$ $\Box$ 1 $\Box$ 3 |
| | `L_KEYWORD('while') Condition L_KEYWORD('do') Statement` | 4 $\Box$ 1 $\Box$ 5 |
| | `L_KEYWORD('read') L_DELIMITER('(') L_ID ArrayIndex L_DELIMITER(')') L_DELIM(';')` | $\Box$ $\Box$ a $\Box$ r $\Box$ |
| | `L_KEYWORD('write') L_DELIMITER('(') Expression L_DELIMITER(')') L_DELIM(';')` | $\Box$ $\Box$ $\Box$ w $\Box$ |
| **Expression** | `L_ID ArrayIndex TermTail ExpressionTail` | a $\Box$ $\Box$ $\Box$ |
| | `L_INT/FLOAT/STR TermTail ExpressionTail` | a $\Box$ $\Box$ |
| | `L_ADD_OP UnaryOperand TermTail ExpressionTail` | $\Box$ [$\Box$/-'] $\Box$ $\Box$ |
| **ExpressionTail**| `L_ADD_OP Term ExpressionTail` | $\Box$ $\Box$ [+/-] |
| **TermTail** | `L_MULT_OP Factor TermTail` | $\Box$ $\Box$ [*/] |
| **ComparisonPart**| `L_COMP_OP Expression` | $\Box$ cmp |
| **ArrayIndex** | `L_DELIMITER('[') Expression IndexTail` | $\Box$ $\Box$ $\Box$ |
| **IndexTail** | `L_DELIMITER(']')` | i |
| | `L_DELIMITER(',') Expression L_DELIMITER(']')` | $\Box$ $\Box$ i2 |
| **ElsePart** | `L_KEYWORD('else') Statement` | 2 $\Box$ |

### Семантические программы управления метками (Магазин меток)

1. **Программа 1 (Условный переход):**
   * Записать текущий индекс ОПС `k` в магазин меток.
   * Записать в ОПС пустой элемент (резерв под метку).
   * Записать в ОПС операцию `jf` (переход если ложь).

2. **Программа 2 (Безусловный переход в else):**
   * Взять адрес из магазина меток (от `if`) и записать туда `k + 2`.
   * Записать в магазин меток текущий `k`.
   * Записать в ОПС пустой элемент (резерв под метку).
   * Записать в ОПС операцию `j` (безусловный переход).

3. **Программа 3 (Фиксация метки выхода if/else):**
   * Взять адрес из магазина меток и записать туда текущий `k`.

4. **Программа 4 (Начало цикла):**
   * Записать текущий `k` в магазин меток (адрес начала проверки условия).

5. **Program 5 (Конец цикла):**
   * Взять адрес из магазина меток (от `jf`) и записать туда `k + 2`.
   * Записать в ОПС адрес начала условия (извлечь из магазина меток).
   * Записать в ОПС операцию `j`.

# 5. Список операций ОПС

* **Арифметические:**
  * `+`, `-` — бинарное сложение и вычитание.
  * `*`, `/` — бинарное умножение и деление.
  * `-'` — унарный минус.
* **Сравнения:**
  * `==`, `!=`, `<`, `>`, `<=`, `>=` — логические сравнения.
* **Управление потоком:**
  * `j` — безусловный переход к элементу ОПС по адресу.
  * `jf` — переход по адресу, если значение на вершине стека ложно.
* **Массивы:**
  * `i` — индексация одномерного массива (берет адрес начала и 1 индекс).
  * `i2` — индексация двумерного массива (берет адрес начала и 2 индекса).
* **Системные:**
  * `:=` — присваивание значения по адресу.
  * `r` — чтение значения в переменную.
  * `w` — вывод значения на экран.

# 6. Формат ОПС

ОПС представляет собой линейный массив элементов, где каждый элемент — это структура или объект, содержащий:
1. **Тип элемента:**
   * `OPERAND_ADDR` — адрес переменной в таблице символов.
   * `OPERAND_CONST` — константное значение (int, float или string).
   * `OPERATION` — код операции (см. список выше).
   * `LABEL` — индекс элемента в массиве ОПС для переходов.
2. **Значение:** (индекс, число или строка в зависимости от типа).

При выполнении используется стек интерпретатора: операнды помещаются в стек, операции извлекают необходимое количество операндов и помещают результат обратно.

# 7. Таблица переходов LL(1)-анализатора и генератора ОПС

В данной таблице в ячейках указаны правила вывода и соответствующие им семантические действия (под правилом).
Символ `λ` означает пустую цепочку. Согласно правилу: если нетерминал nullable, то во все свободные клетки строки записывается `λ`.

**Терминалы (колонки):**
`ID` (L_ID), `Const` (L_INT/FLT/STR), `Add` (+/-), `Mult` (*/), `Cmp` (сравнение), `Assign` (:=), `(` , `)` , `[` , `]` , `,` , `;` , `if`, `then`, `else`, `while`, `do`, `read`, `write`, `┴` (L_TERMINATOR).

| Нетерминал | ID | Const | Add | Mult | Cmp | Assign | ( | ) | [ | ] | , | ; | if | then | else | while | do | read | write | ┴ |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Program** | `Stat Progr` <br> $\Box$ $\Box$ | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `Stat Progr` <br> $\Box$ $\Box$ | `λ` | `λ` | `Stat Progr` <br> $\Box$ $\Box$ | `λ` | `Stat Progr` <br> $\Box$ $\Box$ | `Stat Progr` <br> $\Box$ $\Box$ | `λ` |
| **Statement** | `ID ArrInd := Expr ;` <br> a $\Box$ $\Box$ $\Box$ := | | | | | | | | | | | | `if Cond then Stat Else` <br> $\Box$ $\Box$ 1 $\Box$ 3 | | | `while Cond do Stat` <br> 4 $\Box$ 1 $\Box$ 5 | | `read ( ID ArrInd ) ;` <br> $\Box$ $\Box$ a $\Box$ r $\Box$ | `write ( Expr ) ;` <br> $\Box$ $\Box$ $\Box$ w $\Box$ | |
| **Expression** | `ID ArrInd TTail ETail` <br> a $\Box$ $\Box$ $\Box$ | `Const TTail ETail` <br> a $\Box$ $\Box$ | `Add UOperand TTail ETail` <br> $\Box$ [$\Box$/-'] $\Box$ $\Box$ | | | | `( Expr ) TTail ETail` <br> $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ | | | | | | | | | | | | | |
| **ExpressionTail**| `λ` | `λ` | `Add Term ETail` <br> $\Box$ $\Box$ [+/-] | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **Term** | `ID ArrInd TTail` <br> a $\Box$ $\Box$ | `Const TTail` <br> a $\Box$ | `Add UOperand TTail` <br> $\Box$ [$\Box$/-'] $\Box$ | | | | `( Expr ) TTail` <br> $\Box$ $\Box$ $\Box$ $\Box$ | | | | | | | | | | | | | |
| **TermTail** | `λ` | `λ` | `λ` | `Mult Fact TTail` <br> $\Box$ $\Box$ [*/] | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **Factor** | `ID ArrInd` <br> a $\Box$ | `Const` <br> a | `Add UOperand` <br> $\Box$ [$\Box$/-'] | | | | `( Expr )` <br> $\Box$ $\Box$ $\Box$ | | | | | | | | | | | | | |
| **UnaryOperand**| `ID ArrInd` <br> a $\Box$ | `Const` <br> a | | | | | `( Expr )` <br> $\Box$ $\Box$ $\Box$ | | | | | | | | | | | | | |
| **Condition** | `ID ArrInd TTail ETail CmpPart` <br> a $\Box$ $\Box$ $\Box$ $\Box$ | `Const TTail ETail CmpPart` <br> a $\Box$ $\Box$ $\Box$ | `Add UOperand TTail ETail CmpPart` <br> $\Box$ [$\Box$/-'] $\Box$ $\Box$ $\Box$ | | | | `( Expr ) TTail ETail CmpPart` <br> $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ $\Box$ | | | | | | | | | | | | | |
| **ComparisonPart**| | | | | `Cmp Expr` <br> $\Box$ $\Box$ cmp | | | | | | | | | | | | | | | |
| **ArrayIndex** | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `[ Expr ITail` <br> $\Box$ $\Box$ $\Box$ | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **IndexTail** | | | | | | | | | | `]` <br> i | `, Expr ]` <br> $\Box$ $\Box$ i2 | | | | | | | | | |
| **ElsePart** | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `else Stat` <br> 2 $\Box$ | `λ` | `λ` | `λ` | `λ` | `λ` |
