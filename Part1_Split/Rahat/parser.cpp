// parser.cpp — owned by Rahat (Grammar & Parser Lead)

#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens, ErrorHandler& errHandler)
    : tokens_(tokens), err_(errHandler) {}

// ------------------------------------------------------------------
// Token navigation
// ------------------------------------------------------------------
Token Parser::peek() const { return tokens_[current_]; }
Token Parser::previous() const { return tokens_[current_ > 0 ? current_ - 1 : 0]; }
bool  Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return type == TokenType::END_OF_FILE;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

Token Parser::expect(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    Token tok = peek();
    err_.syntaxError(tok.line, tok.column,
        errorMessage + " (got '" + tok.lexeme + "')");
    // Unwind to the nearest statement loop instead of limping forward
    // with a malformed node — that's what caused duplicate/cascading
    // errors from a single mistake during early testing.
    throw std::runtime_error("syntax error");
}

bool Parser::isRelOp(TokenType t) const {
    return t == TokenType::OP_EQ || t == TokenType::OP_NEQ ||
           t == TokenType::OP_LT || t == TokenType::OP_GT  ||
           t == TokenType::OP_LE || t == TokenType::OP_GE;
}

// ------------------------------------------------------------------
// synchronize() — panic-mode recovery
// Skip tokens until we pass a ';' or land on '}' or a token that starts
// a new statement, so one bad line doesn't abort the whole parse.
// ------------------------------------------------------------------
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        if (check(TokenType::RBRACE)) return;

        switch (peek().type) {
            case TokenType::KW_DECLARE:
            case TokenType::KW_IF:
            case TokenType::KW_WHILE:
            case TokenType::KW_PRINT:
                return;
            default:
                break;
        }
        advance();
    }
}

// ------------------------------------------------------------------
// parseProgram — entry point
// ------------------------------------------------------------------
ASTNodePtr Parser::parseProgram() {
    auto root = std::make_shared<ASTNode>();
    root->type = NodeType::PROGRAM;

    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) root->children.push_back(stmt);
        } catch (const std::runtime_error&) {
            synchronize();
        }
    }
    return root;
}

// ------------------------------------------------------------------
// parseStatement — dispatch on the lookahead token
// ------------------------------------------------------------------
ASTNodePtr Parser::parseStatement() {
    if (check(TokenType::KW_DECLARE)) return parseDeclaration();
    if (check(TokenType::KW_IF))      return parseIfStatement();
    // যতক্ষণ (while) parsing is implemented below (parseWhileStatement)
    // but intentionally not wired into parseStatement() yet — looping
    // control is a Part 2 feature per the review plan.
    if (check(TokenType::KW_PRINT))   return parsePrintStatement();
    if (check(TokenType::IDENTIFIER)) return parseAssignment();

    Token tok = peek();
    err_.syntaxError(tok.line, tok.column,
        "Unexpected token '" + tok.lexeme + "'. Expected a statement.");
    throw std::runtime_error("syntax error");
}

// ------------------------------------------------------------------
// parseDeclaration — "ধরি" <type> <identifier> [ "=" <expression> ] ";"
// ------------------------------------------------------------------
ASTNodePtr Parser::parseDeclaration() {
    Token kw = expect(TokenType::KW_DECLARE, "Expected 'ধরি'");

    DataType dtype = DataType::UNKNOWN;
    if (match(TokenType::KW_TYPE_INT))        dtype = DataType::INT;
    else if (match(TokenType::KW_TYPE_FLOAT)) dtype = DataType::FLOAT;
    else {
        Token t = peek();
        err_.syntaxError(t.line, t.column,
            "Expected type keyword 'সংখ্যা' or 'দশমিক' after 'ধরি'");
        return nullptr;
    }

    Token nameTok = expect(TokenType::IDENTIFIER, "Expected variable name after type");

    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::DECLARATION;
    node->line = kw.line;
    node->column = kw.column;
    node->identifierName = nameTok.lexeme;
    node->declaredType = dtype;

    // Bare declaration ("ধরি সংখ্যা x;") is allowed by the grammar.
    if (match(TokenType::OP_ASSIGN)) {
        node->valueExpr = parseExpression();
    }

    expect(TokenType::SEMICOLON, "Expected ';' after declaration");
    return node;
}

// ------------------------------------------------------------------
// parseAssignment — <identifier> "=" <expression> ";"
// ------------------------------------------------------------------
ASTNodePtr Parser::parseAssignment() {
    Token nameTok = expect(TokenType::IDENTIFIER, "Expected variable name");
    expect(TokenType::OP_ASSIGN, "Expected '=' in assignment");
    auto value = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after assignment");

    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::ASSIGNMENT;
    node->line = nameTok.line;
    node->column = nameTok.column;
    node->identifierName = nameTok.lexeme;
    node->valueExpr = value;
    return node;
}

// ------------------------------------------------------------------
// parseIfStatement — "যদি" "(" <condition> ")" "{" <stmt>* "}"
//                     [ "নাহলে" "{" <stmt>* "}" ]
// ------------------------------------------------------------------
ASTNodePtr Parser::parseIfStatement() {
    Token kw = expect(TokenType::KW_IF, "Expected 'যদি'");
    expect(TokenType::LPAREN, "Expected '(' after 'যদি'");
    auto cond = parseCondition();
    expect(TokenType::RPAREN, "Expected ')' after condition");

    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::IF_STATEMENT;
    node->line = kw.line;
    node->column = kw.column;
    node->condition = cond;
    node->thenBody = parseBlock();

    if (match(TokenType::KW_ELSE)) {
        node->elseBody = parseBlock();
    }
    return node;
}

// ------------------------------------------------------------------
// parseWhileStatement — "যতক্ষণ" "(" <condition> ")" "{" <stmt>* "}"
// ------------------------------------------------------------------
ASTNodePtr Parser::parseWhileStatement() {
    Token kw = expect(TokenType::KW_WHILE, "Expected 'যতক্ষণ'");
    expect(TokenType::LPAREN, "Expected '(' after 'যতক্ষণ'");
    auto cond = parseCondition();
    expect(TokenType::RPAREN, "Expected ')' after condition");

    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::WHILE_STATEMENT;
    node->line = kw.line;
    node->column = kw.column;
    node->condition = cond;
    node->thenBody = parseBlock();
    return node;
}

// ------------------------------------------------------------------
// parsePrintStatement — "দেখাও" "(" <expression> ")" ";"
// ------------------------------------------------------------------
ASTNodePtr Parser::parsePrintStatement() {
    Token kw = expect(TokenType::KW_PRINT, "Expected 'দেখাও'");
    expect(TokenType::LPAREN, "Expected '(' after 'দেখাও'");
    auto expr = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after expression");
    expect(TokenType::SEMICOLON, "Expected ';' after 'দেখাও(...)'");

    auto node = std::make_shared<ASTNode>();
    node->type = NodeType::PRINT_STATEMENT;
    node->line = kw.line;
    node->column = kw.column;
    node->printExpr = expr;
    return node;
}

// ------------------------------------------------------------------
// parseBlock — "{" <statement>* "}"
// ------------------------------------------------------------------
std::vector<ASTNodePtr> Parser::parseBlock() {
    std::vector<ASTNodePtr> stmts;
    expect(TokenType::LBRACE, "Expected '{' to begin block");

    while (!isAtEnd() && !check(TokenType::RBRACE)) {
        try {
            auto stmt = parseStatement();
            if (stmt) stmts.push_back(stmt);
        } catch (const std::runtime_error&) {
            synchronize();
        }
    }

    expect(TokenType::RBRACE, "Expected '}' to end block");
    return stmts;
}

// ══════════════════════════════════════════════════════════════════
//  Expression / condition parsing
// ══════════════════════════════════════════════════════════════════

// condition ::= expression relop expression
ASTNodePtr Parser::parseCondition() {
    auto left = parseExpression();

    if (isRelOp(peek().type)) {
        Token opTok = advance();
        auto right = parseExpression();

        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::RELATIONAL_EXPR;
        node->line = opTok.line;
        node->column = opTok.column;
        node->op = opTok.lexeme;
        node->left = left;
        node->right = right;
        return node;
    }

    // Grammar requires a relop, but degrade gracefully instead of
    // crashing if a condition is malformed.
    Token t = peek();
    err_.syntaxError(t.line, t.column,
        "Expected a comparison operator (==, !=, <, >, <=, >=) in condition");
    return left;
}

// expression ::= term ((+|-) term)*
ASTNodePtr Parser::parseExpression() {
    auto left = parseTerm();

    while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
        Token opTok = advance();
        auto right = parseTerm();

        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_EXPR;
        node->line = opTok.line;
        node->column = opTok.column;
        node->op = opTok.lexeme;
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

// term ::= factor ((*|/) factor)*
ASTNodePtr Parser::parseTerm() {
    auto left = parseFactor();

    while (check(TokenType::OP_STAR) || check(TokenType::OP_SLASH)) {
        Token opTok = advance();
        auto right = parseFactor();

        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::BINARY_EXPR;
        node->line = opTok.line;
        node->column = opTok.column;
        node->op = opTok.lexeme;
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

// factor ::= number | identifier | "(" expression ")" | "-" factor
ASTNodePtr Parser::parseFactor() {
    if (check(TokenType::OP_MINUS)) {
        Token opTok = advance();
        auto operand = parseFactor();

        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::UNARY_EXPR;
        node->line = opTok.line;
        node->column = opTok.column;
        node->op = "-";
        node->left = operand;
        return node;
    }

    if (check(TokenType::INT_LITERAL)) {
        Token t = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::NUMBER_LITERAL;
        node->line = t.line;
        node->column = t.column;
        node->numberValue = std::stod(t.lexeme);
        node->literalType = DataType::INT;
        return node;
    }

    if (check(TokenType::FLOAT_LITERAL)) {
        Token t = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::NUMBER_LITERAL;
        node->line = t.line;
        node->column = t.column;
        node->numberValue = std::stod(t.lexeme);
        node->literalType = DataType::FLOAT;
        return node;
    }

    if (check(TokenType::IDENTIFIER)) {
        Token t = advance();
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::IDENTIFIER;
        node->line = t.line;
        node->column = t.column;
        node->identifierName = t.lexeme;
        return node;
    }

    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    Token t = peek();
    err_.syntaxError(t.line, t.column,
        "Expected an expression but got '" + t.lexeme + "'");
    throw std::runtime_error("syntax error");
}
