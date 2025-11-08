#include "lexer.h"

namespace lexer {

int Lexer::get_token() {
    static int last_char = ' ';

    // Skip whitespaces
    while (isspace(last_char))
        last_char = getchar();

    if (isalpha(last_char) || last_char == '_') {
        // Lexing Identifier or keyword
        this->identifier_str = "";
        this->identifier_str += last_char;
        while (isalnum(last_char = getchar()) || last_char == '_')
            this->identifier_str += last_char;

        if (this->identifier_str == "def") {
            return tok_def;
        }
        if (this->identifier_str == "extern") {
            return tok_extern;
        }
        return tok_identifier;
    }

    // Lexing an int or double
    if (isdigit(last_char) || last_char == '.') {
        std::string num_str;
        bool decimal_seen = false; // Track if a decimal point has been seen
        do {
            if (last_char == '.') {
                if (decimal_seen) {
                    // If a decimal has already been seen, this is an error or end of number
                    break; // Exit the loop, strtod will handle the valid part
                }
                decimal_seen = true;
            }
            num_str += last_char;
            last_char = getchar();
        } while (isdigit(last_char) || last_char == '.');

        this->num_val = strtod(num_str.c_str(), 0);
        return tok_number;
    }

    // Lex a comment
    if (last_char == '#') {
        do {
            last_char = getchar();
        } while (last_char != EOF && last_char != '\n' && last_char != '\r');

        if (last_char != EOF)
            return lexer::Lexer::get_token();
    }

    // Lex the end of a file
    if (last_char == EOF) {
        return tok_eof;
    }

    int this_char = last_char;
    last_char = getchar();
    return this_char;
}

} // namespace lexer
