# JuktiLang
### A Bangla-Inspired Programming Language, Compiled to Python
CSE-4114 — Compiler Design and Construction Sessional

Team: Abdul Mutalib Rahat · Nusrat Jahan Trisha · Farjana Islam Reshmi · MST Sonia Akhter · Samiya Choudhury

---

## The Problem

- 170M+ Bangladeshis think and speak in Bangla.
- Every mainstream teaching language (C, Python, Java) uses English keywords.
- Beginners spend cognitive effort translating `while` → "যতক্ষণ" *before* they can reason about the loop.

---

## The Idea

**JuktiLang** — a toy language with romanized Bangla keywords, compiled straight to runnable Python.

```
ধরি সংখ্যা x = 5;
যদি (x > 3) {
    দেখাও(x);
}
```

compiles to:

```python
x = int(5)
if (x > 3):
    print(x)
```

---

## Language at a Glance

| JuktiLang | Meaning |
|---|---|
| `ধরি`     | declare |
| `সংখ্যা`   | int |
| `দশমিক`   | float |
| `যদি` / `নাহলে` | if / else |
| `যতক্ষণ`  | while |
| `দেখাও`   | print |

---

## Compiler Architecture

```
.jukti → [Lexer] → tokens → [Parser] → AST → [Semantic] → typed AST → [Codegen] → .py
```

- 4 phases, one shared `ErrorHandler`.
- Single-struct `ASTNode` design, frozen in Phase 0 — every phase built against the same contract from Day 1.

---

## Error Handling

- No crashes on malformed input — every phase reports, never throws past `main()`.
- **Panic-mode recovery**: one bad statement is reported and skipped; the rest of the file still compiles.

```
ধরি সংখ্যা a  10;      // missing '=' — reported
ধরি সংখ্যা b = 20;     // still parses fine
দেখাও(b);              // -> 20
```

---

## Type Checking

- `সংখ্যা` (int) and `দশমিক` (float), single global scope.
- No narrowing: a `দশমিক` value can never silently become `সংখ্যা`.
- `/` always produces `দশমিক` — matches Python 3 semantics.

---

## Demo: Factorial

```
ধরি সংখ্যা n = 6;
ধরি সংখ্যা result = 1;
ধরি সংখ্যা i = 1;
যতক্ষণ (i <= n) {
    result = result * i;
    i = i + 1;
}
দেখাও(result);
```

→ compiles and runs → prints `720`

---

## Review Timeline

| Review | Delivered |
|---|---|
| 1 | Lexer, parser, symbol table, type checking, arithmetic |
| 2 | Full codegen (minus while), generated Python runs, error recovery |
| 3 | `যতক্ষণ` wired in, all minimum features complete |

---

## What's Next

- Functions, arrays, a string type
- A browser-based JuktiLang playground

## Thank You
Questions?
