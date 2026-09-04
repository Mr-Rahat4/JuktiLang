// error.h — SHARED INFRASTRUCTURE (added in Phase 0 alongside ast.h)
//
// Every phase reports through this instead of throwing or calling
// std::exit directly. This is what makes Samiya's "no crashes on
// malformed input" rule enforceable: lexer/parser/semantic all keep
// running and collect errors, and main.cpp decides when to stop.

#ifndef JUKTILANG_ERROR_H
#define JUKTILANG_ERROR_H

#include <string>
#include <vector>
#include <iostream>

enum class ErrorPhase { LEXICAL, SYNTAX, SEMANTIC };

struct CompilerError {
    ErrorPhase phase;
    std::string message;
    int line;
    int column;

    std::string toString() const {
        std::string tag;
        switch (phase) {
            case ErrorPhase::LEXICAL:  tag = "[LEXICAL ERROR]";  break;
            case ErrorPhase::SYNTAX:   tag = "[SYNTAX ERROR]";   break;
            case ErrorPhase::SEMANTIC: tag = "[SEMANTIC ERROR]"; break;
        }
        return tag + " Line " + std::to_string(line) + ", Col " +
               std::to_string(column) + ": " + message;
    }
};

class ErrorHandler {
public:
    std::vector<CompilerError> errors;

    void lexicalError(int line, int col, const std::string& msg) {
        errors.push_back({ErrorPhase::LEXICAL, msg, line, col});
    }
    void syntaxError(int line, int col, const std::string& msg) {
        errors.push_back({ErrorPhase::SYNTAX, msg, line, col});
    }
    void semanticError(int line, int col, const std::string& msg) {
        errors.push_back({ErrorPhase::SEMANTIC, msg, line, col});
    }

    bool hasErrors() const { return !errors.empty(); }

    void printAll() const {
        std::cerr << "\n========== JuktiLang Compiler Errors ==========\n";
        for (const auto& e : errors) std::cerr << e.toString() << "\n";
        std::cerr << "================================================\n";
        std::cerr << "Total errors: " << errors.size() << "\n\n";
    }
};

#endif // JUKTILANG_ERROR_H
