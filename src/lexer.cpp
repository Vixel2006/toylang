#include "lexer.h"

static int lexer::gettok() {
    static int LastChar = ' ';

    // Skip whitespaces
    while (isspace(LastChar))
        LastChar = getchar();

    if (isalpha(LastChar)) {
        // Lexing Identifier or keyword
        lexer::IdentifierStr = LastChar;
        while (isalnum(LastChar = getchar()))
            lexer::IdentifierStr += LastChar;

        if (lexer::IdentifierStr == "def")
            return lexer::tok_def;
        if (lexer::IdentifierStr == "extern")
            return lexer::tok_extern;
    }

    // Lexing an int or double
    if (isdigit(LastChar) || LastChar == '.') {
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        lexer::NumVal = strtod(NumStr.c_str(), 0);
        return lexer::tok_number;
    }

    // Lex a comment
    if (LastChar == '#') {
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return lexer::gettok();
    }

    // Lex the end of a file
    if (LastChar == EOF)
        return lexer::tok_eof;

    int ThisChar = LastChar;
    LastChar = getchar();

    return ThisChar;
}
