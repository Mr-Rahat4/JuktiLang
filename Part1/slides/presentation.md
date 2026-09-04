# JuktiLang
### A Bangla-Inspired Programming Language, Compiled to Python
CSE-4114 — Compiler Design and Construction Sessional | **Review 1**

Team: Abdul Mutalib Rahat · Nusrat Jahan Trisha · Farjana Islam Reshmi · MST Sonia Akhter · Samiya Choudhury

---

## The Problem

- 170M+ Bangladeshis think and speak in Bangla.
- Every mainstream teaching language (C, Python, Java) uses English keywords.
- Beginners spend cognitive effort translating vocabulary *before* they can reason about the logic.

---

## The Idea

**JuktiLang** — a toy language with romanized Bangla keywords, compiled to runnable Python.

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

## Language at a Glance (Part 1)

| JuktiLang | Meaning |
|---|---|
| `ধরি`     | declare |
| `সংখ্যা`   | int |
| `দশমিক`   | float |
| `যদি` / `নাহলে` | if / else |
| `দেখাও`   | print |

`যতক্ষণ` (while) is finalized in the grammar and already implemented in the parser and codegen — it ships active in Part 2.

---

## Compiler Architecture

```
.jukti → [Lexer] → tokens → [Parser] → AST → [Semantic] → typed AST → [Codegen] → .py
```

- One shared `ErrorHandler`, one frozen `ASTNode` contract — agreed in a Phase 0 meeting before any implementation started.

---

## Type Checking

- `সংখ্যা` (int) and `দশমিক` (float), single global scope.
- No narrowing: a `দশমিক` value can never silently become `সংখ্যা`.
- `/` always produces `দশমিক`.

---

## Demo

```
ধরি সংখ্যা a = 12;
ধরি সংখ্যা b = 5;
দেখাও(a + b);     // 17
দেখাও(a * b);     // 60
ধরি দশমিক c = a / b;
দেখাও(c);         // 2.4
```

---

## Review 1 Checklist

- Lexer tokenizes correctly ✅
- Parser handles valid input + reports syntax errors ✅
- Symbol table + type checking ✅
- Assignment + arithmetic ✅

---

## Next: Review 2 & 3

- Wire in `যতক্ষণ` (while)
- Demonstrate panic-mode syntax error recovery
- Full codegen for every construct

## Thank You
Questions?
