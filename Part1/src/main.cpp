// main.cpp — Integration point (Samiya owns overall wiring/testing, but
// everyone touches this as their module comes online).
//
// Usage: ./juktilangc <source.jukti> [--tokens] [--no-codegen]
// Output: writes <source>.py next to the input file. Never crashes on
// malformed input — every phase reports through ErrorHandler and main()
// decides when to stop.

#include "token.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

#include <iostream>
#include <fstream>
#include <sstream>

static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[JuktiLang] Error: Cannot open file '" << path << "'\n";
        std::exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[JuktiLang] Error: Cannot write to '" << path << "'\n";
        std::exit(1);
    }
    file << content;
}

static std::string deriveOutputPath(const std::string& inputPath) {
    size_t lastSlash = inputPath.find_last_of("/\\");
    std::string dir = (lastSlash == std::string::npos) ? "" : inputPath.substr(0, lastSlash + 1);
    std::string baseName = (lastSlash == std::string::npos) ? inputPath : inputPath.substr(lastSlash + 1);
    size_t lastDot = baseName.find_last_of('.');
    std::string stem = (lastDot == std::string::npos) ? baseName : baseName.substr(0, lastDot);
    return dir + stem + ".py";
}

static void printBanner() {
    std::cout << "\n";
    std::cout << "  ================================================\n";
    std::cout << "   JuktiLang Compiler  v1.0\n";
    std::cout << "   CSE-4114 | Compiler Design Sessional\n";
    std::cout << "   Language : JuktiLang (.jukti)\n";
    std::cout << "   Features : Part 1 ONLY (Fundamentals)\n";
    std::cout << "   Target   : Python (.py)\n";
    std::cout << "  ================================================\n\n";
}

static void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " <source.jukti> [--tokens] [--no-codegen]\n";
    std::cout << "  <source.jukti> : JuktiLang source file\n";
    std::cout << "  --tokens       : (optional) dump the token stream to stdout\n";
    std::cout << "  --no-codegen   : (optional) run only lex+parse+semantic, no output\n";
}

int main(int argc, char** argv) {
    printBanner();

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath = argv[1];
    bool dumpTokens = false;
    bool noCodegen = false;
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tokens")     dumpTokens = true;
        if (arg == "--no-codegen") noCodegen  = true;
    }

    if (inputPath.size() < 6 || inputPath.substr(inputPath.size() - 6) != ".jukti") {
        std::cerr << "[JuktiLang] Warning: File should have a .jukti extension.\n";
    }

    ErrorHandler err;

    // ── Phase 1: Lexical Analysis ───────────────────────────────────
    std::cout << "[Phase 1] Lexical Analysis...\n";
    std::string source = readFile(inputPath);
    Lexer lexer(source, err);
    std::vector<Token> tokens = lexer.tokenize();

    if (dumpTokens) {
        std::cout << "\n-- Token Stream --------------------------------\n";
        for (const auto& tok : tokens) std::cout << "  " << tok.toString() << "\n";
        std::cout << "--------------------------------------------------\n\n";
    }

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[JuktiLang] Lexical errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> " << tokens.size() << " tokens found.\n";

    // ── Phase 2: Parsing ─────────────────────────────────────────────
    std::cout << "[Phase 2] Parsing...\n";
    Parser parser(tokens, err);
    ASTNodePtr program = parser.parseProgram();

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[JuktiLang] Syntax errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> AST built with " << program->children.size() << " top-level statement(s).\n";

    // ── Phase 3: Semantic Analysis ───────────────────────────────────
    std::cout << "[Phase 3] Semantic Analysis...\n";
    SemanticAnalyzer semantic(err);
    semantic.analyze(program);

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[JuktiLang] Semantic errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> Type checking passed. " << semantic.symbolTable().size()
               << " variable(s) in symbol table.\n";

    if (noCodegen) {
        std::cout << "\n[JuktiLang] Compilation successful (no output requested).\n";
        return 0;
    }

    // ── Phase 4: Code Generation ─────────────────────────────────────
    std::cout << "[Phase 4] Generating Python code...\n";
    CodeGenerator codegen(err);
    std::string pyCode = codegen.generate(program);

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[JuktiLang] Code generation errors found.\n";
        return 1;
    }

    std::string outputPath = deriveOutputPath(inputPath);
    writeFile(outputPath, pyCode);
    std::cout << "  -> Python output written to: " << outputPath << "\n\n";

    std::cout << "========================================\n";
    std::cout << " Compilation successful!\n";
    std::cout << " Run your program: python3 " << outputPath << "\n";
    std::cout << "========================================\n\n";

    return 0;
}
