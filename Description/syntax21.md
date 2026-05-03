# Документация к транслятору-интерпретатору (Генерация ОПС)

---

## 2. КС-грамматика языка, в которой лексемы суть терминалы

Грамматика описывает структуру программы, включающую объявления переменных, присваивания, условия, циклы и операции ввода-вывода. Терминалы соответствуют именам в коде из `lexer.md`.

**Терминалы (Лексемы):**
* `L_ID` — Идентификаторы.
* `L_INT`, `L_FLOAT`, `L_STRING` — Константы.
* `L_KEYWORD(if)`, `L_KEYWORD(then)`, `L_KEYWORD(else)`, `L_KEYWORD(while)`, `L_KEYWORD(do)`, `L_KEYWORD(read)`, `L_KEYWORD(write)`, `L_KEYWORD(int)`, `L_KEYWORD(float)`.
* `L_ADDITIVE_OPERATOR` (`+`, `-`), `L_MULTIPLICATIVE_OPERATOR` (`*`, `/`), `L_COMPARISON_OPERATOR` (`<`, `>`, `==`, `!=`, `<=`, `>=`), `L_ASSIGNMENT_OPERATOR` (`:=`).
* `L_DELIMITER` (`(`, `)`, `[`, `]`, `;`, `,`).
* `L_TERMINATOR` (`⊥`).

**Нетерминалы:**
* `Prog` — Программа.
* `StmtList` — Список операторов.
* `Stmt` — Отдельный оператор.
* `Decl` — Объявление переменной/массива.
* `Expr` — Арифметическое выражение.
* `Cond` — Условие.
* `Idx` — Индексация.

**Правила:**
1. `Prog` → `StmtList` `L_TERMINATOR`
2. `StmtList` → `Stmt` `L_DELIMITER(;)` `StmtList` | λ
3. `Stmt` → `Decl` | `L_ID` `Idx` `L_ASSIGNMENT_OPERATOR` `Expr` | `L_KEYWORD(if)` `Cond` `L_KEYWORD(then)` `Stmt` `ElsePart` | `L_KEYWORD(while)` `Cond` `L_KEYWORD(do)` `Stmt` | `L_KEYWORD(read)` `L_DELIMITER(()` `L_ID` `Idx` `L_DELIMITER())` | `L_KEYWORD(write)` `L_DELIMITER(()` `Expr` `L_DELIMITER())`
4. `ElsePart` → `L_KEYWORD(else)` `Stmt` | λ
5. `Decl` → (`L_KEYWORD(int)` | `L_KEYWORD(float)`) `L_ID` `ArrayDecl`
6. `ArrayDecl` → `L_DELIMITER([)` `L_INT` `ArrayDeclTail` | λ
7. `ArrayDeclTail` → `L_DELIMITER(])` | `L_DELIMITER(,)` `L_INT` `L_DELIMITER(])`
8. `Expr` → `Expr` `L_ADDITIVE_OPERATOR` `Term` | `Term`
9. `Term` → `Term` `L_MULTIPLICATIVE_OPERATOR` `Fact` | `Fact`
10. `Fact` → `L_DELIMITER(()` `Expr` `L_DELIMITER())` | `L_ADDITIVE_OPERATOR` `Fact` | `L_ID` `Idx` | `L_INT` | `L_FLOAT` | `L_STRING`
11. `Idx` → `L_DELIMITER([)` `Expr` `IdxTail` | λ
12. `IdxTail` → `L_DELIMITER(])` | `L_DELIMITER(,)` `Expr` `L_DELIMITER(])`
13. `Cond` → `Expr` `L_COMPARISON_OPERATOR` `Expr`

---

## 3. КС-грамматика в нестрогой нормальной форме Грейбах

Для работы LL(1)-анализатора устранена левая рекурсия. Введены нетерминалы-хвосты (`Tail`).

1. `Prog` → `StmtList` `L_TERMINATOR`
2. `StmtList` → `Stmt` `L_DELIMITER(;)` `StmtList` | λ
3. `Stmt` → `L_KEYWORD(int)` `L_ID` `ArrayDecl` | `L_KEYWORD(float)` `L_ID` `ArrayDecl` | `L_ID` `Idx` `L_ASSIGNMENT_OPERATOR` `Expr` | `L_KEYWORD(if)` `Cond` `L_KEYWORD(then)` `Stmt` `ElsePart` | `L_KEYWORD(while)` `Cond` `L_KEYWORD(do)` `Stmt` | `L_KEYWORD(read)` `L_DELIMITER(()` `L_ID` `Idx` `L_DELIMITER())` | `L_KEYWORD(write)` `L_DELIMITER(()` `Expr` `L_DELIMITER())`
4. `ElsePart` → `L_KEYWORD(else)` `Stmt` | λ
5. `Expr` → `Term` `ExprTail`
6. `ExprTail` → `L_ADDITIVE_OPERATOR` `Term` `ExprTail` | λ
7. `Term` → `Fact` `TermTail`
8. `TermTail` → `L_MULTIPLICATIVE_OPERATOR` `Fact` `TermTail` | λ
9. `Fact` → `L_DELIMITER(()` `Expr` `L_DELIMITER())` | `L_ADDITIVE_OPERATOR` `Fact` | `L_ID` `Idx` | `L_INT` | `L_FLOAT` | `L_STRING`
10. `ArrayDecl` → `L_DELIMITER([)` `L_INT` `ArrayDeclTail` | λ
11. `ArrayDeclTail` → `L_DELIMITER(])` | `L_DELIMITER(,)` `L_INT` `L_DELIMITER(])`
12. `Idx` → `L_DELIMITER([)` `Expr` `IdxTail` | λ
13. `IdxTail` → `L_DELIMITER(])` | `L_DELIMITER(,)` `Expr` `L_DELIMITER(])`
14. `Cond` → `Expr` `L_COMPARISON_OPERATOR` `Expr`

---

## 4. Семантические действия для генерации ОПС

Используется нотация из `parser_rules_manual_verified.md`.  
`a` — адрес идентификатора, `v` — значение константы.

| Правило (фрагмент) | Семантические действия |
| :--- | :--- |
| `Stmt` → `L_ID` `Idx` `L_ASSIGNMENT_OPERATOR` `Expr` | `a` `Box` `Box` `:=` |
| `Stmt` → `L_KEYWORD(if)` `Cond` `L_KEYWORD(then)` `Stmt` `ElsePart` | `Box` `Box` `1` `Box` `Box` `3` |
| `ElsePart` → `L_KEYWORD(else)` `Stmt` | `2` `Box` |
| `Stmt` → `L_KEYWORD(while)` `Cond` `L_KEYWORD(do)` `Stmt` | `4` `Box` `1` `Box` `Box` `5` |
| `Stmt` → `L_KEYWORD(read)` `L_DELIMITER(()` `L_ID` `Idx` `L_DELIMITER())` | `Box` `Box` `a` `Box` `r` `Box` |
| `Stmt` → `L_KEYWORD(write)` `L_DELIMITER(()` `Expr` `L_DELIMITER())` | `Box` `Box` `Box` `w` `Box` |
| `IdxTail` → `L_DELIMITER(])` | `i1` |
| `IdxTail` → `L_DELIMITER(,)` `Expr` `L_DELIMITER(])` | `Box` `Box` `i2` |
| `ExprTail` → `L_ADDITIVE_OPERATOR` `Term` `ExprTail` | `Box` `Box` `{op}` |
| `TermTail` → `L_MULTIPLICATIVE_OPERATOR` `Fact` `TermTail` | `Box` `Box` `{op}` |
| `Fact` → `L_ID` `Idx` | `a` `Box` |
| `Fact` → `L_INT` \| `L_FLOAT` \| `L_STRING` | `v` |
| `Cond` → `Expr` `L_COMPARISON_OPERATOR` `Expr` | `Box` `{op}` `Box` |

**Специальные программы управления метками:**
1. **1**: Сформировать `LABEL_Placeholder`, `JF` (адрес в стек меток).
2. **2**: Сформировать `LABEL_Placeholder`, `JMP`, заполнить адрес для предыдущего `JF`, (новый адрес в стек меток).
3. **3**: Заполнить адрес для последнего `JF` или `JMP` из стека меток текущим индексом ОПС.
4. **4**: Запомнить текущий индекс ОПС (начало условия цикла) в стек меток.
5. **5**: Сформировать `LABEL` (из стека меток), `JMP`, затем выполнить программу **3** для выхода из цикла по `JF`.

---

## 5. Список операций ОПС

| Операция | Описание |
| :--- | :--- |
| `+`, `-`, `*`, `/` | Бинарные арифметические операции. |
| `==`, `!=`, `<`, `>`, `<=`, `>=` | Операции сравнения (результат: 0 или 1). |
| `:=` | Присваивание: `адрес` `значение` `:=`. |
| `r` | Ввод (read): `адрес` `r`. |
| `w` | Вывод (write): `значение` `w`. |
| `i1` | Индексация 1D: `база` `индекс` `i1` -> `адрес_эл`. |
| `i2` | Индексация 2D: `база` `индекс1` `индекс2` `i2` -> `адрес_эл`. |
| `JF` | Условный переход по лжи: `метка` `значение` `JF`. |
| `JMP` | Безусловный переход: `метка` `JMP`. |
| `@` | (Опционально) Взятие значения по адресу (разразименование). |

---

## 6. Формат ОПС

ОПС представляется в виде массива структур.

**Элемент ОПС:**
- **Тип:** (Операнд, Операция, Метка).
- **Значение:** (Индекс в таблице символов, код операции или индекс в массиве ОПС).

**Пример формирования:**
Для `if a < 5 then write(a);`
ОПС: `a` `5` `<` `L1` `JF` `a` `w` `L1:` (где L1 — индекс следующего за блоком элемента).