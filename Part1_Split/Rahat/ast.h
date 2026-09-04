// ast.h — THE AST CONTRACT
//
// This file is the interface between Rahat's parser (produces these nodes)
// and Sonia's codegen (consumes these nodes). Reshmi's semantic analyzer
// also walks this same tree. This shape was frozen after Phase 0 and did
// not change for the rest of the project — every phase (lexer, parser,
// semantic, codegen) targets exactly this struct.

#ifndef JUKTILANG_AST_H
#define JUKTILANG_AST_H

#include <string>
#include <vector>
#include <memory>

enum class DataType {
    INT,     // সংখ্যা
    FLOAT,   // দশমিক
    UNKNOWN  // used before type-checking assigns a real type
};

inline std::string dataTypeToString(DataType t) {
    switch (t) {
        case DataType::INT:   return "সংখ্যা (int)";
        case DataType::FLOAT: return "দশমিক (float)";
        default:              return "unknown";
    }
}

enum class NodeType {
    PROGRAM,
    DECLARATION,
    ASSIGNMENT,
    IF_STATEMENT,
    WHILE_STATEMENT,
    PRINT_STATEMENT,
    BINARY_EXPR,     // +  -  *  /
    RELATIONAL_EXPR, // == != < > <= >=
    UNARY_EXPR,      // unary minus
    NUMBER_LITERAL,
    IDENTIFIER
};

struct ASTNode {
    NodeType type;

    // Position info — needed for good error messages (Trisha's lexer
    // tracks this; Rahat's parser propagates it onto every node).
    int line = 0;
    int column = 0;

    // Generic children (statement lists inside PROGRAM / IF / WHILE bodies)
    std::vector<std::shared_ptr<ASTNode>> children;

    // --- Fields used depending on node type ---

    // DECLARATION / ASSIGNMENT / IDENTIFIER
    std::string identifierName;
    DataType declaredType = DataType::UNKNOWN; // set on DECLARATION

    // NUMBER_LITERAL
    double numberValue = 0.0;
    DataType literalType = DataType::UNKNOWN; // INT or FLOAT

    // BINARY_EXPR / RELATIONAL_EXPR / UNARY_EXPR
    std::string op; // "+", "-", "*", "/", "==", "<", etc.
    std::shared_ptr<ASTNode> left;   // also used as the single operand for UNARY_EXPR
    std::shared_ptr<ASTNode> right;

    // DECLARATION / ASSIGNMENT — the right-hand-side expression (may be
    // null for a bare declaration like "ধরি সংখ্যা x;")
    std::shared_ptr<ASTNode> valueExpr;

    // IF_STATEMENT / WHILE_STATEMENT
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> thenBody;
    std::vector<std::shared_ptr<ASTNode>> elseBody; // empty if no নাহলে

    // PRINT_STATEMENT
    std::shared_ptr<ASTNode> printExpr;

    // Semantic analysis fills this in after type-checking (used by
    // codegen to decide things like float formatting).
    DataType resolvedType = DataType::UNKNOWN;
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

#endif // JUKTILANG_AST_H
