cat << 'ANSWER_END'
# JuktiLang Compiler — Part 1 (Review 1 & 2 snapshot)

JuktiLang → Python compiler, written in C++17.

## What is JuktiLang?

**JuktiLang** (`.jukti`) is an original toy programming language using romanized Bangla keywords, compiled to executable Python. See `docs/report.md` for the full pitch and `docs/grammar.md` / `grammar/juktilang.bnf` for the language specification.

## Team & ownership

| Area | Owner | Files |
|---|---|---|
| Grammar & Parser | Abdul Mutalib Rahat | `grammar/juktilang.bnf`, `src/parser.h/.cpp` |
| Lexer & Tokens | Nusrat Jahan Trisha | `src/token.h`, `src/lexer.h/.cpp` |
| Semantic Analysis | Farjana Islam Reshmi | `src/semantic.h/.cpp` |
| Code Generation | MST Sonia Akhter | `src/codegen.h/.cpp` |
| Integration, Testing & Docs | Samiya Choudhury | `tests/`, `Makefile`, `docs/`, `src/main.cpp`, this README |

**Shared contract:** `src/ast.h` and `src/error.h`, frozen in the Phase 0 meeting.

## Build & run

```bash
g++ -std=c++17 -Wall -Wextra -g src/*.cpp -o juktilangc
./juktilangc examples/hello.jukti   # writes examples/hello.py
python3 examples/hello.py
make test                           # runs every tests/valid + tests/errors file
```

## Status: Review 1 & 2 targets met

- Lexer tokenizes correctly (UTF-8 Bangla keywords + ASCII identifiers/numbers)
- Parser handles declarations, assignment, `যতক্ষণ` (while), `দেখাও`, and reports syntax errors
- Symbol table + static type checking (`সংখ্যা`/`দশমিক`, no float→int narrowing)
- Assignment and full arithmetic (`+ - * /`, unary minus, parentheses, precedence)
- `যতক্ষণ` (while) loops are fully working — matches the Review 2 target of "codegen complete for everything except if-else"
- `যদি`/`নাহলে` (if-else) is finalized in the grammar and already implemented in the parser and codegen internals — intentionally **not** dispatched yet (`parseStatement()` skips `KW_IF`), since conditional branching is deferred to Review 3. It ships active in `Part2/`.

## Directory layout

```
Part1/
├── src/            # lexer, parser, semantic analyzer, codegen, main
├── grammar/        # juktilang.bnf — the authoritative (full) grammar
├── docs/           # report.md, grammar.md, uml.md
├── slides/         # presentation.md
├── examples/       # runnable Part-1-scope sample programs (no if-else)
└── tests/          # valid/ and errors/ regression cases
```

## Rules

- Commit 3x/week minimum, from Day 1.
- No crashes on malformed input — always report an error and continue/exit cleanly.
- Reviews are individual: everyone must be able to explain any part of the code, not just their own module.
ANSWER_END
Output

# JuktiLang Compiler — Part 1 (Review 1 & 2 snapshot)

JuktiLang → Python compiler, written in C++17.

## What is JuktiLang?

**JuktiLang** (`.jukti`) is an original toy programming language using romanized Bangla keywords, compiled to executable Python. See `docs/report.md` for the full pitch and `docs/grammar.md` / `grammar/juktilang.bnf` for the language specification.

## Team & ownership

| Area | Owner | Files |
|---|---|---|
| Grammar & Parser | Abdul Mutalib Rahat | `grammar/juktilang.bnf`, `src/parser.h/.cpp` |
| Lexer & Tokens | Nusrat Jahan Trisha | `src/token.h`, `src/lexer.h/.cpp` |
| Semantic Analysis | Farjana Islam Reshmi | `src/semantic.h/.cpp` |
| Code Generation | MST Sonia Akhter | `src/codegen.h/.cpp` |
| Integration, Testing & Docs | Samiya Choudhury | `tests/`, `Makefile`, `docs/`, `src/main.cpp`, this README |

**Shared contract:** `src/ast.h` and `src/error.h`, frozen in the Phase 0 meeting.

## Build & run

```bash
g++ -std=c++17 -Wall -Wextra -g src/*.cpp -o juktilangc
./juktilangc examples/hello.jukti   # writes examples/hello.py
python3 examples/hello.py
make test                           # runs every tests/valid + tests/errors file
```

## Status: Review 1 & 2 targets met

- Lexer tokenizes correctly (UTF-8 Bangla keywords + ASCII identifiers/numbers)
- Parser handles declarations, assignment, `যতক্ষণ` (while), `দেখাও`, and reports syntax errors
- Symbol table + static type checking (`সংখ্যা`/`দশমিক`, no float→int narrowing)
- Assignment and full arithmetic (`+ - * /`, unary minus, parentheses, precedence)
- `যতক্ষণ` (while) loops are fully working — matches the Review 2 target of "codegen complete for everything except if-else"
- `যদি`/`নাহলে` (if-else) is finalized in the grammar and already implemented in the parser and codegen internals — intentionally **not** dispatched yet (`parseStatement()` skips `KW_IF`), since conditional branching is deferred to Review 3. It ships active in `Part2/`.

## Directory layout

```
Part1/
├── src/            # lexer, parser, semantic analyzer, codegen, main
├── grammar/        # juktilang.bnf — the authoritative (full) grammar
├── docs/           # report.md, grammar.md, uml.md
├── slides/         # presentation.md
├── examples/       # runnable Part-1-scope sample programs (no if-else)
└── tests/          # valid/ and errors/ regression cases
```

## Rules

- Commit 3x/week minimum, from Day 1.
- No crashes on malformed input — always report an error and continue/exit cleanly.
- Reviews are individual: everyone must be able to explain any part of the code, not just their own module.