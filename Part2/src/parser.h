// parser.h — owned by Rahat (Grammar & Parser Lead)

#ifndef JUKTILANG_PARSER_H
#define JUKTILANG_PARSER_H

#include "token.h"
#include "ast.h"
#include "error.h"
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens, ErrorHandler& errHandler);

    // Returns the root PROGRAM node. On syntax errors, records them via
    // the shared ErrorHandler instead of throwing, and uses panic-mode
    // recovery (skip to next ';' or a statement-starting keyword) so the
    // whole file is still scanned for errors in one pass.
    ASTNodePtr parseProgram();

    bool hadError() const { return err_.hasErrors(); }

private:
    std::vector<Token> tokens_;
    ErrorHandler& err_;
    size_t current_ = 0;

    // ── Token navigation ──────────────────────────────────────────
    Token peek() const;
    Token previous() const;
    Token advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    Token expect(TokenType type, const std::string& errorMessage);
    void synchronize(); // panic-mode recovery

    // ── Grammar rules — mirror grammar/juktilang.bnf exactly ───────
    ASTNodePtr parseStatement();
    ASTNodePtr parseDeclaration();
    ASTNodePtr parseAssignment();
    ASTNodePtr parseIfStatement();
    ASTNodePtr parseWhileStatement();
    ASTNodePtr parsePrintStatement();
    std::vector<ASTNodePtr> parseBlock(); // "{" <statement>* "}"

    // ── Expression grammar, precedence-climbing ─────────────────────
    //   condition   ::= expression relop expression
    //   expression  ::= term ((+|-) term)*
    //   term        ::= factor ((*|/) factor)*
    //   factor      ::= number | identifier | "(" expression ")" | "-" factor
    ASTNodePtr parseCondition();
    ASTNodePtr parseExpression();
    ASTNodePtr parseTerm();
    ASTNodePtr parseFactor();

    bool isRelOp(TokenType t) const;
};

#endif // JUKTILANG_PARSER_H
