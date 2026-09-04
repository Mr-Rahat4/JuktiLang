# JuktiLang — UML & Architecture Diagrams
## CSE-4114 Compiler Design and Construction Sessional

---

## 1. Compiler Pipeline (Activity Diagram)

```mermaid
flowchart TD
    A[".jukti source file"] --> B["Lexer (Trisha)\ntokenize()"]
    B -->|token stream| C["Parser (Rahat)\nparseProgram()"]
    C -->|AST : ASTNodePtr| D["SemanticAnalyzer (Reshmi)\nanalyze()"]
    D -->|type-checked AST| E["CodeGenerator (Sonia)\ngenerate()"]
    E -->|Python source string| F[".py output file"]

    B -.->|lexicalError| G["ErrorHandler\n(shared)"]
    C -.->|syntaxError| G
    D -.->|semanticError| G
    G -.->|printAll + abort| H["main.cpp\n(Samiya, integration)"]
```

Each phase reports through the same `ErrorHandler` instead of throwing past `main()`; `main.cpp` checks `err.hasErrors()` after every phase and stops before running a later phase on bad input.

---

## 2. Class / Struct Diagram

```mermaid
classDiagram
    class Token {
        +TokenType type
        +string lexeme
        +int line
        +int column
        +toString() string
    }

    class Lexer {
        -string source_
        -ErrorHandler err_
        -size_t pos_
        -int line_
        -int column_
        +tokenize() vector~Token~
    }

    class Parser {
        -vector~Token~ tokens_
        -ErrorHandler err_
        -size_t current_
        +parseProgram() ASTNodePtr
        -parseStatement() ASTNodePtr
        -parseDeclaration() ASTNodePtr
        -parseIfStatement() ASTNodePtr
        -parseWhileStatement() ASTNodePtr
        -parseExpression() ASTNodePtr
        -synchronize() void
    }

    class ASTNode {
        +NodeType type
        +int line
        +int column
        +vector~ASTNodePtr~ children
        +string identifierName
        +DataType declaredType
        +double numberValue
        +string op
        +ASTNodePtr left
        +ASTNodePtr right
        +ASTNodePtr valueExpr
        +ASTNodePtr condition
        +vector~ASTNodePtr~ thenBody
        +vector~ASTNodePtr~ elseBody
        +ASTNodePtr printExpr
        +DataType resolvedType
    }

    class SemanticAnalyzer {
        -ErrorHandler err_
        -unordered_map~string,SymbolInfo~ symbolTable_
        +analyze(ASTNodePtr) void
        -visitExpression(ASTNodePtr) DataType
    }

    class CodeGenerator {
        -ErrorHandler err_
        -ostringstream out_
        -int indentLevel_
        +generate(ASTNodePtr) string
        -genExpression(ASTNodePtr) string
    }

    class ErrorHandler {
        +vector~CompilerError~ errors
        +lexicalError(int, int, string) void
        +syntaxError(int, int, string) void
        +semanticError(int, int, string) void
        +hasErrors() bool
        +printAll() void
    }

    Lexer --> Token : produces
    Parser --> Token : consumes
    Parser --> ASTNode : builds
    SemanticAnalyzer --> ASTNode : walks + annotates
    CodeGenerator --> ASTNode : walks
    Lexer --> ErrorHandler
    Parser --> ErrorHandler
    SemanticAnalyzer --> ErrorHandler
    CodeGenerator --> ErrorHandler
```

---

## 3. Why One `ASTNode` Struct Instead of a Class Hierarchy

An alternative design (used by some other teams) gives every node kind its own class — `DeclarationNode`, `IfNode`, `BinaryOpNode`, etc. — connected through a polymorphic base pointer. JuktiLang deliberately uses one flat `ASTNode` struct with a `NodeType` tag instead, because:

- **One header, frozen once.** The whole team could agree on `ast.h` in a single Phase 0 meeting and never touch it again — a class-per-node design would need every new node type coordinated across the person who parses it, the person who type-checks it, and the person who generates code from it.
- **No RTTI / `dynamic_cast` needed.** Every visitor (semantic, codegen) dispatches on a plain `switch (node->type)`, which is easier to get exhaustiveness-checked and to debug under time pressure.
- **Trade-off, acknowledged.** The struct carries fields that are irrelevant for most node kinds (e.g. `elseBody` is empty on every node except `IF_STATEMENT`). For a language this small the memory and clarity cost was judged acceptable in exchange for integration speed.

---

## 4. Example AST — `factorial.jukti`

Source (excerpt):
```
ধরি সংখ্যা n = 6;
ধরি সংখ্যা result = 1;
যতক্ষণ (i <= n) {
    result = result * i;
}
```

Resulting tree shape:

```mermaid
graph TD
    P[PROGRAM] --> D1["DECLARATION\nn : সংখ্যা"]
    D1 --> L1["NUMBER_LITERAL\n6"]
    P --> D2["DECLARATION\nresult : সংখ্যা"]
    D2 --> L2["NUMBER_LITERAL\n1"]
    P --> W["WHILE_STATEMENT"]
    W --> COND["RELATIONAL_EXPR (<=)"]
    COND --> I1["IDENTIFIER i"]
    COND --> I2["IDENTIFIER n"]
    W --> ASSIGN["ASSIGNMENT\nresult ="]
    ASSIGN --> BIN["BINARY_EXPR (*)"]
    BIN --> I3["IDENTIFIER result"]
    BIN --> I4["IDENTIFIER i"]
```
