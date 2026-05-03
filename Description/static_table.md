| Обозначение | Описание |
| :--- | :--- |
| `id` | Идентификатор (переменная). |
| `k` | Константа (`L_INT`, `L_FLOAT`, `L_STRING`). |
| `□` | Ожидание завершения вычисления нетерминала (результат его разбора). |
| `i` | Генерация операции индексации одномерного массива. |
| `i2` | Генерация операции индексации двумерного массива. |
| `~` (или `-` унарный) | Операция унарного минуса (изменение знака). |
| `+`, `-`, `*`, `:=`, `<` и др. | Символы арифметических и логических операций, означающие генерацию соответствующего символа в ОПЗ (Обратной Польской Записи). |

---

| Нетерминал | Правило | Семантическое действие (ОПС) |
| :--- | :--- | :--- |
| `<Program>` | `L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> ; <Program>` | `ID □ □ := □` |
| | `, L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <Program>` | `□ m1 jf □ □ □` |
| | `, L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <Program>` | `сохранить m0 □ m1 jf □ m0 j заполнить m1 □` |
| | `, L_KEYWORD('read') ( L_ID <ArrayIndex> ) ; <Program>` | `ID □ r □` |
| | `, L_KEYWORD('write') ( <Expression> ) ; <Program>` | `□ w □` |
| `<Condition>` | `<Exp> L_COMPARISON_OPERATOR <Exp>` | `□ □ {OP}` (где `{OP}` — `<`, `>`, `=` и т.д.) |
| `<Expression>` | `L_ID <ArrayIndex> <TermTail> <ExpressionTail>` | `ID □ □ □` |
| | `, L_INT <TermTail> <ExpressionTail>` | `INT □ □` |
| | `, L_ADDITIVE_OPERATOR('-') <UnaryOperand> <TermTail> <ExpressionTail>` | `□ ~ □ □` |
| `<ExpressionTail>` | `L_ADDITIVE_OPERATOR <Term> <ExpressionTail>` | `□ □ {+ или -}` |
| | `, λ` | *(пусто)* |
| `<TermTail>` | `L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail>` | `□ □ {* или /}` |
| | `, λ` | *(пусто)* |
| `<Factor>` | `L_ID <ArrayIndex>` | `ID □` |
| | `, L_INT` | `INT` |
| `<ArrayIndex>` | `[ <Expression> ]` | `□ i` |
| | `[ <Expression> , <Expression> ]` | `□ □ i2` |
| | `, λ` | *(пусто)* |
| `<ElsePart>` | `L_KEYWORD('else') <Statement>` | `m2 j заполнить m1 □ заполнить m2` |
| | `, λ` | `заполнить m1` |