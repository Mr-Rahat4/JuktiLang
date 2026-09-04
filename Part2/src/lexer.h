// lexer.h — owned by Trisha (Lexer & Tokens)

#ifndef JUKTILANG_LEXER_H
#define JUKTILANG_LEXER_H

#include "token.h"
#include "error.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string& source, ErrorHandler& errHandler);

    // Tokenizes the whole input and returns the token stream, ending in
    // END_OF_FILE. This is what Rahat's parser calls.
    std::vector<Token> tokenize();

private:
    std::string source_;
    ErrorHandler& err_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;

    // Keyword lookup table — raw UTF-8 keyword string -> TokenType.
    static const std::unordered_map<std::string, TokenType> KEYWORDS;

    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);

    void skipWhitespaceAndComments();
    Token nextToken();
    Token scanNumber();
    Token scanIdentifierOrKeyword();
};

#endif // JUKTILANG_LEXER_H
