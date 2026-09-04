// codegen.h — owned by Sonia (Code Generation, AST -> Python)

#ifndef JUKTILANG_CODEGEN_H
#define JUKTILANG_CODEGEN_H

#include "ast.h"
#include "error.h"
#include <string>
#include <sstream>

class CodeGenerator {
public:
    explicit CodeGenerator(ErrorHandler& errHandler);

    // Returns generated Python source as a string. Assumes the AST has
    // already passed semantic analysis (resolvedType fields filled in).
    std::string generate(const ASTNodePtr& program);

private:
    ErrorHandler& err_;
    std::ostringstream out_;
    int indentLevel_ = 0;

    void writeLine(const std::string& code);
    std::string indent() const;

    void genStatement(const ASTNodePtr& node);
    void genBlock(const std::vector<ASTNodePtr>& stmts);
    void genDeclaration(const ASTNodePtr& node);
    void genAssignment(const ASTNodePtr& node);
    void genIfStatement(const ASTNodePtr& node);
    void genWhileStatement(const ASTNodePtr& node);
    void genPrintStatement(const ASTNodePtr& node);

    // Expression generation -> returns a Python expression string.
    std::string genExpression(const ASTNodePtr& node);
    std::string formatNumber(const ASTNodePtr& node) const;
};

#endif // JUKTILANG_CODEGEN_H
