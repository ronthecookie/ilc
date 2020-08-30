#include <ionshared/error_handling/notice.h>
#include <ionshared/llvm/llvm_module.h>
#include <ionlang/error_handling/code_backtrack.h>
#include <ionlang/passes/codegen/ionir_codegen_pass.h>
#include <ionlang/lexical/lexer.h>
#include <ionlang/syntax/parser.h>
#include <ionir/passes/codegen/llvm_codegen_pass.h>
#include <ilc/passes/ionlang/ionlang_logger_pass.h>
#include <ilc/repl/ionlang_processor.h>

namespace ilc {
    std::vector<ionlang::Token> IonLangProcessor::lex() {
        ionlang::Lexer lexer = ionlang::Lexer(this->getInput());
        std::vector<ionlang::Token> tokens = lexer.scan();

        std::cout << "--- Lexer: " << tokens.size() << " token(s) ---" << std::endl;

        for (auto token : tokens) {
            std::cout << token << std::endl;
        }

        return tokens;
    }

    ionshared::Ptr<ionlang::Module> IonLangProcessor::parse(std::vector<ionlang::Token> tokens) {
        ionlang::TokenStream stream = ionlang::TokenStream(tokens);
        ionlang::Parser parser = ionlang::Parser(stream);

        try {
            ionshared::OptPtr<ionlang::Module> moduleResult = parser.parseModule();

            // TODO: Improve if block?
            if (ionshared::util::hasValue(moduleResult)) {
                // TODO: What if multiple top-level, in-line constructs are parsed? (Additional note below).
                std::cout << "--- Parser ---" << std::endl;
            }
            else {
                std::cout << "Parser: [Exception] Could not parse module" << std::endl;

                ionshared::Ptr<ionshared::NoticeStack> noticeStack = parser.getNoticeSentinel()->getNoticeStack();
                ionlang::CodeBacktrack codeBacktrack = ionlang::CodeBacktrack(this->getInput(), stream);

                // TODO: Not showing stack trace until implemented.
                std::cout << "! Skipping stack trace because it's not yet implemented !" << std::endl;
//                std::optional<std::string> stackTraceResult = StackTraceFactory::makeStackTrace(IonIrStackTraceOpts{
//                    codeBacktrack,
//                    noticeStack,
//                    this->getOptions().stackTraceHighlight
//                });
//
//                // TODO: Check for null ->make().
//                if (stackTraceResult.has_value()) {
//                    std::cout << std::endl << *stackTraceResult;
//                }
//                else {
//                    std::cout << "Could not create stack-trace" << std::endl;
//                }

                // TODO: Cannot return null.
//                return;
            }

            return *moduleResult;
        }
        catch (std::exception &exception) {
            std::cout << "Parser: [Exception] " << exception.what() << std::endl;
            this->tryThrow(exception);
        }

        throw std::runtime_error("!! DEBUGGING POINT: NEEDS RETURN !!");
    }

    void IonLangProcessor::codegen(ionshared::Ptr<ionlang::Module> module) {
        try {
            // TODO: Creating mock AST?
            ionlang::Ast ast = {
                module
            };

            /**
             * Create a pass manager instance & run applicable passes
             * over the resulting AST.
             */
            ionlang::PassManager passManager = ionlang::PassManager();

            // Register all passes to be used by the pass manager.
            // TODO: Create and implement IonLangLogger pass.
            passManager.registerPass(std::make_shared<IonLangLoggerPass>());

            // Execute the pass manager against the parser's resulting AST.
            passManager.run(ast);

            // TODO: CRITICAL: Should be used with the PassManager instance, as a normal pass instead of manually invoking the visit functions.
            ionlang::IonIrCodegenPass codegenPass = ionlang::IonIrCodegenPass();

            // TODO: What if multiple top-level constructs are defined in-line? Use ionir::Driver (finish it first) and use its resulting Ast. (Additional note above).
            // Visit the parsed module construct.
            codegenPass.visitModule(module);

            ionshared::OptPtr<ionir::Module> ionIrModuleBuffer = codegenPass.getModuleBuffer();

            if (!ionshared::util::hasValue(ionIrModuleBuffer)) {
                throw std::runtime_error("Module is nullptr");
            }

            // Now, make the ionir::LlvmCodegenPass.
            ionir::LlvmCodegenPass ionIrLlvmCodegenPass = ionir::LlvmCodegenPass();

            // Visit the resulting IonIR module buffer from the IonLang codegen pass.
            ionIrLlvmCodegenPass.visitModule(*ionIrModuleBuffer);

            std::map<std::string, llvm::Module *> modules = ionIrLlvmCodegenPass.getModules()->unwrap();

            // Display the resulting code of all the modules.
            for (const auto &[key, value] : modules) {
                std::cout << "--- LLVM code-generation: " << key << " ---" << std::endl;
                ionshared::LlvmModule(value).print();
            }

            if (modules.empty()) {
                std::cout << "--- LLVM code-generation contained no modules ---" << std::endl;
            }
        }
        catch (std::exception &exception) {
            std::cout << "LLVM code-generation: [Exception] " << exception.what() << std::endl;
            this->tryThrow(exception);
        }
    }

    IonLangProcessor::IonLangProcessor(Options options, std::string input)
        : ReplProcessor(options, input) {
        //
    }

    void IonLangProcessor::run() {
        std::vector<ionlang::Token> tokens = this->lex();
        ionshared::Ptr<ionlang::Module> module = this->parse(tokens);

        this->codegen(module);
    }
}
