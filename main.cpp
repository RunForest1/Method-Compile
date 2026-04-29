#include <iostream>
#include "lexer/Lexer.h"
#include "parser/Parser.h"


int main() {
    std::string sourceCode = R"(

        y = "hello";

    )";

    Lexer lexer(sourceCode);
    std::vector<Token> tokens;

    Token t;
    do {
        t = lexer.nextToken();
        tokens.push_back(t);
    } while (t.type != T_EOF);

    Parser parser(tokens);
    auto rpn = parser.parse();

    std::cout << "RPN: ";
    for (auto& tok : rpn) {
        std::cout << tok.value << " ";
    }

    return 0;
}