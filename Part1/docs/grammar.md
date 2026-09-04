# JuktiLang — Complete Language Grammar (BNF)
## CSE-4114 Compiler Design and Construction Sessional

---

## Grammar Notation

- `::=` — is defined as
- `|` — or (alternative)
- `[ ]` — optional (zero or one occurrence)
- `*` — repetition (zero or more occurrences)
- `" "` — terminal (literal keyword/symbol)

---

## 1. Program Structure

```bnf
<program>   ::= <statement>*

<statement> ::= <declaration>
              | <assignment>
              | <if-statement>
              | <while-statement>
              | <print-statement>
```

A JuktiLang program is simply a flat sequence of statements — there are no functions, so `<program>` and the body of a block share the same statement grammar.

---

## 2. Statement Rules

### 2.1 Variable Declaration

```bnf
<declaration> ::= "ধরি" <type> <identifier> "=" <expression> ";"
                | "ধরি" <type> <identifier> ";"

<type>        ::= "সংখ্যা" | "দশমিক"
```

`ধরি` ("declare") introduces a new variable of type `সংখ্যা` (int) or `দশমিক` (float). The bare form (no initializer) is allowed by the grammar; the code generator gives it a zero value (`0` or `0.0`) so the emitted Python always has a defined value to work with.

### 2.2 Assignment

```bnf
<assignment> ::= <identifier> "=" <expression> ";"
```

The target must already be declared — enforced by the semantic analyzer, not by this grammar rule.

### 2.3 If / Else

```bnf
<if-statement> ::= "যদি" "(" <condition> ")" "{" <statement>* "}"
                    [ "নাহলে" "{" <statement>* "}" ]
```

`নাহলে` ("otherwise") is optional. Bodies are always wrapped in `{ }` — there is no single-statement-without-braces form, which removes an entire class of "dangling নাহলে" ambiguity from the grammar.

### 2.4 While Loop (Part 2)

```bnf
<while-statement> ::= "যতক্ষণ" "(" <condition> ")" "{" <statement>* "}"
```

`যতক্ষণ` ("as long as") repeats its body while `<condition>` holds. The compiler places no artificial limit on loop count or nesting depth.

### 2.5 Print

```bnf
<print-statement> ::= "দেখাও" "(" <expression> ")" ";"
```

`দেখাও` ("show") takes exactly one expression, function-call style, unlike a bare `দেখাও expr;` — this was a Phase 0 decision so the grammar reads more consistently with how `যদি`/`যতক্ষণ` also use parentheses.

---

## 3. Conditions

```bnf
<condition> ::= <expression> <rel-op> <expression>
<rel-op>    ::= "==" | "!=" | "<" | ">" | "<=" | ">="
```

A condition is always a single comparison between two arithmetic expressions — there is no `&&`/`||` boolean composition in the minimum feature set.

---

## 4. Expressions

```bnf
<expression> ::= <term> ( ("+" | "-") <term> )*
<term>       ::= <factor> ( ("*" | "/") <factor> )*
<factor>     ::= <number> | <identifier> | "(" <expression> ")" | ("-" <factor>)
```

This is a standard precedence-climbing grammar: `<expression>` handles the lowest-precedence operators (`+`/`-`), `<term>` handles `*`/`/`, and `<factor>` handles atoms, parenthesized groups, and unary minus — so `-2 * 3 + 1` parses as `((-2) * 3) + 1`, matching normal arithmetic conventions.

---

## 5. Literals & Identifiers

```bnf
<number>        ::= <int-literal> | <float-literal>
<int-literal>   ::= digit+
<float-literal> ::= digit+ "." digit+

<identifier>    ::= letter (letter | digit | "_")*
```

Keywords and type names are Bangla (UTF-8); identifiers, numbers, and operators are ASCII, so a `.jukti` file mixes both scripts by design — e.g. `ধরি সংখ্যা student_count = 30;`.

---

## 6. Semantic Rules (Not Expressible in BNF)

The grammar above only defines what is *syntactically* legal. The semantic analyzer (Reshmi) additionally enforces, at compile time:

1. A variable must be declared before it is read or assigned.
2. A variable cannot be declared twice in the same scope (JuktiLang has one global scope — `if`/`while` bodies do not introduce new scopes).
3. `দশমিক` (float) values cannot be narrowed into `সংখ্যা` (int) variables, at declaration or assignment.
4. `"/"` always produces `দশমিক`, even when both operands are `সংখ্যা`.
5. Relational operators always produce `সংখ্যা` (0 or 1).

These rules are documented here because they are exactly as load-bearing as the BNF for defining "a valid JuktiLang program" — a syntactically correct program can still be semantically rejected.
