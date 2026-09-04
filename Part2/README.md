# JuktiLang Compiler — Part 2 (Reviews 1–3, complete)

JuktiLang → Python compiler, written in C++17.

## What is JuktiLang?

**JuktiLang** (`.jukti`) is an original toy programming language using romanized Bangla keywords, compiled to executable Python. See `docs/report.md` for the full pitch and `docs/grammar.md` / `grammar/juktilang.bnf` for the language specification.

```
ধরি সংখ্যা x = 5;
যদি (x > 3) {
    দেখাও(x);
}
```

## Team & ownership

| Area | Owner | Files |
|---|---|---|
| Grammar & Parser | Abdul Mutalib Rahat | `grammar/juktilang.bnf`, `src/parser.h/.cpp` |
| Lexer & Tokens | Nusrat Jahan Trisha | `src/token.h`, `src/lexer.h/.cpp` |
| Semantic Analysis | Farjana Islam Reshmi | `src/semantic.h/.cpp` |
| Code Generation | MST Sonia Akhter | `src/codegen.h/.cpp` |
| Integration, Testing & Docs | Samiya Choudhury | `tests/`, `Makefile`, `docs/`, `src/main.cpp`, this README |

**Shared contract:** `src/ast.h` (the AST every phase reads/writes) and `src/error.h` (the shared error reporting infra) were agreed on in the Phase 0 meeting and frozen for the rest of the project.

## Build & run

```bash
make                              # builds ./juktilangc
./juktilangc examples/hello.jukti # writes examples/hello.py
python3 examples/hello.py
make test                         # runs every tests/valid + tests/errors file,
                                   # and executes every successfully-generated .py
```

CLI flags: `--tokens` dumps the lexer's token stream; `--no-codegen` stops after semantic analysis (useful for checking a file compiles without producing output).

## Status: all minimum features complete

- Declarations (`ধরি`), assignment, full arithmetic (`+ - * /`, unary minus, parentheses)
- `যদি` / `নাহলে` (if/else), `যতক্ষণ` (while) — arbitrarily nested
- `দেখাও` (print)
- Symbol table + static type checking (`সংখ্যা`/`দশমিক`, no float→int narrowing)
- Panic-mode syntax error recovery (see `examples/syntax_recovery.jukti`)
- Every example under `examples/` compiles (or intentionally fails) and every successful one's generated Python was executed and verified

## Directory layout

```
Part2/
├── src/            # lexer, parser, semantic analyzer, codegen, main
├── grammar/        # juktilang.bnf — the authoritative grammar
├── docs/           # report.md, grammar.md, uml.md
├── slides/         # presentation.md
├── examples/       # runnable, documented sample programs
└── tests/          # valid/ and errors/ regression cases for `make test`
```

## Review history

- **Review 1:** lexer tokenizes correctly; parser handles valid input and reports syntax errors; symbol table + type checking + assignment + arithmetic working.
- **Review 2:** codegen complete except while; generated Python compiles/runs; error recovery demonstrated (see Part1, which snapshots this state).
- **Review 3 (this folder):** `যতক্ষণ` fully wired in end-to-end; all minimum features complete; codegen produces executable output for every construct.

## Rules that held for the whole project

- Commit 3x/week minimum, from Day 1.
- No crashes on malformed input — always report an error and continue/exit cleanly.
- Reviews are individual: everyone must be able to explain any part of the code, not just their own module.
