// token.h — owned by Trisha (Lexer & Tokens)
// Finalized in Phase 0. Matches grammar/juktilang.bnf.

#ifndef JUKTILANG_TOKEN_H
#define JUKTILANG_TOKEN_H

#include <string>

enum class TokenType {
    // Keywords
    KW_DECLARE,   // ধরি
    KW_IF,        // যদি
    KW_ELSE,      // নাহলে
    KW_WHILE,     // যতক্ষণ
    KW_PRINT,     // দেখাও
    KW_TYPE_INT,  // সংখ্যা
    KW_TYPE_FLOAT,// দশমিক

    // Literals & identifiers
    IDENTIFIER,
    INT_LITERAL,
    FLOAT_LITERAL,

    // Operators
    OP_PLUS, OP_MINUS, OP_STAR, OP_SLASH,
    OP_ASSIGN,      // =
    OP_EQ, OP_NEQ,  // == !=
    OP_LT, OP_GT, OP_LE, OP_GE,

    // Punctuation
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    SEMICOLON,

    // Control
    END_OF_FILE,
    UNKNOWN // lexer emits this + an error for anything it can't classify
};

// Human-readable name, used in --tokens dumps and error messages.
inline std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case TokenType::KW_DECLARE:    return "KW_DECLARE";
        case TokenType::KW_IF:         return "KW_IF";
        case TokenType::KW_ELSE:       return "KW_ELSE";
        case TokenType::KW_WHILE:      return "KW_WHILE";
        case TokenType::KW_PRINT:      return "KW_PRINT";
        case TokenType::KW_TYPE_INT:   return "KW_TYPE_INT";
        case TokenType::KW_TYPE_FLOAT: return "KW_TYPE_FLOAT";
        case TokenType::IDENTIFIER:    return "IDENTIFIER";
        case TokenType::INT_LITERAL:   return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::OP_PLUS:       return "OP_PLUS";
        case TokenType::OP_MINUS:      return "OP_MINUS";
        case TokenType::OP_STAR:       return "OP_STAR";
        case TokenType::OP_SLASH:      return "OP_SLASH";
        case TokenType::OP_ASSIGN:     return "OP_ASSIGN";
        case TokenType::OP_EQ:         return "OP_EQ";
        case TokenType::OP_NEQ:        return "OP_NEQ";
        case TokenType::OP_LT:         return "OP_LT";
        case TokenType::OP_GT:         return "OP_GT";
        case TokenType::OP_LE:         return "OP_LE";
        case TokenType::OP_GE:         return "OP_GE";
        case TokenType::LPAREN:        return "LPAREN";
        case TokenType::RPAREN:        return "RPAREN";
        case TokenType::LBRACE:        return "LBRACE";
        case TokenType::RBRACE:        return "RBRACE";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::END_OF_FILE:   return "EOF";
        case TokenType::UNKNOWN:       return "UNKNOWN";
        default:                       return "???";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;  // raw text, e.g. "3.14", "x", "যদি"
    int line;
    int column;

    std::string toString() const {
        return "[" + tokenTypeToString(type) + " '" + lexeme + "' L" +
               std::to_string(line) + ":C" + std::to_string(column) + "]";
    }
};

#endif // JUKTILANG_TOKEN_H
