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
