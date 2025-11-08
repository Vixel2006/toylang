#pragma once

#include <string>

namespace lexer {

enum Token {
    tok_eof = -1,

    // commands
    tok_def = -2,
    tok_extern = -3,

    // primary
    tok_identifier = -4,
    tok_number = -5,
};

class Lexer {
  public:
    std::string identifier_str;
    double num_val;

    Lexer() {}
    ~Lexer() = default;

    int get_token();
};

} // namespace lexer
