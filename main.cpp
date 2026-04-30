#include <iostream>
#include "lexer/Lexer.h"
#include "parser/Parser.h"


int main() {
    std::string sourceCode = R"(
        names[0] := "Alice";
        names[1] := "Hello world";
    )";

    Lexer lexer(sourceCode);
    std::vector<Token> tokens;

    Token t;
    do {
        t = lexer.nextToken();
        
        // Отладка: раскомментируйте, чтобы видеть, что дает лексер
        if (t.type != T_EOF) std::cout << "Lexed: '" << t.value << "' Type: " << t.type << std::endl;

        tokens.push_back(t);
    } while (t.type != T_EOF);

    Parser parser(tokens);
    auto rpn = parser.parse();

    std::cout << "RPN: ";
    for (auto& tok : rpn) {
        std::cout << tok.value << " ";
    }
    std::cout << std::endl;

    return 0;
}