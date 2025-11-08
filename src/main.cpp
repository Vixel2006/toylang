#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

#include "ast.h"
#include "lexer.h"

#include <iostream>

using namespace llvm;

static void MainLoop(ast::Parser &parser) {
    while (true) {
        fprintf(stderr, "ready> ");
        switch (parser.cur_token) {
        case lexer::tok_eof:
            return;
        case ';': // ignore top-level semicolons.
            parser.get_next_token();
            break;
        case lexer::tok_def:
            parser.handle_definition();
            break;
        case lexer::tok_extern:
            parser.handle_extern();
            break;
        default:
            parser.handle_topLevel_expression();
            break;
        }
    }
}

int main() {
    auto lex = std::make_unique<lexer::Lexer>();
    ast::Parser parser;
    parser.scanner = std::move(lex);

    parser.binop_precedence['<'] = 10;
    parser.binop_precedence['+'] = 20;
    parser.binop_precedence['-'] = 20;
    parser.binop_precedence['*'] = 40; // highest.

    // Prime the first token.
    fprintf(stderr, "ready> ");
    parser.get_next_token();

    // Run the main "interpreter loop" now.
    MainLoop(parser);
    return 0;
}
