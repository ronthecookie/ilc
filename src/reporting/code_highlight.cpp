#include <sstream>
#include <vector>
#include <ionir/lexical/token_identifier.h>
#include <ilc/misc/console_color.h>
#include <ilc/reporting/code_highlight.h>

namespace ilc {
    std::string CodeHighlight::coat(ionir::Token token) {
        // Abstract the token's kind & value to avoid repetition.
        const ionir::TokenKind kind = token.getKind();
        const std::string value = token.getValue();

        if (ionir::TokenIdentifier::isKeyword(kind)) {
            return ConsoleColor::blue(value);
        }
        else if (kind == ionir::TokenKind::Identifier) {
            return ConsoleColor::green(value);
        }
        else if (ionir::TokenIdentifier::isNumeric(kind)) {
            return ConsoleColor::magenta(value);
        }
            // No coating should be applied to the provided token's value.
        else {
            return value;
        }
    }
}

// TODO
//std::optional<std::string> CodeHighlight::create(uint32_t lineNumber, uint32_t grace) {
//    std::stringstream codeBlock;
//
//    // Separate code block from previous messages by a single line.
//    codeBlock << std::endl << std::endl;
//
//    // Compute start & end line for the code block.
//    uint32_t start = grace >= lineNumber ? 0 : lineNumber - grace;
//    uint32_t end = lineNumber + grace;
//
//    this->stream.begin();
//
//    uint32_t lineCounter = 0;
//    std::optional<Token> tokenBuffer = std::nullopt;
//    std::vector<std::string> lineBuffer = {};
//    bool prime = true;
//    bool met = false;
//
//    while (lineCounter != start) {
//        if (!this->stream.hasNext()) {
//            // Could not reach starting point.
//            return std::nullopt;
//        }
//
//        tokenBuffer = this->stream.next();
//
//        if (tokenBuffer->getLineNumber() != lineCounter) {
//            lineCounter = tokenBuffer->getLineNumber();
//        }
//    }
//
//    while (lineCounter != end) {
//        met = lineCounter >= lineNumber;
//
//        if (!prime) {
//            tokenBuffer = this->stream.next();
//        }
//        else {
//            tokenBuffer = this->stream.get();
//            prime = false;
//        }
//
//        bool streamHasNext = this->stream.hasNext();
//
//        lineBuffer.push_back(tokenBuffer->getValue());
//
//        if (stream.peek()->getLineNumber() != lineCounter) {
//            codeBlock
//                << CodeTraceBlock::createLine(Util::joinStringVector(lineBuffer), tokenBuffer->getLineNumber());
//
//            lineBuffer.clear();
//            lineCounter = stream.peek()->getLineNumber();
//
//            continue;
//        }
//
//        // Could not reach end point.
//        if (!streamHasNext && !met) {
//            return std::nullopt;
//        }
//            // Return requirements have been met. Do not continue.
//        else if (!streamHasNext) {
//            codeBlock
//                << CodeTraceBlock::createLine(Util::joinStringVector(lineBuffer), tokenBuffer->getLineNumber());
//
//            return codeBlock.str();
//        }
//    }
//
//    return codeBlock.str();
//}