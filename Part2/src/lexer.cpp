// lexer.cpp — owned by Trisha (Lexer & Tokens)

#include "lexer.h"
#include <cctype>

// Keyword table — maps the raw UTF-8 source word to a TokenType.
const std::unordered_map<std::string, TokenType> Lexer::KEYWORDS = {
    {"ধরি",     TokenType::KW_DECLARE},
    {"যদি",     TokenType::KW_IF},
    {"নাহলে",   TokenType::KW_ELSE},
    {"যতক্ষণ",  TokenType::KW_WHILE},
    {"দেখাও",   TokenType::KW_PRINT},
    {"সংখ্যা",  TokenType::KW_TYPE_INT},
    {"দশমিক",   TokenType::KW_TYPE_FLOAT},
};

// Bangla keywords are multi-byte UTF-8. Any byte >= 0x80 is treated as
// part of a "word" character so identifiers/keywords scan correctly;
// ASCII letters/digits/underscore are allowed too (identifiers in the
// grammar are Latin: letter (letter|digit|"_")* ).
static bool isWordStart(unsigned char c) {
    return std::isalpha(c) || c == '_' || c >= 0x80;
}
static bool isWordPart(unsigned char c) {
    return std::isalnum(c) || c == '_' || c >= 0x80;
}

Lexer::Lexer(const std::string& source, ErrorHandler& errHandler)
    : source_(source), err_(errHandler) {}

bool Lexer::isAtEnd() const { return pos_ >= source_.size(); }

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[pos_];
}

char Lexer::peekNext() const {
    if (pos_ + 1 >= source_.size()) return '\0';
    return source_[pos_ + 1];
}

char Lexer::advance() {
    char c = source_[pos_++];
    if (c == '\n') { line_++; column_ = 1; }
    else            { column_++; }
    return c;
}

bool Lexer::match(char expected) {
    if (isAtEnd() || source_[pos_] != expected) return false;
    advance();
    return true;
}

void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peekNext() == '/') {
            while (!isAtEnd() && peek() != '\n') advance();
        } else {
            break;
        }
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        tokens.push_back(nextToken());
    }
    tokens.push_back(Token{TokenType::END_OF_FILE, "", line_, column_});
    return tokens;
}

Token Lexer::nextToken() {
    int startLine = line_, startCol = column_;
    unsigned char c = static_cast<unsigned char>(advance());

    switch (c) {
        case '+': return {TokenType::OP_PLUS,   "+", startLine, startCol};
        case '-': return {TokenType::OP_MINUS,  "-", startLine, startCol};
        case '*': return {TokenType::OP_STAR,   "*", startLine, startCol};
        case '/': return {TokenType::OP_SLASH,  "/", startLine, startCol};
        case '(': return {TokenType::LPAREN,    "(", startLine, startCol};
        case ')': return {TokenType::RPAREN,    ")", startLine, startCol};
        case '{': return {TokenType::LBRACE,    "{", startLine, startCol};
        case '}': return {TokenType::RBRACE,    "}", startLine, startCol};
        case ';': return {TokenType::SEMICOLON, ";", startLine, startCol};
        case '=':
            if (match('=')) return {TokenType::OP_EQ, "==", startLine, startCol};
            return             {TokenType::OP_ASSIGN, "=", startLine, startCol};
        case '!':
            if (match('=')) return {TokenType::OP_NEQ, "!=", startLine, startCol};
            err_.lexicalError(startLine, startCol, "Unexpected '!'; did you mean '!='?");
            return {TokenType::UNKNOWN, "!", startLine, startCol};
        case '<':
            if (match('=')) return {TokenType::OP_LE, "<=", startLine, startCol};
            return             {TokenType::OP_LT, "<", startLine, startCol};
        case '>':
            if (match('=')) return {TokenType::OP_GE, ">=", startLine, startCol};
            return             {TokenType::OP_GT, ">", startLine, startCol};
        default: break;
    }

    if (std::isdigit(c)) {
        pos_--; column_ = startCol;
        return scanNumber();
    }

    if (isWordStart(c)) {
        pos_--; column_ = startCol;
        return scanIdentifierOrKeyword();
    }

    std::string msg = "Unknown character '";
    msg += static_cast<char>(c);
    msg += "'";
    err_.lexicalError(startLine, startCol, msg);
    return {TokenType::UNKNOWN, std::string(1, static_cast<char>(c)), startLine, startCol};
}

Token Lexer::scanNumber() {
    int startLine = line_, startCol = column_;
    std::string num;
    bool isFloat = false;

    while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
        num += advance();

    if (!isAtEnd() && peek() == '.' && std::isdigit(static_cast<unsigned char>(peekNext()))) {
        isFloat = true;
        num += advance(); // consume '.'
        while (!isAtEnd() && std::isdigit(static_cast<unsigned char>(peek())))
            num += advance();
    }

    return isFloat
        ? Token{TokenType::FLOAT_LITERAL, num, startLine, startCol}
        : Token{TokenType::INT_LITERAL,   num, startLine, startCol};
}

Token Lexer::scanIdentifierOrKeyword() {
    int startLine = line_, startCol = column_;
    std::string word;

    while (!isAtEnd() && isWordPart(static_cast<unsigned char>(peek())))
        word += advance();

    auto it = KEYWORDS.find(word);
    if (it != KEYWORDS.end())
        return {it->second, word, startLine, startCol};

    return {TokenType::IDENTIFIER, word, startLine, startCol};
}
