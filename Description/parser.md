## 2. КС-грамматика языка, в которой лексемы суть терминалы

Синтаксический анализатор оперирует не отдельными символами исходного кода, а **лексемами**, выданными конечным автоматом (лексером).

### 2.1. Алфавит терминалов 

| Терминал в грамматике | Соответствующий `Lexem` | Условие по `value`  | Пример лексемы |
|:---|:---|:---|:---|
| `T_ID` | `L_ID` | - | `a`, `matrix` |
| `T_INT` | `L_INT` | - | `42` |
| `T_FLOAT` | `L_FLOAT` | - | `3.14` |
| `T_STRING` | `L_STRING` | - | `"hello"`, `"text"` |
| `T_ADD` | `L_ADDITIVE_OPERATOR` | `+` или `-` | `+` |
| `T_MUL` | `L_MULTIPLICATIVE_OPERATOR` | `*` или `/` | `*` |
| `T_ASSIGN` | `L_ASSIGNMENT_OPERATOR` | `:=` | `:=` |
| `T_READ` | `L_KEYWORD` | `"read"` | `read` |
| `T_WRITE` | `L_KEYWORD` | `"write"` | `write` |
| `T_IF` | `L_KEYWORD` | `"if"` | `if` |
| `T_THEN` | `L_KEYWORD` | `"then"` | `then` |
| `T_ELSE` | `L_KEYWORD` | `"else"` | `else` |
| `T_WHILE` | `L_KEYWORD` | `"while"` | `while` |
| `T_DO` | `L_KEYWORD` | `"do"` | `do` |
| `T_CMP` | `L_COMPARISON_OPERATOR` | `<`, `>`, `==`, `!=`, etc. | `<=` |
| `T_LPAREN` | `L_DELIMITER` | `(` | `(` |
| `T_RPAREN` | `L_DELIMITER` | `)` | `)` |
| `T_LBRACKET` | `L_DELIMITER` | `[` | `[` |
| `T_RBRACKET` | `L_DELIMITER` | `]` | `]` |
| `T_SEMICOLON` | `L_DELIMITER` | `;` | `;` |
| `T_EOF` | `L_TERMINATOR` | - | `⊥` |

### 2.2. Правила порождения (БНФ)

Стартовый символ: `Program`. Грамматика описывает структуру операторов, выражений и управляющих конструкций.

```bnf
Program     → StmtList T_EOF
StmtList    → Stmt T_SEMICOLON StmtList | λ
Stmt        → T_ID T_ASSIGN Expr
            | T_READ T_ID
            | T_WRITE Expr
            | T_IF Cond T_THEN StmtList T_ELSE StmtList
            | T_WHILE Cond T_DO StmtList
Cond        → Expr T_CMP Expr
Expr        → Expr T_ADD Term | Expr T_SUB Term | Term
Term        → Term T_MUL Factor | Term T_DIV Factor | Factor
Factor      → T_LPAREN Expr T_RPAREN 
            | T_ID ArrayIdx 
            | T_INT 
            | T_FLOAT
            | T_STRING
ArrayIdx    → T_LBRACKET Expr T_RBRACKET | λ
```

*Примечание:* Вертикальная черта `|` разделяет альтернативы. `λ` обозначает пустую цепочку.

---

## 3. КС-грамматика, преобразованная для LL(1)-анализа

Для обеспечения детерминированного LL(1)-анализа исходная грамматика преобразуется путем **устранения левой рекурсии**. Левая рекурсия присутствует в правилах для `Expr` и `Term`.

### 3.1. Устранение левой рекурсии

Правила вида $A \to A\alpha | \beta$ заменяются на $A \to \beta A'$ и $A' \to \alpha A' | \lambda$:

```bnf
Expr  → Term U
U     → T_ADD Term U | T_SUB Term U | λ

Term  → Factor V
V     → T_MUL Factor V | T_DIV Factor V | λ
```

Остальные правила не содержат левой рекурсии и остаются без изменений (за исключением раскрытия `StmtList` для однозначности выбора правила по первому символу).

### 3.2. Итоговая грамматика (готовая для построения таблицы FIRST/FOLLOW)

```bnf
Program  → StmtList T_EOF

StmtList → T_ID T_ASSIGN Expr T_SEMICOLON StmtList
         | T_READ T_ID T_SEMICOLON StmtList
         | T_WRITE Expr T_SEMICOLON StmtList
         | T_IF Cond T_THEN StmtList T_ELSE StmtList
         | T_WHILE Cond T_DO StmtList
         | T_SEMICOLON StmtList   // Пустой оператор
         | λ                      // Конец списка

Stmt     → T_ID T_ASSIGN Expr
         | T_READ T_ID
         | T_WRITE Expr
         | T_IF Cond T_THEN StmtList T_ELSE StmtList
         | T_WHILE Cond T_DO StmtList

Expr     → Term U
U        → T_ADD Term U | T_SUB Term U | λ

Term     → Factor V
V        → T_MUL Factor V | T_DIV Factor V | λ

Factor   → T_LPAREN Expr T_RPAREN
         | T_ID ArrayIdx
         | T_INT
         | T_FLOAT
         | T_STRING

ArrayIdx → T_LBRACKET Expr T_RBRACKET | λ

Cond     → Expr T_CMP Expr
```

**Результат:** Все правые части правил начинаются с терминала (`T_ID`, `T_INT`, `T_STRING`, `T_LPAREN`, `T_READ`, `T_WRITE`, `T_IF`, `T_WHILE`, `T_SEMICOLON`) или являются пустыми (`λ`). Это позволяет однозначно определить применяемое правило по одной следующей лексеме (lookahead).

---

## 4. Семантические действия для генерации ОПС

Генерация обратной польской строки (ОПС) выполняется параллельно с синтаксическим разбором.

### 4.1. Система обозначений действий

| Символ действия | Описание |
|:---|:---|
| `□` | Пустое действие (синхронизация стека) |
| `PUSH_ID` | Взять `value` текущей лексемы `T_ID`, найти/создать запись в `VarTable`, поместить индекс в стек ОПС. |
| `PUSH_INT` | Взять `value` лексемы `T_INT`, добавить в `ConstTable`, поместить индекс в стек ОПС. |
| `PUSH_FLOAT` | Взять `value` лексемы `T_FLOAT`, добавить в `ConstTable`, поместить индекс в стек ОПС. |
| `PUSH_STR` | Взять `value` лексемы `T_STRING`, добавить в `StringTable`, поместить индекс в стек ОПС. |
| `OP_ADD`, `OP_SUB` | Сгенерировать операцию сложения/вычитания. |
| `OP_MUL`, `OP_DIV` | Сгенерировать операцию умножения/деления. |
| `OP_ASSIGN` | Сгенерировать операцию присваивания `:=`. |
| `OP_READ` | Сгенерировать операцию ввода. |
| `OP_WRITE` | Сгенерировать операцию вывода. |
| `OP_IDX` | Сгенерировать операцию индексации массива. |
| `JZ`, `JMP`, `LABEL` | Управление потоком (для `IF`/`WHILE`). |

### 4.2. Фрагмент таблицы LL(1)-анализатора с семантическими действиями

Ячейки содержат последовательность действий, соответствующих символам правой части правила.

| Нетерминал | Входной токен | Правило (Правая часть) | Семантические действия (последовательность) |
|:---|:---|:---|:---|
| `Program` | `T_ID`... | `StmtList T_EOF` | `□ □` |
| `StmtList` | `T_ID` | `T_ID T_ASSIGN Expr ; StmtList` | `PUSH_ID` `□` `□` `□` `□` |
| `StmtList` | `T_READ` | `T_READ T_ID ; StmtList` | `□` `PUSH_ID` `OP_READ` `□` `□` |
| `StmtList` | `T_WRITE` | `T_WRITE Expr ; StmtList` | `□` `□` `OP_WRITE` `□` `□` |
| `Expr` | `T_INT` | `Term U` | `□ □` |
| `Term` | `T_INT` | `Factor V` | `□ □` |
| `Factor` | `T_INT` | `T_INT` | `PUSH_INT` |
| `Factor` | `T_FLOAT` | `T_FLOAT` | `PUSH_FLOAT` |
| `Factor` | `T_STRING` | `T_STRING` | `PUSH_STR` |
| `Factor` | `T_ID` | `T_ID ArrayIdx` | `PUSH_ID` `□` |
| `ArrayIdx` | `T_LBRACKET` | `[ Expr ]` | `□` `□` `□` `OP_IDX` |
| `U` | `T_ADD` | `+ Term U` | `OP_ADD` `□` `□` |
| `V` | `T_MUL` | `* Factor V` | `OP_MUL` `□` `□` |

*Примечание:* Действия вроде `OP_ADD` выполняются после того, как операнды уже обработаны и находятся в стеке ОПС. В таблице они привязаны к моменту чтения оператора.

---

## 5. Список операций ОПС и формат данных

### 5.1. Структура элемента ОПС

```c
typedef enum {
    OPS_CONST = 0,    // Числовая константа (int/float)
    OPS_VAR = 1,      // Простая переменная
    OPS_ARRAY = 2,    // Массив (ссылка на дескриптор)
    OPS_OP = 3,       // Операция
    OPS_ADDR = 4,     // Адрес перехода (метка)
    OPS_STRING = 5    // Строковая константа
} OPS_Type;

struct OPS_Element {
    OPS_Type type;    // Тип элемента
    union {
        int  ref_idx; // Индекс в таблице символов, констант или строк
        int  op_code; // Код операции
        int  addr;    // Смещение в массиве ОПС для переходов
    } data;
};
```

### 5.2. Таблицы, используемые парсером

| Таблица | Назначение | Содержимое |
|:---|:---|:---|
| `VarTable` | Скалярные переменные | Имя, тип, текущее значение |
| `ConstTable` | Числовые литералы | Значение (int или double) |
| `StringTable` | Строковые литералы | Значение строки (`std::string`) |
| `ArrayTable` | Массивы | Имя, размерность, границы, ссылка на данные |

### 5.3. Пример генерации ОПС со строками

**Входная цепочка:** `msg := "Hello"`

**Лексемы:**
1. `Lexem(L_ID, "msg")`
2. `Lexem(L_ASSIGNMENT_OPERATOR, ":=")`
3. `Lexem(L_STRING, "Hello")`

**Сгенерированная ОПС:**
```cpp
{ OPS_STRING, ref_Hello }  // PUSH_STR: индекс строки "Hello" в StringTable
{ OPS_VAR, ref_msg }       // PUSH_ID: индекс переменной msg в VarTable
{ OPS_OP, OP_ASSIGN }      // OP_ASSIGN: присваивание
```

**Интерпретация:**
1. В стек операндов помещается ссылка на строковую константу.
2. В стек помещается ссылка на переменную `msg`.
3. Операция `:=` извлекает адрес переменной и значение, записывает строку в память переменной.