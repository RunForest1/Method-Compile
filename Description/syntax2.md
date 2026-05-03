# Документация к транслятору-интерпретатору (Генерация ОПС)

---

## 2. КС-грамматика языка, в которой лексемы суть терминалы

В данной грамматике используются более «говорящие» названия нетерминалов для повышения читаемости. Лексемы (терминалы) выделены строчными буквами, символами операций или полужирным шрифтом для ключевых слов.

**Терминалы:** 
* `id` (идентификаторы), `int_val` (целые), `float_val` (вещественные), `str_val` (строки).
* `+`, `-`, `*`, `/`, `:=`, `<`, `>`, `==`, `!=`, `<=`, `>=`.
* `(`, `)`, `[`, `]`, `,`.
* **if**, **then**, **else**, **while**, **do**, **read**, **write**.

**Нетерминалы:**
* `Stmt` (Оператор)
* `Expr` (Арифметическое выражение)
* `Term` (Слагаемое)
* `Factor` (Множитель)
* `FactorPrefix` (Префикс множителя для унарных операций)
* `Index` (Индексация массива)
* `Cond` (Условие)
* `FuncArgs` (Аргументы функции)

**Порождающие правила:**

1. `Stmt` $\rightarrow$ `id` `Index` `:=` `Expr`
2. `Stmt` $\rightarrow$ **if** `Cond` **then** `Stmt` `ElsePart`
3. `ElsePart` $\rightarrow$ **else** `Stmt` $\mid \lambda$
4. `Stmt` $\rightarrow$ **while** `Cond` **do** `Stmt`
5. `Stmt` $\rightarrow$ **read** `(` `id` `Index` `)`
6. `Stmt` $\rightarrow$ **write** `(` `Expr` `)`

**Арифметические выражения:**

7. `Expr` $\rightarrow$ `Expr` `+` `Term` $\mid$ `Expr` `-` `Term` $\mid$ `Term`
8. `Term` $\rightarrow$ `Term` `*` `Factor` $\mid$ `Term` `/` `Factor` $\mid$ `Factor`
9. `Factor` $\rightarrow$ `(` `Expr` `)` $\mid$ `+` `FactorPrefix` $\mid$ `-` `FactorPrefix` $\mid$ `id` `Index` $\mid$ `id` `(` `FuncArgs` `)` $\mid$ `int_val` $\mid$ `float_val` $\mid$ `str_val`
10. `FactorPrefix` $\rightarrow$ `(` `Expr` `)` $\mid$ `id` `Index` $\mid$ `id` `(` `FuncArgs` `)` $\mid$ `int_val` $\mid$ `float_val` $\mid$ `str_val`

**Массивы и функции:**

11. `Index` $\rightarrow$ `[` `Expr` `]` $\mid$ `[` `Expr` `,` `Expr` `]` $\mid \lambda$
12. `FuncArgs` $\rightarrow$ `Expr` $\mid \lambda$ *(упрощено для 1 аргумента стандартных мат. функций: sqrt, exp, log)*

**Условия:**

13. `Cond` $\rightarrow$ `Expr` `<` `Expr` $\mid$ `Expr` `>` `Expr` $\mid$ `Expr` `==` `Expr` $\mid$ `Expr` `!=` `Expr` $\mid$ `Expr` `<=` `Expr` $\mid$ `Expr` `>=` `Expr`

---

## 3. КС-грамматика языка, преобразованная в нестрогую нормальную форму Грейбах

Для реализации табличного LL(1)-анализатора грамматика очищена от левой рекурсии и приведена к нестрогой форме Грейбах (правые части начинаются с терминала или равны $\lambda$). 

Введены дополнительные нетерминалы для устранения левой рекурсии: `ExprTail`, `TermTail`, `IndexTail`, `CondTail`, `Z` (для обозначения конца разбора ветви). `Const` объединяет терминалы `int_val`, `float_val`, `str_val`.

**Порождающие правила в форме Грейбах:**

1. `Stmt` $\rightarrow$ `id` `Index` `:=` `Expr` `Z` $\mid$ **if** `Cond` **then** `Stmt` `ElsePart` `Z` $\mid$ **while** `Cond` **do** `Stmt` `Z` $\mid$ **read** `(` `id` `Index` `)` `Z` $\mid$ **write** `(` `Expr` `)` `Z`
2. `ElsePart` $\rightarrow$ **else** `Stmt` $\mid \lambda$
3. `Expr` $\rightarrow$ `(` `Expr` `)` `TermTail` `ExprTail` $\mid$ `id` `Index` `TermTail` `ExprTail` $\mid$ `id` `(` `FuncArgs` `)` `TermTail` `ExprTail` $\mid$ `Const` `TermTail` `ExprTail` $\mid$ `+` `FactorPrefix` `TermTail` `ExprTail` $\mid$ `-` `FactorPrefix` `TermTail` `ExprTail`
4. `ExprTail` $\rightarrow$ `+` `Term` `ExprTail` $\mid$ `-` `Term` `ExprTail` $\mid \lambda$
5. `Term` $\rightarrow$ `(` `Expr` `)` `TermTail` $\mid$ `id` `Index` `TermTail` $\mid$ `id` `(` `FuncArgs` `)` `TermTail` $\mid$ `Const` `TermTail` $\mid$ `+` `FactorPrefix` `TermTail` $\mid$ `-` `FactorPrefix` `TermTail`
6. `TermTail` $\rightarrow$ `*` `Factor` `TermTail` $\mid$ `/` `Factor` `TermTail` $\mid \lambda$
7. `Factor` $\rightarrow$ `(` `Expr` `)` $\mid$ `id` `Index` $\mid$ `id` `(` `FuncArgs` `)` $\mid$ `Const` $\mid$ `+` `FactorPrefix` $\mid$ `-` `FactorPrefix` `Z`
8. `FactorPrefix` $\rightarrow$ `(` `Expr` `)` $\mid$ `id` `Index` $\mid$ `id` `(` `FuncArgs` `)` $\mid$ `Const`
9. `Index` $\rightarrow$ `[` `Expr` `IndexTail` $\mid \lambda$
10. `IndexTail` $\rightarrow$ `]` $\mid$ `,` `Expr` `]`
11. `FuncArgs` $\rightarrow$ `Expr` $\mid \lambda$
12. `Cond` $\rightarrow$ `(` `Expr` `)` `TermTail` `ExprTail` `CondTail` $\mid$ `id` `Index` `TermTail` `ExprTail` `CondTail` $\mid$ `Const` `TermTail` `ExprTail` `CondTail` $\mid$ `+` `FactorPrefix` `TermTail` `ExprTail` `CondTail` $\mid$ `-` `FactorPrefix` `TermTail` `ExprTail` `CondTail`
13. `CondTail` $\rightarrow$ `==` `Expr` `Z` $\mid$ `!=` `Expr` `Z` $\mid$ `<` `Expr` `Z` $\mid$ `>` `Expr` `Z` $\mid$ `<=` `Expr` `Z` $\mid$ `>=` `Expr` `Z`
14. `Z` $\rightarrow \lambda$

---

## 4. Семантические действия для генерации ОПС

Семантические действия выполняются синхронно с извлечением элементов из магазина LL(1)-анализатора.

**Обозначения действий:**
* $\Box$ — пустое действие.
* $id$ — запись адреса переменной/функции.
* $const$ — запись значения константы.
* $+$, $-$, $*$, $/$ — бинарные арифметические операции.
* $-'$ — унарный минус.
* $:=$, $==$, $!=$, $<$, $>$, $<=$, $>=$ — операции присваивания и сравнения.
* $i1$ — индексация 1D массива.
* $i2$ — индексация 2D массива.
* $call$ — вызов функции (sqrt, exp, log и т.д.).
* $r$, $w$ — чтение и запись.
* $1, 2, 3, 4, 5$ — семантические программы работы с метками переходов (управление потоком).

**Привязка к правилам (выборка ключевых правил):**

**Присваивание:**
`Stmt` $\rightarrow$ `id` `Index` `:=` `Expr` `Z`
*Действия:* $id \ \Box \ \Box \ \Box \ :=$

**Чтение / Запись:**
`Stmt` $\rightarrow$ **read** `(` `id` `Index` `)` `Z`
*Действия:* $\Box \ \Box \ id \ \Box \ r \ \Box$
`Stmt` $\rightarrow$ **write** `(` `Expr` `)` `Z`
*Действия:* $\Box \ \Box \ \Box \ w \ \Box$

**Арифметика (на примере ExprTail и TermTail):**
`ExprTail` $\rightarrow$ `+` `Term` `ExprTail`
*Действия:* $\Box \ \Box \ +$
`TermTail` $\rightarrow$ `*` `Factor` `TermTail`
*Действия:* $\Box \ \Box \ *$

**Индексы и функции:**
`IndexTail` $\rightarrow$ `]`
*Действия:* $i1$
`IndexTail` $\rightarrow$ `,` `Expr` `]`
*Действия:* $\Box \ \Box \ i2$
`Factor` $\rightarrow$ `id` `(` `FuncArgs` `)`
*Действия:* $id \ \Box \ \Box \ call$

**Условия (на примере `<`):**
`CondTail` $\rightarrow$ `<` `Expr` `Z`
*Действия:* $\Box \ \Box \ <$

**Управляющие конструкции (If / While):**
`Stmt` $\rightarrow$ **if** `Cond` **then** `Stmt` `ElsePart` `Z`
*Действия:* $\Box \ \Box \ 1 \ \Box \ \Box \ 3$
`ElsePart` $\rightarrow$ **else** `Stmt`
*Действия:* $2 \ \Box$

`Stmt` $\rightarrow$ **while** `Cond` **do** `Stmt` `Z`
*Действия:* $4 \ \Box \ 1 \ \Box \ \Box \ 5$

*(Программы 1-5 полностью соответствуют логике заполнения массива ОПС операциями `JF`, `JMP` и пустыми ссылками с их последующим разрешением через дополнительный стек меток).*

---

## 5. Список операций ОПС

Все элементы, попадающие в массив ОПС в результате трансляции, можно разделить на операнды и операции.

| Класс | Мнемоника | Описание (Действие в интерпретаторе) |
| :--- | :--- | :--- |
| **Арифметика** | `+`, `-`, `*`, `/` | Извлекают 2 верхних операнда, выполняют операцию, результат на стек. Выполняется приведение типов (int -> float), если операнды разных типов. |
| **Унарные** | `-'` | Извлекает 1 операнд, меняет знак, кладет обратно. |
| **Отношения** | `==`, `!=`, `<`, `>`, `<=`, `>=` | Извлекают 2 операнда, сравнивают, кладут булево значение (или 1/0). |
| **Память** | `:=` | Извлекает значение и адрес переменной (L-value). Записывает значение по адресу. |
| **Индексация** | `IDX1` | Извлекает индекс и базовый адрес массива. Возвращает эффективный адрес элемента. |
| | `IDX2` | Извлекает 2 индекса и базовый адрес двумерного массива. Возвращает адрес. |
| **Ввод/Вывод** | `READ` | Извлекает адрес, запрашивает ввод с клавиатуры, конвертирует в нужный тип (int/float/string), записывает по адресу. |
| | `WRITE` | Извлекает значение со стека, выводит в консоль. |
| **Переходы** | `JMP` | Безусловный переход. Извлекает адрес (метку) со стека, изменяет Instruction Pointer (IP/Счетчик ОПС). |
| | `JF` | Условный переход (Jump if False). Извлекает метку и условие. Если условие ложно, IP = метка. |
| **Функции** | `CALL_SQRT`, `CALL_EXP`, `CALL_LOG`, `TO_INT`, `TO_FLOAT` | Извлекают необходимое количество аргументов со стека, вычисляют стандартную подпрограмму, результат кладут на стек. |

---

## 6. Формат ОПС

ОПС формируется как последовательный одномерный массив (или список) объектов/структур `RPNItem`. Каждый элемент массива ОПС хранит метаинформацию о своем типе и само значение.

Структура элемента ОПС в памяти (псевдокод):
```cpp
enum class RPNItemType {
    VAR_REF,       // Ссылка на переменную (индекс в таблице символов)
    CONST_INT,     // Целочисленная константа
    CONST_FLOAT,   // Вещественная константа
    CONST_STRING,  // Строковая константа (индекс в таблице строк)
    OPERATOR,      // Код операции (+, -, :=, READ, JMP и т.д.)
    LABEL          // Метка (индекс элемента в массиве ОПС для JMP/JF)
};

struct RPNItem {
    RPNItemType type;
    union {
        int varIndex;     // Для VAR_REF
        int intValue;     // Для CONST_INT
        double floatValue;// Для CONST_FLOAT
        int strIndex;     // Для CONST_STRING
        OpCode op;        // Для OPERATOR (enum OpCode: OP_ADD, OP_ASSIGN...)
        int jumpTarget;   // Для LABEL
    } value;
};
```
Пример формата генерируемого ОПС:

Для выражения: if a < 5 then a := a + 1

Массив ОПС будет выглядеть следующим образом (в виде индексированного списка):


    VAR_REF(a)

    CONST_INT(5)

    OPERATOR(<)

    LABEL(8) (заполнено семантической программой 3 по завершении then-блока)

    OPERATOR(JF)

    VAR_REF(a)

    VAR_REF(a)

    CONST_INT(1)

    OPERATOR(+)

    OPERATOR(:=)