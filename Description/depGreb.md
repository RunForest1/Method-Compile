<Program> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';') <Program>
           | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger> <Program>
           | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger> <Program>
           | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';') <Program>
           | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';') <Program>
           | L_TERMINATOR
           | λ

<Statement> -> L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> <SemanticTrigger> L_DELIMITER(';')
             | L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <SemanticTrigger>
             | L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <SemanticTrigger>
             | L_KEYWORD('read') L_DELIMITER('(') L_ID <ArrayIndex> L_DELIMITER(')') L_DELIMITER(';')
             | L_KEYWORD('write') L_DELIMITER('(') <Expression> L_DELIMITER(')') L_DELIMITER(';')

<ElsePart> -> L_KEYWORD('else') <Statement>
            | λ

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

<ExpressionTail> -> L_ADDITIVE_OPERATOR <Term> <ExpressionTail>
                  | λ

<Term> -> L_DELIMITER('(') <Expression> L_DELIMITER(')') <TermTail>
         | L_ADDITIVE_OPERATOR('+') <UnaryOperand> <TermTail>
         | L_ADDITIVE_OPERATOR('-') <UnaryOperand> <SemanticTrigger> <TermTail>
         | L_ID <ArrayIndex> <TermTail>
         | L_INT <TermTail>
         | L_FLOAT <TermTail>
         | L_STRING <TermTail>

<TermTail> -> L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail>
            | λ

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