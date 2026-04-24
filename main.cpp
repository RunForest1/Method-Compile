#include <iostream>
#include "lexer/Lexer.h"

int main() {
    std::string sourceCode = R"(
        READ n;
        a = 10 + 3.14 * b;
        PRINT a;
    )";

    Lexer lexer(sourceCode);
    Token token;

    do {
        token = lexer.nextToken();

        std::cout << token.type << " : " << token.value << std::endl;

    } while (token.type != T_EOF);

    return 0;
}