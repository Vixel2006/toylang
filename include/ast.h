#pragma once

#include "lexer.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ast {

// Parse basic language experssions
class ExprAST {
  public:
    virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {
    double Val;

  public:
    NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {
    std::string Name;

  public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {
    char Op;

    std::unique_ptr<ExprAST> LHS, RHS;

  public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

  public:
    CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

// Parse function prototypes
class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;

  public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}

    const std::string &getName() const {
        return Name;
    }
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::unique_ptr<ExprAST> Body;

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

class Parser {
  public:
    int cur_token;
    std::map<char, int> binop_precedence;
    std::unique_ptr<lexer::Lexer> scanner;

    Parser() {}
    ~Parser() = default;

    int get_next_token();
    int get_token_precedence();

    std::unique_ptr<ExprAST> log_error(const char *Str);
    std::unique_ptr<PrototypeAST> log_error_proto(const char *Str);

    std::unique_ptr<ExprAST> parse_expression();

    std::unique_ptr<ExprAST> parse_number_expr();
    std::unique_ptr<ExprAST> parse_paren_expr();
    std::unique_ptr<ExprAST> parse_identifier_expr();
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_binop_RHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);

    std::unique_ptr<PrototypeAST> parse_prototype();
    std::unique_ptr<FunctionAST> parse_definition();
    std::unique_ptr<FunctionAST> parse_topLevel_expr();
    std::unique_ptr<PrototypeAST> parse_extern();

    void handle_definition();
    void handle_extern();
    void handle_topLevel_expression();
};

} // namespace ast
