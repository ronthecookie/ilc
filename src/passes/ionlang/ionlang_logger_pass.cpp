#include <ilc/passes/ionlang/ionlang_logger_pass.h>
#include <ionlang/const/const.h>

namespace ilc {
    void IonLangLoggerPass::visit(ionshared::Ptr<ionlang::Construct> node) {
        ionlang::ConstructKind constructKind = node->getConstructKind();
        std::optional<std::string> constructName = ionlang::Const::getConstructKindName(constructKind);
        std::string defaultName = "Unknown (" + std::to_string((int)constructKind) + ")";
        std::string addressString = " [" + ionshared::util::getPointerAddressString(node.get()) + "]";
        std::cout << "Visiting node: " << constructName.value_or(defaultName) << addressString << std::endl;

        ionlang::Pass::visit(node);
    }
}
