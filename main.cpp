#include <iostream>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"

void printOPS(const Program& prog) {
    std::cout << "=== REVERSE POLISH NOTATION (OPS) ===" << std::endl;
    for (size_t i = 0; i < prog.size(); i++) {
        const auto& instr = prog[i];
        std::cout << "[" << i << "] ";
        
        switch (instr.op) {
            case OpType::OP_PUSH_INT:
                std::cout << "PUSH_INT " << instr.arg;
                break;
            case OpType::OP_PUSH_FLOAT:
                std::cout << "PUSH_FLOAT " << instr.arg;
                break;
            case OpType::OP_PUSH_STRING:
                std::cout << "PUSH_STRING " << instr.arg;
                break;
            case OpType::OP_PUSH_VAR:
                std::cout << "PUSH_VAR " << instr.arg;
                break;
            case OpType::OP_POP_VAR:
                std::cout << "POP_VAR " << instr.arg;
                break;
            case OpType::OP_ADD:
                std::cout << "ADD";
                break;
            case OpType::OP_SUB:
                std::cout << "SUB";
                break;
            case OpType::OP_MUL:
                std::cout << "MUL";
                break;
            case OpType::OP_DIV:
                std::cout << "DIV";
                break;
            case OpType::OP_NEG:
                std::cout << "NEG";
                break;
            case OpType::OP_LT:
                std::cout << "LT";
                break;
            case OpType::OP_GT:
                std::cout << "GT";
                break;
            case OpType::OP_LE:
                std::cout << "LE";
                break;
            case OpType::OP_GE:
                std::cout << "GE";
                break;
            case OpType::OP_EQ:
                std::cout << "EQ";
                break;
            case OpType::OP_NE:
                std::cout << "NE";
                break;
            case OpType::OP_AND:
                std::cout << "AND";
                break;
            case OpType::OP_OR:
                std::cout << "OR";
                break;
            case OpType::OP_NOT:
                std::cout << "NOT";
                break;
            case OpType::OP_PRINT:
                std::cout << "PRINT";
                break;
            case OpType::OP_READ:
                std::cout << "READ";
                break;
            case OpType::OP_JMP:
                std::cout << "JMP " << instr.jumpTarget;
                break;
            case OpType::OP_JMP_FALSE:
                std::cout << "JMP_FALSE " << instr.jumpTarget;
                break;
            case OpType::OP_END:
                std::cout << "END";
                break;
            default:
                std::cout << "UNKNOWN";
        }
        std::cout << std::endl;
    }
}

int main() {
    std::string sourceCode = R"(
        READ n;
        a = 10 + 3.14 * 2;
        PRINT a;
    )";

    std::cout << "=== LEXICAL ANALYSIS ===" << std::endl;
    Lexer lexer(sourceCode);
    Token token;
    int tokenCount = 0;

    do {
        token = lexer.nextToken();
        if (token.type != T_EOF) {
            tokenCount++;
            std::cout << "Token [" << token.type << "]: '" << token.value << "'" 
                      << " at (" << token.line << "," << token.column << ")" << std::endl;
        }
    } while (token.type != T_EOF);

    std::cout << "\nTotal tokens: " << tokenCount << std::endl;

    std::cout << "\n=== SYNTAX ANALYSIS ===" << std::endl;

    Parser parser(sourceCode);
    auto ast = parser.parse();

    if (parser.hasErrors()) {
        std::cout << "Parsing errors:" << std::endl;
        for (const auto& err : parser.getErrors()) {
            std::cout << "  " << err << std::endl;
        }
        return 1;
    }

    Program ops = parser.getOPS();
    printOPS(ops);

    std::cout << "\n=== EXECUTION ===" << std::endl;
    try {
        Interpreter interpreter(ops);
        interpreter.execute();
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << std::endl;
    return 0;
}
