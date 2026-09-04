# JuktiLang — Final Report
## CSE-4114 Compiler Design and Construction Sessional

**Team Project | Compiler + Report + Presentation**

---

## Section 1: The Pitch — Why JuktiLang Matters

### 1.1 Introduction

Bangladesh has over 170 million people, and the overwhelming majority think, speak, and reason in Bangla. Yet every mainstream programming language taught in schools and universities — C, Java, Python — forces a first-time learner to master an unfamiliar English vocabulary *before* they can express even the simplest computational idea.

**JuktiLang** ("যুক্তি" = logic/reasoning) is a small toy programming language that removes that vocabulary barrier. Its keywords are romanized Bangla words for the exact concepts they represent, so a beginner reads `যদি (x > 5)` and thinks "if x is greater than 5" without a translation step in between.

### 1.2 Real-World Relevance

**Target audience:** Bangla-medium secondary school students (roughly ages 12–17) encountering programming for the first time, and first-generation university CS students who are more comfortable reasoning in Bangla than in English.

**The problem JuktiLang solves:** Most introductory programming material available in Bangladesh is written in English. A student has to mentally translate `while` → `যতক্ষণ`, `print` → `দেখাও`, before they can even start reasoning about *what the loop does*. JuktiLang collapses that translation step so early lessons can focus purely on sequencing, decisions, and repetition — the actual ideas of programming — instead of vocabulary.

| English Keyword | JuktiLang | Meaning |
|---|---|---|
| `let`/`var` | `ধরি`     | declare a variable |
| `if`        | `যদি`     | conditional branch |
| `else`      | `নাহলে`   | alternative branch |
| `while`     | `যতক্ষণ`  | loop while a condition holds |
| `print`     | `দেখাও`   | show a value |

### 1.3 What Gaps Does JuktiLang Address?

1. **Language barrier in early CS education** — students in rural and Bangla-medium schools often disengage from programming not because the logic is hard, but because the syntax reads as foreign.
2. **No lightweight Bangla-syntax teaching language** — Bangla-script IDEs and translated tutorials exist, but there is no small, formally specified, *compilable* language with Bangla-inspired keywords that a student can run and see execute.
3. **Motivation** — code that "reads" in a student's own language lowers the activation energy to try the next example.

### 1.4 Long-Term Vision

The long-term vision is for JuktiLang to serve as a first, disposable stepping-stone language — used for a few weeks of instruction before students transition to a mainstream language like Python or C, carrying over the concepts (variables, branching, loops) without the vocabulary friction.

**Vision milestones:**
1. v1.0 (Part 1) — declarations, arithmetic, `যদি`/`নাহলে`, `দেখাও`
2. v2.0 (Part 2) — `যতক্ষণ` loops, panic-mode syntax error recovery
3. v3.0 (future) — string type (`বর্ণ`), functions, arrays
4. v4.0 (future) — a browser-based JuktiLang playground (compile-and-run in the browser)

### 1.5 Future Roadmap (Hypothetical Features)

| Feature | Description | Target Version |
|---|---|---|
| Functions | `কাজ myFunc() { ... }` | v3.0 |
| Arrays | `ধরি সংখ্যা[] arr = [1, 2, 3];` | v3.0 |
| String type | `ধরি বর্ণ name = "Rahim";` | v3.0 |
| For loop | `জন্য (i = 1; i <= 10; i = i + 1)` | v3.0 |
| Standard library | Built-in math/string helpers | v4.0 |
| Browser playground | Compile-and-run JuktiLang in-browser | v4.0 |

---

## Section 2: Compiler Design

### 2.1 Architecture Overview

The JuktiLang compiler is a single-pass, multi-phase compiler written in **C++17**. It translates `.jukti` source files into executable Python (`.py`) files through four sequential phases:

```
Source (.jukti)
      │
      ▼
┌─────────────────┐
│ 1. Lexer         │  source text -> token stream        (owner: Trisha)
├─────────────────┤
│ 2. Parser        │  token stream -> AST (recursive      (owner: Rahat)
│                  │  descent, panic-mode recovery)
├─────────────────┤
│ 3. Semantic      │  AST walk -> symbol table + type     (owner: Reshmi)
│    Analyzer      │  checking, fills resolvedType
├─────────────────┤
│ 4. Code          │  AST walk -> Python source string    (owner: Sonia)
│    Generator     │
└─────────────────┘
      │
      ▼
 Output (.py)
```

`main.cpp` (integration: Samiya) wires the four phases together and stops at the first phase that reports an error, printing every collected error for that phase before aborting — so a user sees *all* lexical, or all syntax, or all semantic problems in one run rather than one-at-a-time.

### 2.2 The AST Contract

All four phases operate on one shared node type, `ASTNode` (`src/ast.h`), tagged with a `NodeType` enum (`PROGRAM`, `DECLARATION`, `ASSIGNMENT`, `IF_STATEMENT`, `WHILE_STATEMENT`, `PRINT_STATEMENT`, `BINARY_EXPR`, `RELATIONAL_EXPR`, `UNARY_EXPR`, `NUMBER_LITERAL`, `IDENTIFIER`). This single-struct design (rather than a class hierarchy with one struct per node kind) was chosen in Phase 0 specifically so all five team members could agree on and freeze *one* header before writing any other code, instead of coordinating changes across a dozen node classes mid-sprint.

### 2.3 Symbol Table & Type Checking

The semantic analyzer keeps a single global symbol table (`std::unordered_map<std::string, SymbolInfo>`) mapping each declared identifier to its `DataType` (`INT` or `FLOAT`) and declaration line. Two rules are enforced:

- **No redeclaration** — declaring the same identifier twice is a semantic error.
- **No use-before-declaration** — every identifier reference is checked against the symbol table.
- **No narrowing** — a `দশমিক` (float) value can never be assigned into a `সংখ্যা` (int) variable, whether at declaration or later assignment. The reverse (int literal into a float variable) is always allowed.

Division (`/`) always resolves to `দশমিক`, matching Python 3's default division behavior, and relational operators (`==`, `!=`, `<`, `>`, `<=`, `>=`) always resolve to `সংখ্যা` (treated as 0/1, mirroring how Python treats booleans as ints in this generated code).

### 2.4 Code Generation Strategy

Python was chosen as the target because it needs no separate compilation step for the student to see their program run, and its indentation-based blocks map directly onto JuktiLang's `{ }` blocks without extra bookkeeping. Each `ধরি` declaration is emitted with an explicit `int(...)`/`float(...)` cast so a `সংখ্যা` variable can never end up silently holding a Python float, even if the source expression's static type were ever miscalculated upstream.

### 2.5 Error Handling Strategy

A shared `ErrorHandler` (`src/error.h`) collects `CompilerError` records tagged by phase (`LEXICAL`, `SYNTAX`, `SEMANTIC`) instead of any phase calling `exit()` or throwing an uncaught exception. The parser additionally implements **panic-mode recovery**: on a syntax error it throws internally, is caught by the nearest statement-parsing loop, and skips tokens until it reaches a `;`, a `}`, or a token that starts a new statement (`ধরি`, `যদি`, `যতক্ষণ`, `দেখাও`) — so one malformed line is reported without derailing the rest of the file (see `examples/syntax_recovery.jukti`).

### 2.6 Testing

Every construct in the language has at least one example program under `examples/` and `tests/`, each compiled and its generated Python actually executed (not just "does it compile") as part of `make test`. Three examples are intentionally invalid — `type_error.jukti`, `undeclared_error.jukti`, `missing_semicolon.jukti` — to confirm the compiler reports the *right* error instead of silently accepting bad input or crashing.

---

## Section 3: Grammar in BNF

The full grammar is finalized in `grammar/juktilang.bnf` (owned by Rahat) and reproduced in `docs/grammar.md` with per-rule explanations. See those files for the complete specification; the shape at a glance:

```
program        ::= statement*
statement      ::= declaration | assignment | if-statement
                  | while-statement | print-statement
declaration    ::= "ধরি" type identifier ["=" expression] ";"
if-statement   ::= "যদি" "(" condition ")" "{" statement* "}"
                    ["নাহলে" "{" statement* "}"]
while-statement::= "যতক্ষণ" "(" condition ")" "{" statement* "}"
print-statement::= "দেখাও" "(" expression ")" ";"
condition      ::= expression rel-op expression
expression     ::= term (("+"|"-") term)*
term           ::= factor (("*"|"/") factor)*
factor         ::= number | identifier | "(" expression ")" | "-" factor
```

---

## Section 4: Team & Review Outcomes

| Review | Target | Status |
|---|---|---|
| Review 1 | Lexer, parser, symbol table, type checking, assignment, arithmetic | ✅ Complete |
| Review 2 | Codegen (excluding while), generated Python runs, error recovery demonstrated | ✅ Complete |
| Review 3 | `যতক্ষণ` loops wired in, all minimum features, codegen for every construct | ✅ Complete |

See `README.md` for build instructions and the module ownership table.
