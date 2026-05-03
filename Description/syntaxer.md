
# 2. КС-грамматика языка

В данной грамматике терминалами являются лексемы, определенные в лексическом анализаторе.

**Нетерминалы:**
* `<Program>` — входная точка программы, последовательность операторов.
* `<Statement>` — обобщенный оператор языка.
* `<Assignment>` — оператор присваивания значения переменной или элементу массива.
* `<IfStatement>` — условный оператор (полная и сокращенная формы).
* `<ElsePart>` — ветка «иначе» условного оператора.
* `<WhileStatement>` — цикл с предусловием.
* `<ReadStatement>` — оператор ввода данных.
* `<WriteStatement>` — оператор вывода данных.
* `<Condition>` — логическое условие (сравнение двух выражений).
* `<Expression>` — арифметическое выражение (операции +, -).
* `<Term>` — слагаемое выражения (операции *, /).
* `<Factor>` — элементарный множитель (число, переменная, выражение в скобках или унарная операция).
* `<UnaryOperand>` — операнд, к которому применяется унарный знак (соответствует нетерминалу `G`).
* `<ArrayIndex>` — индексный блок массива (поддержка одномерных и двумерных массивов).
* `<SemanticTrigger>` — нетерминал, порождающий λ, служащий для вызова семантического действия в ОПС (соответствует `Z`).

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

<ReadStatement> -> L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')

<WriteStatement> -> L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<Condition> -> <Expression> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>

<Expression> -> <Expression> L_ADDITIVE_OPERATOR <Term> | <Term>

<Term> -> <Term> L_MULTIPLICATIVE_OPERATOR <Factor> | <Factor>

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
           | L_ADDITIVE_OPERATOR('+') <UnaryOperand>
           | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger>
           | L_ID <ArrayIndex>
           | L_INT
           | L_FLOAT
           | L_STRING

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
                | L_ID <ArrayIndex>
                | L_INT
                | L_FLOAT

<ArrayIndex> -> L_DELIMITER('[') <Expression> <ArrayTail>
<ArrayTail> -> L_DELIMITER(']') | L_DELIMITER(',') <Expression> L_DELIMITER(']') | λ

<SemanticTrigger> -> λ
```

# Избавление от левой рекурсии в КС-грамматике

В исходной грамматике левая рекурсия присутствует в правилах для арифметических выражений, что недопустимо для LL(1)-анализаторов. Ниже приведен процесс трансформации.

## 1. Выявление леворекурсивных правил

Левая рекурсия (прямая) обнаружена в следующих правилах:
1. `<Expression> -> <Expression> L_ADDITIVE_OPERATOR <Term> | <Term>`
2. `<Term> -> <Term> L_MULTIPLICATIVE_OPERATOR <Factor> | <Factor>`

Остальные правила не являются леворекурсивными, так как их правые части начинаются либо с терминалов, либо с нетерминалов, не приводящих к текущему в первом же шаге вывода.

## 2. Алгоритм устранения

### 2.1. Для нетерминала `<Expression>`
Исходное:
* $A = \langle\text{Expression}\rangle$
* $\alpha = L\_ADDITIVE\_OPERATOR \langle\text{Term}\rangle$
* $\beta = \langle\text{Term}\rangle$

Результат:
* `<Expression> -> <Term> <ExpressionTail>`
* `<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail> | λ`

### 2.2. Для нетерминала `<Term>`
Исходное:
* $A = \langle\text{Term}\rangle$
* $\alpha = L\_MULTIPLICATIVE\_OPERATOR \langle\text{Factor}\rangle$
* $\beta = \langle\text{Factor}\rangle$

Результат:
* `<Term> -> <Factor> <TermTail>`
* `<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ`

---

## 3. Итоговая грамматика без левой рекурсии

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

<ReadStatement> -> L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')

<WriteStatement> -> L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<Condition> -> <Expression> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>

<Expression> -> <Term> <ExpressionTail>

<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail> | λ

<Term> -> <Factor> <TermTail>

<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
           | L_ADDITIVE_OPERATOR('+') <UnaryOperand>
           | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger>
           | L_ID <ArrayIndex>
           | L_INT
           | L_FLOAT
           | L_STRING

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
                | L_ID <ArrayIndex>
                | L_INT
                | L_FLOAT

<ArrayIndex> -> L_DELIMITER('[') <Expression> L_DELIMITER(']') 
              | L_DELIMITER('[') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(']') 
              | λ

<SemanticTrigger> -> λ
```


# 3. КС-грамматика в нестрогой нормальной форме Грейбах

Для LL(1)-анализа устранена левая рекурсия (введены хвосты `<ExpressionTail>` и `<TermTail>`) и выполнены подстановки для обеспечения терминального начала правил.

```
<Program> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';') <Program>
           | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger> <Program>
           | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger> <Program>
           | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';') <Program>
           | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';') <Program>
           | λ | L_TERMINATOR

<Statement> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')
             | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>
             | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>
             | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')
             | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<Assignment> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')

<IfStatement> -> L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>

<ElsePart> -> L_KEYWORD('else') <Statement> | λ

<WhileStatement> -> L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>

<ReadStatement> -> L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')

<WriteStatement> -> L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<Condition> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_ID <ArrayIndex> <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_INT <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_FLOAT <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>
             | L_STRING <TermTail> <ExpressionTail> L_COMPARISON_OPERATOR <Expression> <SemanticTrigger>

<Expression> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail> <ExpressionTail>
              | L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail> <ExpressionTail>
              | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail> <ExpressionTail>
              | L_ID <ArrayIndex> <TermTail> <ExpressionTail>
              | L_INT <TermTail> <ExpressionTail>
              | L_FLOAT <TermTail> <ExpressionTail>
              | L_STRING <TermTail> <ExpressionTail>

<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail> | λ

<Term> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail>
        | L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail>
        | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail>
        | L_ID <ArrayIndex> <TermTail>
        | L_INT <TermTail>
        | L_FLOAT <TermTail>
        | L_STRING <TermTail>

<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail> | λ

<Factor> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
          | L_ADDITIVE_OPERATOR('+') <UnaryOperand>
          | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger>
          | L_ID <ArrayIndex>
          | L_INT
          | L_FLOAT
          | L_STRING

<UnaryOperand> -> L_DELIMITER('(') <Expression> L_DELIMITER(')')
                | L_ID <ArrayIndex>
                | L_INT
                | L_FLOAT

<ArrayIndex> -> L_DELIMITER('[') <Expression> L_DELIMITER(']') 
              | L_DELIMITER('[') <Expression> L_DELIMITER(',') <Expression> L_DELIMITER(']') 
              | λ

<SemanticTrigger> -> λ

```