| Нетерминал | Правило и Семантическое действие (ОПС) |
| :--- | :--- |
| `<Program>` | `L_ID <ArrayIndex> L_ASSIGNMENT_OPERATOR <Expression> ; <Program>`<br>`ID □ □ := □` |
| | `, L_KEYWORD('if') <Condition> L_KEYWORD('then') <Statement> <ElsePart> <Program>`<br>`□ m1 jf □ □ □` |
| | `, L_KEYWORD('while') <Condition> L_KEYWORD('do') <Statement> <Program>`<br>`сохранить m0 □ m1 jf □ m0 j заполнить m1 □` |
| | `, L_KEYWORD('read') ( L_ID <ArrayIndex> ) ; <Program>`<br>`ID □ r □` |
| | `, L_KEYWORD('write') ( <Expression> ) ; <Program>`<br>`□ w □` |
| `<Condition>` | `"<Exp> L_COMPARISON_OPERATOR <Exp>`<br>`□ □ {OP} (где {OP} — <, >, = и т.д.)"` |
| `<Expression>` | `L_ID <ArrayIndex> <TermTail> <ExpressionTail>`<br>`ID □ □ □` |
| | `, L_INT <TermTail> <ExpressionTail>`<br>`INT □ □` |
| | `, L_ADDITIVE_OPERATOR('-') <UnaryOperand> <TermTail> <ExpressionTail>`<br>`□ ~ □ □` |
| `<ExpressionTail>` | `L_ADDITIVE_OPERATOR <Term> <ExpressionTail>`<br>`□ □ {+ или -}` |
| | `, λ (пустое слово)` |
| `<TermTail>` | `L_MULTIPLICATIVE_OPERATOR <Factor> <TermTail>`<br>`□ □ {* или /}` |
| | `, λ` |
| `<Factor>` | `L_ID <ArrayIndex>`<br>`ID □` |
| | `, L_INT`<br>`INT` |
| `<ArrayIndex>` | `[ <Expression> ]`<br>`□ i` |
| | `"[ <Expression> , <Expression> ]"`<br>`□ □ i2` |
| | `, λ` |
| `<ElsePart>` | `L_KEYWORD('else') <Statement>`<br>`m2 j заполнить m1 □ заполнить m2` |
| | `, λ`<br>`заполнить m1`