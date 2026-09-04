// semantic.h — owned by Reshmi (Semantic Analysis)

#ifndef JUKTILANG_SEMANTIC_H
#define JUKTILANG_SEMANTIC_H

#include "ast.h"
#include "error.h"
#include <string>
#include <unordered_map>

struct SymbolInfo {
    DataType type;
    int declaredLine;
};

// SemanticAnalyzer — single global scope (confirmed in Phase 0: if/while
// bodies do not introduce new scopes for the minimum feature set).
class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(ErrorHandler& errHandler);

    // Walks the AST, fills in resolvedType on expression nodes, and
    // reports through the shared ErrorHandler instead of throwing.
    void analyze(const ASTNodePtr& program);

    const std::unordered_map<std::string, SymbolInfo>& symbolTable() const { return symbolTable_; }

private:
    ErrorHandler& err_;
    std::unordered_map<std::string, SymbolInfo> symbolTable_;

    void visitStatement(const ASTNodePtr& node);
    void visitDeclaration(const ASTNodePtr& node);
    void visitAssignment(const ASTNodePtr& node);
    void visitIfStatement(const ASTNodePtr& node);
    void visitWhileStatement(const ASTNodePtr& node);
    void visitPrintStatement(const ASTNodePtr& node);

    // Returns the resolved type of an expression subtree and stamps it
    // onto node->resolvedType.
    DataType visitExpression(const ASTNodePtr& node);

    bool isDeclared(const std::string& name) const;
    void declare(const std::string& name, DataType type, int line);
    DataType typeOf(const std::string& name) const;
};

#endif // JUKTILANG_SEMANTIC_H
