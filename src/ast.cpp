#include "ast.h"

namespace ast {

int Parser::get_next_token() {
    return this->cur_token = scanner->get_token();
}

int Parser::get_token_precedence() {
    if (!isascii(this->cur_token))
        return -1;

    int token_prec = this->binop_precedence[this->cur_token];
    if (token_prec <= 0)
        return -1;
    return token_prec;
}

std::unique_ptr<ExprAST> Parser::log_error(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::log_error_proto(const char *Str) {
    fprintf(stderr, "Error: %s\n", Str);
    return nullptr;
}

std::unique_ptr<ExprAST> Parser::parse_number_expr() {
    auto Result = std::make_unique<NumberExprAST>(scanner->num_val);
    this->get_next_token();
    return std::move(Result);
}

std::unique_ptr<ExprAST> Parser::parse_paren_expr() {
    this->get_next_token();
    auto V = this->parse_expression();
    if (!V)
        return nullptr;

    if (this->cur_token != ')')
        return this->log_error("expected ')'");
    this->get_next_token();
    return V;
}

std::unique_ptr<ExprAST> Parser::parse_identifier_expr() {
    std::string id_name = scanner->identifier_str;

    this->get_next_token();

    if (this->cur_token != '(')
        return std::make_unique<VariableExprAST>(id_name);

    this->get_next_token();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (this->cur_token != ')') {
        while (true) {
            if (auto arg = this->parse_expression())
                args.push_back(std::move(arg));
            else
                return nullptr;

            if (this->cur_token == ')')
                break;

            if (this->cur_token != ',')
                return this->log_error("Expected ')' or ',' in argument list");
            this->get_next_token();
        }
    }

    this->get_next_token();

    return std::make_unique<CallExprAST>(id_name, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
    switch (this->cur_token) {
    default:
        this->log_error("unknown token when expecting an expression");
        this->get_next_token();
        return nullptr;
    case lexer::tok_identifier:
        return this->parse_identifier_expr();
    case lexer::tok_number:
        return this->parse_number_expr();
    case '(':
        return this->parse_paren_expr();
    }
}

std::unique_ptr<ExprAST> Parser::parse_binop_RHS(int expr_prec, std::unique_ptr<ast::ExprAST> LHS) {
    while (true) {
        int token_prec = this->get_token_precedence();

        if (token_prec < expr_prec)
            return LHS;

        int BinOp = this->cur_token;
        this->get_next_token();

        auto RHS = this->parse_primary();
        if (!RHS)
            return nullptr;

        int NextPrec = this->get_token_precedence();
        if (token_prec < NextPrec) {
            RHS = this->parse_binop_RHS(token_prec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<ast::BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));
    }
}

std::unique_ptr<ExprAST> Parser::parse_expression() {
    auto LHS = this->parse_primary();
    if (!LHS)
        return nullptr;

    return this->parse_binop_RHS(0, std::move(LHS));
}

std::unique_ptr<PrototypeAST> Parser::parse_prototype() {
    if (this->cur_token != lexer::tok_identifier)
        return this->log_error_proto("Expected function name in prototype");

    std::string fn_name = scanner->identifier_str;
    this->get_next_token(); // Consume function name

    if (this->cur_token != '(')
        return this->log_error_proto("Expected '(' in prototype");
    this->get_next_token(); // Consume '('

    std::vector<std::string> arg_names;
    while (this->cur_token == lexer::tok_identifier) { // Check cur_token directly
        arg_names.push_back(scanner->identifier_str);
        this->get_next_token(); // Consume argument identifier
    }

    if (this->cur_token != ')')
        return this->log_error_proto("Expected ')' in prototype");
    this->get_next_token(); // Consume ')'

    return std::make_unique<PrototypeAST>(fn_name, std::move(arg_names));
}

std::unique_ptr<FunctionAST> Parser::parse_definition() {
    this->get_next_token();
    auto proto = this->parse_prototype();
    if (!proto)
        return nullptr;

    if (auto E = this->parse_expression())
        return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
    return nullptr;
}

std::unique_ptr<FunctionAST> Parser::parse_topLevel_expr() {
    if (auto E = this->parse_expression()) {
        auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(E));
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::parse_extern() {
    this->get_next_token();
    return this->parse_prototype();
}

void Parser::handle_definition() {
    if (this->parse_definition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        this->get_next_token();
    }
}

void Parser::handle_extern() {
    if (this->parse_extern()) {
        fprintf(stderr, "Parsed an extern\n");
    } else {
        this->get_next_token();
    }
}

void Parser::handle_topLevel_expression() {
    if (this->parse_topLevel_expr()) {
        fprintf(stderr, "Parsed a top-level expr\n");
    } else {
        this->get_next_token();
    }
}

} // namespace ast
