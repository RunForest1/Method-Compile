# 2. КС-грамматика языка

В данной грамматике терминалами являются лексемы, определенные в лексическом анализаторе.

**Нетерминалы:**
* `<Program>` — корень программы (последовательность операторов).
* `<Statement>` — отдельный оператор.
* `<Expression>` — арифметическое выражение (+, -).
* `<ExpressionTail>` — хвост выражения для устранения левой рекурсии.
* `<Term>` — слагаемое (*, /).
* `<TermTail>` — хвост слагаемого.
* `<Factor>` — элементарный множитель.
* `<UnaryOperand>` — операнд после унарного знака (соответствует `G`).
* `<Condition>` — логическое условие для `if` и `while`.
* `<ComparisonPart>` — вторая часть операции сравнения.
* `<ArrayIndex>` — индекс массива.
* `<IndexTail>` — завершение индексации (различие между `[x]` и `[x, y]`).
* `<ElsePart>` — необязательная ветвь `else`.
* `<SemanticTrigger>` — нетерминал, всегда порождающий λ, служащий сигналом для выполнения семантического действия из магазина (соответствует `Z`).

**Порождающие правила (стандартная форма):**

```
<Program> -> <Statement> <Program> | λ | L_TERMINATOR

<Statement> -> <Assignment>
              | <IfStatement>
              | <WhileStatement>
              | <ReadStatement>
              | <WriteStatement>

<Assignment> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')

<IfStatement> -> L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>

<ElsePart> -> L_KEYWORD('else') <Statement> | λ

<WhileStatement> -> L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>

<ReadStatement> -> L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';')

<WriteStatement> -> L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';')

<Condition> -> <Expression> L_COMPARISON_OPERATOR <Expression>

<Expression> -> <Expression> L_ADDITIVE_OPERATOR <Term> | <Term>

<Term> -> <Term> L_MULTIPLICATIVE_OPERATOR <Factor> | <Factor>

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
           | L_ADDITIVE_OPERATOR <UnaryOperand> <SemanticTrigger>
           | L_ID <ArrayIndex>
           | L_INT
           | L_FLOAT
           | L_STRING

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
                | L_ID <ArrayIndex>
                | L_INT
                | L_FLOAT

<ArrayIndex> -> L_DELIMITER('[') <Expression> <IndexTail> | λ

<IndexTail> -> L_DELIMITER(',') <Expression> L_DELIMITER(']') | L_DELIMITER(']')

<SemanticTrigger> -> λ
```

# 3. КС-грамматика в нестрогой нормальной форме Грейбах

```
<Program> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';') <Program>
           | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger> <Program>
           | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger> <Program>
           | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';') <Program>
           | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';') <Program>
           | λ

<Statement> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')
             | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>
             | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>
             | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';')
             | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') <SemanticTrigger> L_DELIMITER(';')

<Expression> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail>
              | L_ID <ArrayIndex> <TermTail> <ExpressionTail>
              | L_INT <TermTail> <ExpressionTail>
              | L_FLOAT <TermTail> <ExpressionTail>
              | L_STRING <TermTail> <ExpressionTail>
              | L_ADDITIVE_OPERATOR <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail>

<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail> | λ

<Term> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail>
        | L_ID <ArrayIndex> <TermTail>
        | L_INT <TermTail>
        | L_FLOAT <TermTail>
        | L_STRING <TermTail>
        | L_ADDITIVE_OPERATOR <UnaryOperand> <SemanticTrigger> <TermTail>

<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
          | L_ID <ArrayIndex>
          | L_INT
          | L_FLOAT
          | L_STRING
          | L_ADDITIVE_OPERATOR <UnaryOperand> <SemanticTrigger>

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
               | L_ID <ArrayIndex>
               | L_INT
               | L_FLOAT

<Condition> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail> <ComparisonPart>
             | L_ID <ArrayIndex> <TermTail> <ExpressionTail> <ComparisonPart>
             | L_INT <TermTail> <ExpressionTail> <ComparisonPart>
             | L_FLOAT <TermTail> <ExpressionTail> <ComparisonPart>
             | L_STRING <TermTail> <ExpressionTail> <ComparisonPart>
             | L_ADDITIVE_OPERATOR <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail> <ComparisonPart>

<ComparisonPart> -> L_COMPARISON_OPERATOR <Expression>

<ArrayIndex> -> L_DELIMITER('[') <Expression> <IndexTail> | λ

<IndexTail> -> L_DELIMITER(']') | L_DELIMITER(',') <Expression> L_DELIMITER(']')

<ElsePart> -> L_KEYWORD('else') <Statement> | λ

<SemanticTrigger> -> λ
```

# 4. Семантические действия для генерации ОПС

В LL(1)-генераторе действия привязаны к конкретным правилам. Когда правило раскрывается, список действий помещается в специальный магазин действий.

| Нетерминал | Правило | Семантические действия |
| :--- | :--- | :--- |
| **Statement** | `L_ID ArrayIndex L_ASSIGN_OP Expression SemanticTrigger L_DELIM(';')` | a $\Box$ $\Box$ $\Box$ := $\Box$ |
| | `L_KEY('if') Condition L_KEY('then') Statement ElsePart SemanticTrigger` | $\Box$ $\Box$ 1 $\Box$ $\Box$ 3 |
| | `L_KEY('while') Condition L_KEY('do') Statement SemanticTrigger` | 4 $\Box$ 1 $\Box$ 5 |
| | `L_KEY('read') L_DELIM('(') L_ID ArrayIndex L_DELIM(')') SemanticTrigger L_DELIM(';')` | $\Box$ $\Box$ a $\Box$ $\Box$ r $\Box$ |
| | `L_KEY('write') L_DELIM('(') Expression L_DELIM(')') SemanticTrigger L_DELIM(';')` | $\Box$ $\Box$ $\Box$ $\Box$ w $\Box$ |
| **Expression** | `L_ADD_OP UnaryOperand SemanticTrigger TermTail ExpressionTail` | $\Box$ $\Box$ -' $\Box$ $\Box$ |
| **ElsePart** | `L_KEY('else') Statement` | 2 $\Box$ |
| **ComparisonPart**| `L_COMP_OP Expression` | $\Box$ cmp |
| **IndexTail** | `L_DELIMITER(']')` | i |
| | `L_DELIMITER(',') Expression L_DELIMITER(']')` | $\Box$ $\Box$ i2 |

# 5. Список операций ОПС
* `+`, `-`, `*`, `/` — арифметические операции.
* `-'` — унарный минус.
* `==`, `!=`, `<`, `>`, `<=`, `>=` — операции сравнения (`cmp`).
* `:=` — присваивание.
* `j` — безусловный переход.
* `jf` — переход по условию (false).
* `i`, `i2` — индексация массивов.
* `r` — чтение (read).
* `w` — запись (write).

# 6. Формат ОПС
ОПС представляет собой массив объектов, содержащих тип (операнд/операция/метка) и значение. Исполнение происходит с использованием стека: операнды кладутся в стек, операции извлекают их и кладут результат обратно.

# 7. Таблица переходов LL(1)-анализатора и генератора ОПС

| Нетерминал | ID | Const | Add | Mult | Cmp | Assign | ( | ) | [ | ] | , | ; | if | then | else | while | do | read | write | ┴ |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Program** | `Stat Progr` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `Stat Progr` | `λ` | `λ` | `Stat Progr` | `λ` | `Stat Progr` | `Stat Progr` | `λ` |
| **Statement** | `ID ArrInd := Expr SemTr ;` <br> a $\Box$ $\Box$ $\Box$ := $\Box$ | | | | | | | | | | | | `if Cond then Stat Else SemTr` <br> $\Box$ $\Box$ 1 $\Box$ $\Box$ 3 | | | `while Cond do Stat SemTr` <br> 4 $\Box$ 1 $\Box$ 5 | | `read ( ID ArrInd ) SemTr ;` <br> $\Box$ $\Box$ a $\Box$ $\Box$ r $\Box$ | `write ( Expr ) SemTr ;` <br> $\Box$ $\Box$ $\Box$ $\Box$ w $\Box$ | |
| **Expression** | `ID ArrInd TTail ETail` | `Const TTail ETail` | `Add UOper SemTr TTail ETail` <br> $\Box$ $\Box$ -' $\Box$ $\Box$ | | | | `( Expr ) TTail ETail` | | | | | | | | | | | | | |
| **ExpressionTail**| `λ` | `λ` | `Add Term ETail` <br> $\Box$ $\Box$ [+/-] | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **Term** | `ID ArrInd TTail` | `Const TTail` | `Add UOper SemTr TTail` | | | | `( Expr ) TTail` | | | | | | | | | | | | | |
| **TermTail** | `λ` | `λ` | `λ` | `Mult Fact TTail` <br> $\Box$ $\Box$ [*/] | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **Factor** | `ID ArrInd` | `Const` | `Add UOper SemTr` | | | | `( Expr )` | | | | | | | | | | | | | |
| **UnaryOperand** | `ID ArrInd` | `Const` | | | | | `( Expr )` | | | | | | | | | | | | | |
| **Condition** | `ID ArrInd TTail ETail CmpP` | `Const TTail ETail CmpP` | `Add UOper SemTr TTail ETail CmpP` | | | | `( Expr ) TTail ETail CmpP` | | | | | | | | | | | | | |
| **ComparisonPart**| | | | | `Cmp Expr` <br> $\Box$ cmp | | | | | | | | | | | | | | | |
| **ArrayIndex** | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `[ Expr ITail` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
| **IndexTail** | | | | | | | | | | `]` <br> i | `, Expr ]` <br> $\Box$ $\Box$ i2 | | | | | | | | | |
| **ElsePart** | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `else Stat` <br> 2 $\Box$ | `λ` | `λ` | `λ` | `λ` | `λ` |
| **SemanticTrigger**| `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` | `λ` |
