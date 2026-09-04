// semantic.cpp — owned by Reshmi (Semantic Analysis)
//
// Coercion rule agreed in Phase 0: দশমিক (float) values may not be
// silently narrowed into সংখ্যা (int) variables — that's a semantic
// error, not an implicit cast. int -> float is always safe and never
// flagged. Division ("/") always produces দশমিক, matching how the
// codegen team wanted division to behave (mirrors Python 3's "/").

#include "semantic.h"

SemanticAnalyzer::SemanticAnalyzer(ErrorHandler& errHandler) : err_(errHandler) {}

bool SemanticAnalyzer::isDeclared(const std::string& name) const {
    return symbolTable_.find(name) != symbolTable_.end();
}

void SemanticAnalyzer::declare(const std::string& name, DataType type, int line) {
    symbolTable_[name] = {type, line};
}

DataType SemanticAnalyzer::typeOf(const std::string& name) const {
    auto it = symbolTable_.find(name);
    return it != symbolTable_.end() ? it->second.type : DataType::UNKNOWN;
}

void SemanticAnalyzer::analyze(const ASTNodePtr& program) {
    if (!program) return;
    for (const auto& stmt : program->children) {
        visitStatement(stmt);
    }
}

void SemanticAnalyzer::visitStatement(const ASTNodePtr& node) {
    if (!node) return;

    switch (node->type) {
        case NodeType::DECLARATION:      visitDeclaration(node);      break;
        case NodeType::ASSIGNMENT:       visitAssignment(node);       break;
        case NodeType::IF_STATEMENT:     visitIfStatement(node);      break;
        case NodeType::WHILE_STATEMENT:  visitWhileStatement(node);   break;
        case NodeType::PRINT_STATEMENT:  visitPrintStatement(node);   break;
        default:
            err_.semanticError(node->line, node->column,
                "Internal error: unexpected node in statement position");
    }
}

// ধরি <type> <id> [= <expr>];
void SemanticAnalyzer::visitDeclaration(const ASTNodePtr& node) {
    if (isDeclared(node->identifierName)) {
        err_.semanticError(node->line, node->column,
            "Variable '" + node->identifierName + "' is already declared.");
        return;
    }

    if (node->valueExpr) {
        DataType initType = visitExpression(node->valueExpr);
        if (node->declaredType == DataType::INT && initType == DataType::FLOAT) {
            err_.semanticError(node->line, node->column,
                "Type mismatch: cannot assign দশমিক (float) value to "
                "সংখ্যা (int) variable '" + node->identifierName + "'.");
        }
    }

    declare(node->identifierName, node->declaredType, node->line);
}

// <id> = <expr>;
void SemanticAnalyzer::visitAssignment(const ASTNodePtr& node) {
    if (!isDeclared(node->identifierName)) {
        err_.semanticError(node->line, node->column,
            "Undeclared variable '" + node->identifierName + "'.");
        // Still analyze the RHS so later errors in the same expression
        // are reported too.
        visitExpression(node->valueExpr);
        return;
    }

    DataType varType = typeOf(node->identifierName);
    DataType exprType = visitExpression(node->valueExpr);

    if (varType == DataType::INT && exprType == DataType::FLOAT) {
        err_.semanticError(node->line, node->column,
            "Type mismatch: cannot assign দশমিক (float) expression to "
            "সংখ্যা (int) variable '" + node->identifierName + "'.");
    }
}

// যদি (cond) { ... } [নাহলে { ... }]
void SemanticAnalyzer::visitIfStatement(const ASTNodePtr& node) {
    if (node->condition) visitExpression(node->condition);
    for (const auto& s : node->thenBody) visitStatement(s);
    for (const auto& s : node->elseBody) visitStatement(s);
}

// যতক্ষণ (cond) { ... }
void SemanticAnalyzer::visitWhileStatement(const ASTNodePtr& node) {
    if (node->condition) visitExpression(node->condition);
    for (const auto& s : node->thenBody) visitStatement(s);
}

// দেখাও(expr);
void SemanticAnalyzer::visitPrintStatement(const ASTNodePtr& node) {
    if (node->printExpr) visitExpression(node->printExpr);
}

// ══════════════════════════════════════════════════════════════════
//  Expression type resolution
// ══════════════════════════════════════════════════════════════════

DataType SemanticAnalyzer::visitExpression(const ASTNodePtr& node) {
    if (!node) return DataType::UNKNOWN;

    switch (node->type) {
        case NodeType::NUMBER_LITERAL:
            node->resolvedType = node->literalType;
            return node->resolvedType;

        case NodeType::IDENTIFIER: {
            if (!isDeclared(node->identifierName)) {
                err_.semanticError(node->line, node->column,
                    "Undeclared variable '" + node->identifierName + "'.");
                node->resolvedType = DataType::UNKNOWN;
                return DataType::UNKNOWN;
            }
            node->resolvedType = typeOf(node->identifierName);
            return node->resolvedType;
        }

        case NodeType::UNARY_EXPR: {
            DataType t = visitExpression(node->left);
            node->resolvedType = t;
            return t;
        }

        case NodeType::BINARY_EXPR: {
            DataType lt = visitExpression(node->left);
            DataType rt = visitExpression(node->right);

            DataType result = (lt == DataType::FLOAT || rt == DataType::FLOAT)
                                   ? DataType::FLOAT
                                   : DataType::INT;

            // Division always promotes to দশমিক (float), same convention
            // codegen relies on when emitting Python.
            if (node->op == "/") result = DataType::FLOAT;

            node->resolvedType = result;
            return result;
        }

        case NodeType::RELATIONAL_EXPR: {
            visitExpression(node->left);
            visitExpression(node->right);
            // Comparisons produce a boolean-like সংখ্যা (0 or 1), matching
            // how conditions feed directly into যদি/যতক্ষণ.
            node->resolvedType = DataType::INT;
            return DataType::INT;
        }

        default:
            err_.semanticError(node->line, node->column,
                "Internal error: unexpected node in expression position");
            return DataType::UNKNOWN;
    }
}
