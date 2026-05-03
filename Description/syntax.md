## 2. КС-грамматика языка

На данном этапе лексемы, выделенные лексическим анализатором, рассматриваются как терминальные символы (терминалы). Грамматика описывает синтаксическую структуру языка.

### 2.1. Список терминалов (лексем)
В качестве терминалов используются классы лексем из лексического анализатора и конкретные ключевые слова/разделители:
- **L_ID** — идентификаторы (имена переменных и массивов).
- **L_INT** — целые числа.
- **L_FLOAT** — вещественные числа.
- **L_STRING** — символьные строки.
- **L_ADDITIVE_OPERATOR** — операторы `+`, `-`.
- **L_MULTIPLICATIVE_OPERATOR** — операторы `*`, `/`.
- **L_COMPARISON_OPERATOR** — операторы `<`, `>`, `==`, `<=`, `>=`, `!=`.
- **L_ASSIGNMENT_OPERATOR** — оператор присваивания `:=`.
- **L_KEYWORD** — служебные слова языка (`if`, `then`, `else`, `while`, `do`, `read`, `write`, `int`, `float`). В правилах грамматики они для наглядности записываются в виде конкретных строк.
- **L_DELIMITER** — разделители (`(`, `)`, `[`, `]`, `;`, `,`). В правилах для наглядности используются конкретные символы.

### 2.2. Нетерминальные символы
- `<Program>` — программа.
- `<StmtList>` — список операторов.
- `<Stmt>` — отдельный оператор.
- `<AssignStmt>` — оператор присваивания.
- `<IfStmt>` — условный оператор.
- `<WhileStmt>` — оператор цикла.
- `<ReadStmt>` — оператор ввода.
- `<WriteStmt>` — оператор вывода.
- `<Variable>` — переменная или элемент массива (L-value).
- `<Expr>` — арифметическое выражение.
- `<Term>` — терм (для обработки приоритета умножения/деления).
- `<Factor>` — элементарный множитель выражения.
- `<Condition>` — логическое условие.

### 2.3. Правила грамматики

**1. Структура программы:**
- `<Program> → <StmtList>`
- `<StmtList> → <Stmt> | <Stmt> ; <StmtList>`

**2. Операторы:**
- `<Stmt> → <AssignStmt> | <IfStmt> | <WhileStmt> | <ReadStmt> | <WriteStmt>`

**3. Переменные и массивы:**
- `<Variable> → L_ID`
- `<Variable> → L_ID [ <Expr> ]`
- `<Variable> → L_ID [ <Expr> , <Expr> ]`

**4. Выражения (с учетом приоритетов):**
- `<AssignStmt> → <Variable> L_ASSIGNMENT_OPERATOR <Expr>`
- `<Expr> → <Term> | <Expr> L_ADDITIVE_OPERATOR <Term>`
- `<Term> → <Factor> | <Term> L_MULTIPLICATIVE_OPERATOR <Factor>`
- `<Factor> → ( <Expr> ) | <Variable> | L_INT | L_FLOAT`

**5. Условные конструкции и циклы:**
- `<Condition> → <Expr> L_COMPARISON_OPERATOR <Expr>`
- `<IfStmt> → if <Condition> then <Stmt>`
- `<IfStmt> → if <Condition> then <Stmt> else <Stmt>`
- `<WhileStmt> → while <Condition> do <Stmt>`

**6. Ввод-вывод:**
- `<ReadStmt> → read ( <Variable> )`
- `<WriteStmt> → write ( <WriteArg> )`
- `<WriteArg> → <Expr> | L_STRING`

## 3. КС-грамматика в нестрогой нормальной форме Грейбах

Преобразование исходной грамматики в нестрогую нормальную форму Грейбах (НФГ) выполняется в три основных этапа:
1. Устранение левой рекурсии.
2. Подстановка правил для того, чтобы каждая правая часть начиналась с терминала или была пустой ($\lambda$).
3. Левая факторизация (устранение одинаковых префиксов в альтернативах для однозначного разбора).

---

### Шаг 1. Устранение левой рекурсии

#### 1. Список операторов `<StmtList>`
* **Исходные правила**: 
  `<StmtList> → <Stmt> | <Stmt> ; <StmtList>`
* **Преобразование**:
  `<StmtList> → <Stmt> <StmtTail>`
  `<StmtTail> → ; <StmtList> | λ`

#### 2. Арифметические выражения `<Expr>`
* **Исходные правила**:
  `<Expr> → <Term> | <Expr> L_ADDITIVE_OPERATOR <Term>`
* **Преобразование**:
  `<Expr> → <Term> <ExprTail>`
  `<ExprTail> → L_ADDITIVE_OPERATOR <Term> <ExprTail> | λ`

#### 3. Термы `<Term>`
* **Исходные правила**:
  `<Term> → <Factor> | <Term> L_MULTIPLICATIVE_OPERATOR <Factor>`
* **Преобразование**:
  `<Term> → <Factor> <TermTail>`
  `<TermTail> → L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ`

---

### Шаг 2. Приведение к нестрогой НФ Грейбах

#### 1. Переменные `<Variable>`
* `<Variable> → L_ID <VarExt>`
* `<VarExt> → [ <Expr> <IndexTail> ] | λ`
* `<IndexTail> → , <Expr> | λ`

#### 2. Факторы (множители) `<Factor>`
* `<Factor> → ( <Expr> ) | L_ID <VarExt> | L_INT | L_FLOAT`

#### 3. Термы `<Term>`
* `<Term> → ( <Expr> ) <TermTail>`
* `<Term> → L_ID <VarExt> <TermTail>`
* `<Term> → L_INT <TermTail>`
* `<Term> → L_FLOAT <TermTail>`

#### 4. Выражения `<Expr>`
* `<Expr> → ( <Expr> ) <TermTail> <ExprTail>`
* `<Expr> → L_ID <VarExt> <TermTail> <ExprTail>`
* `<Expr> → L_INT <TermTail> <ExprTail>`
* `<Expr> → L_FLOAT <TermTail> <ExprTail>`

#### 5. Оператор присваивания `<AssignStmt>`
* `<AssignStmt> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr>`

#### 6. Другие базовые операторы
* `<IfStmt> → if <Condition> then <Stmt> <IfTail>`
* `<IfTail> → else <Stmt> | λ`
* `<WhileStmt> → while <Condition> do <Stmt>`
* `<ReadStmt> → read ( <Variable> )`
* `<WriteStmt> → write ( <WriteArg> )`
* `<WriteArg> → ( <Expr> ) <TermTail> <ExprTail> | L_ID <VarExt> <TermTail> <ExprTail> | L_INT <TermTail> <ExprTail> | L_FLOAT <TermTail> <ExprTail> | L_STRING`

#### 7. Обобщенный оператор `<Stmt>`
* `<Stmt> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr>`
* `<Stmt> → if <Condition> then <Stmt> <IfTail>`
* `<Stmt> → while <Condition> do <Stmt>`
* `<Stmt> → read ( <Variable> )`
* `<Stmt> → write ( <WriteArg> )`

#### 8. Список операторов `<StmtList>`
* `<StmtList> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr> <StmtTail>`
* `<StmtList> → if <Condition> then <Stmt> <IfTail> <StmtTail>`
* `<StmtList> → while <Condition> do <Stmt> <StmtTail>`
* `<StmtList> → read ( <Variable> ) <StmtTail>`
* `<StmtList> → write ( <WriteArg> ) <StmtTail>`

---

## 3. КС-грамматика в нестрогой нормальной форме Грейбах

Преобразование исходной грамматики в нестрогую нормальную форму Грейбах (НФГ) выполняется в три основных этапа:
1. Устранение левой рекурсии.
2. Подстановка правил для того, чтобы каждая правая часть начиналась с терминала или была пустой ($\lambda$).
3. Левая факторизация (устранение одинаковых префиксов в альтернативах для однозначного разбора).

---

### Шаг 1. Устранение левой рекурсии

#### 1. Список операторов `<StmtList>`
* **Исходные правила**: 
  `<StmtList> → <Stmt> | <Stmt> ; <StmtList>`
* **Преобразование**:
  `<StmtList> → <Stmt> <StmtTail>`
  `<StmtTail> → ; <StmtList> | λ`

#### 2. Арифметические выражения `<Expr>`
* **Исходные правила**:
  `<Expr> → <Term> | <Expr> L_ADDITIVE_OPERATOR <Term>`
* **Преобразование**:
  `<Expr> → <Term> <ExprTail>`
  `<ExprTail> → L_ADDITIVE_OPERATOR <Term> <ExprTail> | λ`

#### 3. Термы `<Term>`
* **Исходные правила**:
  `<Term> → <Factor> | <Term> L_MULTIPLICATIVE_OPERATOR <Factor>`
* **Преобразование**:
  `<Term> → <Factor> <TermTail>`
  `<TermTail> → L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ`

---

### Шаг 2. Приведение к нестрогой НФ Грейбах

#### 1. Переменные `<Variable>`
* `<Variable> → L_ID <VarExt>`
* `<VarExt> → [ <Expr> <IndexTail> ] | λ`
* `<IndexTail> → , <Expr> | λ`

#### 2. Факторы (множители) `<Factor>`
* `<Factor> → ( <Expr> ) | L_ID <VarExt> | L_INT | L_FLOAT`

#### 3. Термы `<Term>`
* `<Term> → ( <Expr> ) <TermTail>`
* `<Term> → L_ID <VarExt> <TermTail>`
* `<Term> → L_INT <TermTail>`
* `<Term> → L_FLOAT <TermTail>`

#### 4. Выражения `<Expr>`
* `<Expr> → ( <Expr> ) <TermTail> <ExprTail>`
* `<Expr> → L_ID <VarExt> <TermTail> <ExprTail>`
* `<Expr> → L_INT <TermTail> <ExprTail>`
* `<Expr> → L_FLOAT <TermTail> <ExprTail>`

#### 5. Оператор присваивания `<AssignStmt>`
* `<AssignStmt> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr>`

#### 6. Другие базовые операторы
* `<IfStmt> → if <Condition> then <Stmt> <IfTail>`
* `<IfTail> → else <Stmt> | λ`
* `<WhileStmt> → while <Condition> do <Stmt>`
* `<ReadStmt> → read ( <Variable> )`
* `<WriteStmt> → write ( <WriteArg> )`
* `<WriteArg> → ( <Expr> ) <TermTail> <ExprTail> | L_ID <VarExt> <TermTail> <ExprTail> | L_INT <TermTail> <ExprTail> | L_FLOAT <TermTail> <ExprTail> | L_STRING`

#### 7. Обобщенный оператор `<Stmt>`
* `<Stmt> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr>`
* `<Stmt> → if <Condition> then <Stmt> <IfTail>`
* `<Stmt> → while <Condition> do <Stmt>`
* `<Stmt> → read ( <Variable> )`
* `<Stmt> → write ( <WriteArg> )`

#### 8. Список операторов `<StmtList>`
* `<StmtList> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr> <StmtTail>`
* `<StmtList> → if <Condition> then <Stmt> <IfTail> <StmtTail>`
* `<StmtList> → while <Condition> do <Stmt> <StmtTail>`
* `<StmtList> → read ( <Variable> ) <StmtTail>`
* `<StmtList> → write ( <WriteArg> ) <StmtTail>`

---

### Шаг 3. Факторизация правил (если необходимо)

Факторизация нужна там, где несколько альтернатив одного нетерминала начинаются с одинаковых терминалов, что вызывает неоднозначность при разборе сверху вниз.

В нашей грамматике такие ситуации возникают в `<Expr>`, `<Factor>`, `<Term>` и `<WriteArg>`, так как все они могут начинаться с `L_ID`, `(`, `L_INT`, `L_FLOAT`. Однако благодаря тому, что мы уже строго разделили выражения на `<TermTail>` и `<ExprTail>`, а также ввели `<VarExt>`, данные правила факторизованы на уровне своих цепочек.

Единственное место, требующее явной левой факторизации — это `<WriteArg>` и `<Expr>`, так как они имеют одинаковые префиксы. Но в рамках НФГ они уже имеют уникальные цепочки вывода для каждого начального терминала:

---

### Итоговая КС-грамматика в нестрогой НФГ

**Терминалы (лексемы):** `L_ID`, `L_INT`, `L_FLOAT`, `L_STRING`, `L_ADDITIVE_OPERATOR`, `L_MULTIPLICATIVE_OPERATOR`, `L_COMPARISON_OPERATOR`, `L_ASSIGNMENT_OPERATOR`, `if`, `then`, `else`, `while`, `do`, `read`, `write`, `(`, `)`, `[`, `]`, `;`, `,`.

**Нетерминалы:** `<Program>`, `<StmtList>`, `<StmtTail>`, `<Stmt>`, `<IfTail>`, `<Variable>`, `<VarExt>`, `<IndexTail>`, `<Expr>`, `<ExprTail>`, `<TermTail>`, `<Condition>`, `<WriteArg>`.

**Правила вывода:**

`<Program> → <StmtList>`

`<StmtList> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr> <StmtTail> | if <Condition> then <Stmt> <IfTail> <StmtTail> | while <Condition> do <Stmt> <StmtTail> | read ( <Variable> ) <StmtTail> | write ( <WriteArg> ) <StmtTail>`

`<StmtTail> → ; <StmtList> | λ`

`<Stmt> → L_ID <VarExt> L_ASSIGNMENT_OPERATOR <Expr> | if <Condition> then <Stmt> <IfTail> | while <Condition> do <Stmt> | read ( <Variable> ) | write ( <WriteArg> )`

`<IfTail> → else <Stmt> | λ`

`<Variable> → L_ID <VarExt>`

`<VarExt> → [ <Expr> <IndexTail> ] | λ`

`<IndexTail> → , <Expr> | λ`

`<Expr> → ( <Expr> ) <TermTail> <ExprTail> | L_ID <VarExt> <TermTail> <ExprTail> | L_INT <TermTail> <ExprTail> | L_FLOAT <TermTail> <ExprTail>`

`<ExprTail> → L_ADDITIVE_OPERATOR <Term> <ExprTail> | λ`

`<TermTail> → L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ`

`<Factor> → ( <Expr> ) | L_ID <VarExt> | L_INT | L_FLOAT`

`<Condition> → <Expr> L_COMPARISON_OPERATOR <Expr>`

`<WriteArg> → ( <Expr> ) <TermTail> <ExprTail> | L_ID <VarExt> <TermTail> <ExprTail> | L_INT <TermTail> <ExprTail> | L_FLOAT <TermTail> <ExprTail> | L_STRING`

## 4. Семантические действия для генерации ОПС

Для генерации обратной польской записи (ОПС) в процессе синтаксического анализа по грамматике в нестрогой НФГ к правилам вывода добавляются семантические действия. Семантическое действие обозначается как `{ Действие }` и выполняется в момент его извлечения из стека (или при сопоставлении в процессе разбора).

В процессе разбора используются два основных стека:
- **Стек ОПС (`OPS_Stack`)** — хранит результирующую выходную строку ОПС.
- **Стек операций (`Op_Stack`)** — используется для временного хранения знаков операций при разборе выражений.

### 4.1. Обозначения семантических действий
- `{ PUSH_ID }` — занести имя текущего идентификатора `L_ID` в ОПС.
- `{ PUSH_CONST }` — занести значение константы (`L_INT`, `L_FLOAT`, `L_STRING`) в ОПС.
- `{ PUSH_OP(op) }` — положить оператор в стек операций.
- `{ POP_OP }` — извлечь оператор из стека операций и дописать его в ОПС.
- `{ GEN_LABEL }` — сгенерировать новую уникальную метку для переходов (`М1`, `М2`, ...).
- `{ PUSH_LABEL(label) }` — занести метку в ОПС.
- `{ PLACE_LABEL(label) }` — зафиксировать положение метки в результирующей строке ОПС.
- `{ GEN_ARRAY_REF }` — генерация обращения к элементу массива. Принимает количество размерностей. В ОПС записывается специальный оператор индексации.

---

### 4.2. Правила грамматики с семантическими действиями

#### 1. Выражения и термы
* `<Expr> → ( <Expr> ) <TermTail> <ExprTail>`
* `<Expr> → L_ID { PUSH_ID } <VarExt> <TermTail> <ExprTail>`
* `<Expr> → L_INT { PUSH_CONST } <TermTail> <ExprTail>`
* `<Expr> → L_FLOAT { PUSH_CONST } <TermTail> <ExprTail>`

* `<ExprTail> → L_ADDITIVE_OPERATOR { PUSH_OP } <Term> { POP_OP } <ExprTail> | λ`
* `<TermTail> → L_MULTIPLICATIVE_OPERATOR { PUSH_OP } <Factor> { POP_OP } <TermTail> | λ`

#### 2. Массивы (Обработка индексов)
* `<VarExt> → [ <Expr> <IndexTail> ] { GEN_ARRAY_REF } | λ`
* `<IndexTail> → , <Expr> { SET_DIM_2 } | λ` *(по умолчанию 1-мерный массив, при наличии запятой фиксируется 2-мерный)*

#### 3. Оператор присваивания
* `<AssignStmt> → L_ID { PUSH_ID } <VarExt> L_ASSIGNMENT_OPERATOR <Expr> { WRITE_OP(':=') }`

#### 4. Условные операторы и циклы

**Условный оператор (`if-then-else`):**
* `<IfStmt> → if <Condition> { GEN_LABEL(L1), PUSH_LABEL(L1), WRITE_OP('УПХ') } then <Stmt> <IfTail>`
* `<IfTail> → else { GEN_LABEL(L2), PUSH_LABEL(L2), WRITE_OP('БП'), PLACE_LABEL(L1) } <Stmt> { PLACE_LABEL(2) } | λ { PLACE_LABEL(L1) }`

**Оператор цикла (`while`):**
* `<WhileStmt> → while { GEN_LABEL(L1), PLACE_LABEL(L1) } <Condition> { GEN_LABEL(L2), PUSH_LABEL(L2), WRITE_OP('УПХ') } do <Stmt> { PUSH_LABEL(L1), WRITE_OP('БП'), PLACE_LABEL(L2) }`

#### 5. Ввод-вывод
* `<ReadStmt> → read ( L_ID { PUSH_ID } <VarExt> ) { WRITE_OP('READ') }`
* `<WriteStmt> → write ( <WriteArg> ) { WRITE_OP('WRITE') }`
* `<WriteArg> → <Expr> | L_STRING { PUSH_CONST }`

---

## 5. Список операций ОПС

В результирующей строке ОПС используются следующие типы элементов:
1. **Операнды**: идентификаторы переменных, константы.
2. **Метки**: идентификаторы точек перехода вида `М<номер>`.
3. **Операции**:

| Символ операции | Описание | Приоритет |
| :--- | :--- | :--- |
| `+`, `-` | Сложение и вычитание | 2 |
| `*`, `/` | Умножение и деление | 3 |
| `<`, `>`, `==`, `<=`, `>=`, `!=` | Операции сравнения | 1 |
| `:=` | Присваивание значения | 0 |
| `[]1` | Обращение к одномерному массиву | 4 |
| `[]2` | Обращение к двумерному массиву | 4 |
| `УПХ` | Условный переход по ложному значению (на метку) | 0 |
| `БП` | Безусловный переход (на метку) | 0 |
| `READ` | Чтение значения переменной из потока ввода | 0 |
| `WRITE` | Вывод значения переменной/константы | 0 |

---

## 6. Формат ОПС

ОПС представляется в виде линейной последовательности токенов (элементов ОПС), разделенных пробелами. Вычисление строки ОПС производится слева направо с использованием стека значений.

### Примеры преобразования исходного кода в ОПС:

#### Пример 1. Присваивание с арифметическим выражением
* **Исходный код:**

`a := b + c * 2;`

* **Выходная строка ОПС:**

`a b c 2 * + :=`

#### Пример 2. Обращение к двумерному массиву
* **Исходный код:**

`x := A[i, j + 1];`

* **Выходная строка ОПС:**

`x A i j 1 + []2 :=`

#### Пример 3. Условный оператор (if-then-else)
* **Исходный код:**

`if a > b then x := 1 else x := 2;`

* **Выходная строка ОПС:**

`a b > M1 УПХ x 1 := M2 БП M1 : x 2 := M2 :`

#### Пример 4. Цикл (while)
* **Исходный код:**

`while i < 10 do i := i + 1;`

* **Выходная строка ОПС:**

`M1 : i 10 < M2 УПХ i i 1 + := M1 БП M2 :`

#### Пример 5. Ввод и вывод
* **Исходный код:**

`read(a);`
`write("Result is");`
`write(a);`

* **Выходная строка ОПС:**

`a READ "Result is" WRITE a WRITE`