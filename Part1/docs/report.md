cat << 'ANSWER_END'
# JuktiLang — Report (Part 1 / Review 1 & 2 Snapshot)
## CSE-4114 Compiler Design and Construction Sessional

**Team Project | Compiler + Report + Presentation**

> This is the Part 1 snapshot of the project, covering Review 1 and
> Review 2 scope. See `Part2/docs/report.md` for the final report
> (all three reviews, `যদি`/`নাহলে` if-else included).

---

## Section 1: The Pitch — Why JuktiLang Matters

### 1.1 Introduction

Bangladesh has over 170 million people, and the overwhelming majority think, speak, and reason in Bangla. Yet every mainstream programming language taught in schools and universities — C, Java, Python — forces a first-time learner to master an unfamiliar English vocabulary *before* they can express even the simplest computational idea.

**JuktiLang** ("যুক্তি" = logic/reasoning) is a small toy programming language that removes that vocabulary barrier. Its keywords are romanized Bangla words for the exact concepts they represent, so a beginner reads `যতক্ষণ (i < 5)` and thinks "while i is less than 5" without a translation step in between.

### 1.2 Real-World Relevance

**Target audience:** Bangla-medium secondary school students (roughly ages 12–17) encountering programming for the first time, and first-generation university CS students more comfortable reasoning in Bangla than in English.

| English Keyword | JuktiLang | Meaning |
|---|---|---|
| `let`/`var` | `ধরি`     | declare a variable |
| `while`     | `যতক্ষণ`  | loop while a condition holds |
| `print`     | `দেখাও`   | show a value |

### 1.3 Long-Term Vision

Part 1 establishes the core: declarations, arithmetic, looping, and output. Conditional branching (`যদি`/`নাহলে`) is designed and grammar-complete already (see `grammar/juktilang.bnf`) but ships disabled until Part 2/Review 3, per the team's staged review plan (Review 2 targets "codegen complete for everything except if-else"). Beyond that, the roadmap includes a string type, functions, arrays, and eventually a browser-based playground — see `Part2/docs/report.md` Section 1.5 for the full roadmap table.

---

## Section 2: Compiler Design (Part 1 Scope)

### 2.1 Architecture Overview

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

For Part 1, `Parser::parseStatement()` dispatches to declarations, `যতক্ষণ` (while), `দেখাও`, and assignment — `যদি`/`নাহলে` is deliberately left out of the dispatch table (see `src/parser.cpp`) even though `parseIfStatement()`, semantic handling, and codegen for it already exist and are exercised by Part 2.

### 2.2 The AST Contract

All phases share one `ASTNode` struct tagged by a `NodeType` enum, frozen in the Phase 0 meeting (`src/ast.h`). This let all five team members build against the same header from Day 1 without waiting on each other's modules. See `Part2/docs/uml.md` Section 3 for the full rationale.

### 2.3 Symbol Table & Type Checking

A single global symbol table maps each declared identifier to its `DataType` (`INT`/`FLOAT`). Enforced at this stage:

- No redeclaration.
- No use-before-declaration.
- No narrowing — `দশমিক` (float) cannot be assigned into a `সংখ্যা` (int) variable.
- `/` always resolves to `দশমিক`.

### 2.4 Error Handling

A shared `ErrorHandler` collects errors by phase instead of crashing; the parser uses panic-mode recovery (skip to `;`, `}`, or the next statement keyword) so one bad line doesn't abort the whole file's diagnostics.

### 2.5 Testing (Part 1)

`examples/` and `tests/` contain declaration/arithmetic/while-loop/print programs (`countdown.jukti`, `sum_loop.jukti`) plus intentionally-invalid programs (`type_error.jukti`, `undeclared_error.jukti`, `missing_semicolon.jukti`) confirming the compiler reports the correct error rather than crashing.

---

## Section 3: Grammar in BNF

Full grammar: `grammar/juktilang.bnf` (finalized in Phase 0, including `যদি`/`নাহলে` even though it isn't wired into the Part 1 parser yet). See `docs/grammar.md` for the annotated version.

---

## Section 4: Review Status

| Review | Target | Status (this snapshot) |
|---|---|---|
| Review 1 | Lexer, parser, symbol table, type checking, assignment, arithmetic | ✅ Complete |
| Review 2 | Codegen complete except if-else, generated Python runs, error recovery demonstrated | ✅ Complete |
| Review 3 | `যদি`/`নাহলে` wired in, all minimum features | See `Part2/` |
ANSWER_END
Output

# JuktiLang — Report (Part 1 / Review 1 & 2 Snapshot)
## CSE-4114 Compiler Design and Construction Sessional

**Team Project | Compiler + Report + Presentation**

> This is the Part 1 snapshot of the project, covering Review 1 and
> Review 2 scope. See `Part2/docs/report.md` for the final report
> (all three reviews, `যদি`/`নাহলে` if-else included).

---

## Section 1: The Pitch — Why JuktiLang Matters

### 1.1 Introduction

Bangladesh has over 170 million people, and the overwhelming majority think, speak, and reason in Bangla. Yet every mainstream programming language taught in schools and universities — C, Java, Python — forces a first-time learner to master an unfamiliar English vocabulary *before* they can express even the simplest computational idea.

**JuktiLang** ("যুক্তি" = logic/reasoning) is a small toy programming language that removes that vocabulary barrier. Its keywords are romanized Bangla words for the exact concepts they represent, so a beginner reads `যতক্ষণ (i < 5)` and thinks "while i is less than 5" without a translation step in between.

### 1.2 Real-World Relevance

**Target audience:** Bangla-medium secondary school students (roughly ages 12–17) encountering programming for the first time, and first-generation university CS students more comfortable reasoning in Bangla than in English.

| English Keyword | JuktiLang | Meaning |
|---|---|---|
| `let`/`var` | `ধরি`     | declare a variable |
| `while`     | `যতক্ষণ`  | loop while a condition holds |
| `print`     | `দেখাও`   | show a value |

### 1.3 Long-Term Vision

Part 1 establishes the core: declarations, arithmetic, looping, and output. Conditional branching (`যদি`/`নাহলে`) is designed and grammar-complete already (see `grammar/juktilang.bnf`) but ships disabled until Part 2/Review 3, per the team's staged review plan (Review 2 targets "codegen complete for everything except if-else"). Beyond that, the roadmap includes a string type, functions, arrays, and eventually a browser-based playground — see `Part2/docs/report.md` Section 1.5 for the full roadmap table.

---

## Section 2: Compiler Design (Part 1 Scope)

### 2.1 Architecture Overview

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

For Part 1, `Parser::parseStatement()` dispatches to declarations, `যতক্ষণ` (while), `দেখাও`, and assignment — `যদি`/`নাহলে` is deliberately left out of the dispatch table (see `src/parser.cpp`) even though `parseIfStatement()`, semantic handling, and codegen for it already exist and are exercised by Part 2.

### 2.2 The AST Contract

All phases share one `ASTNode` struct tagged by a `NodeType` enum, frozen in the Phase 0 meeting (`src/ast.h`). This let all five team members build against the same header from Day 1 without waiting on each other's modules. See `Part2/docs/uml.md` Section 3 for the full rationale.

### 2.3 Symbol Table & Type Checking

A single global symbol table maps each declared identifier to its `DataType` (`INT`/`FLOAT`). Enforced at this stage:

- No redeclaration.
- No use-before-declaration.
- No narrowing — `দশমিক` (float) cannot be assigned into a `সংখ্যা` (int) variable.
- `/` always resolves to `দশমিক`.

### 2.4 Error Handling

A shared `ErrorHandler` collects errors by phase instead of crashing; the parser uses panic-mode recovery (skip to `;`, `}`, or the next statement keyword) so one bad line doesn't abort the whole file's diagnostics.

### 2.5 Testing (Part 1)

`examples/` and `tests/` contain declaration/arithmetic/while-loop/print programs (`countdown.jukti`, `sum_loop.jukti`) plus intentionally-invalid programs (`type_error.jukti`, `undeclared_error.jukti`, `missing_semicolon.jukti`) confirming the compiler reports the correct error rather than crashing.

---

## Section 3: Grammar in BNF

Full grammar: `grammar/juktilang.bnf` (finalized in Phase 0, including `যদি`/`নাহলে` even though it isn't wired into the Part 1 parser yet). See `docs/grammar.md` for the annotated version.

---

## Section 4: Review Status

| Review | Target | Status (this snapshot) |
|---|---|---|
| Review 1 | Lexer, parser, symbol table, type checking, assignment, arithmetic | ✅ Complete |
| Review 2 | Codegen complete except if-else, generated Python runs, error recovery demonstrated | ✅ Complete |
| Review 3 | `যদি`/`নাহলে` wired in, all minimum features | See `Part2/` |