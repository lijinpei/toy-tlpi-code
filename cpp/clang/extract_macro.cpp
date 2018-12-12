/*
g++ $(llvm-config --cxxflags) extrac_macro.cpp $(llvm-config --ldflags) $(llvm-config --link-shared) -lLLVM -lclangTooling -lclangAST -lclangFrontend -lclangBasic -lclangSema -lclangLex -lclangParse
 */
#include "clang/AST/APValue.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Parse/Parser.h"
#include "clang/Sema/SemaConsumer.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/ADT/APInt.h"

#include <string>
#include <utility>
#include <vector>

using namespace clang;

using ValVecTy = std::vector<std::pair<clang::APValue*, const clang::Type*>>;

class MacroDefinitionConsumer : public SemaConsumer {
  CompilerInstance& ci_;
  std::string& output_;
  std::vector<std::string>& names_;

 public:
  explicit MacroDefinitionConsumer(CompilerInstance& ci, std::string& output,
                                   std::vector<std::string>& names)
      : ci_(ci), output_(output), names_(names) {}
  void HandleTranslationUnit(ASTContext&) override {
    Preprocessor& prep = ci_.getPreprocessor();
    int cnt = 0;
    for (auto itor = prep.macro_begin(), itor_end = prep.macro_end();
         itor != itor_end; ++itor) {
      MacroDefinition md = prep.getMacroDefinition(itor->first);
      MacroInfo* mi = md.getMacroInfo();
      if (!mi || !mi->isObjectLike()) {
        continue;
      }
      llvm::StringRef name = itor->first->getName();
      if (name == "errno") {
        continue;
      }
      if (name.size() >= 2 && name[0] == '_' &&
          (name[1] == '_' || std::isupper(name[1]))) {
        continue;
      }
      std::string name_str = name.str();
      output_ +=
          "\nconst auto tmp" + std::to_string(cnt++) + " = " + name_str + ";";
      names_.emplace_back(std::move(name_str));
    }
  }
};

class MacroValueVisitor : public RecursiveASTVisitor<MacroValueVisitor> {
  size_t nbr_;
  ValVecTy& values_;

 public:
  explicit MacroValueVisitor(size_t nbr, ValVecTy& values)
      : nbr_(nbr), values_(values) {
    values.resize(nbr);
  }
  bool VisitVarDecl(VarDecl* decl) {
    llvm::StringRef name = decl->getName();
    if (!name.startswith("tmp")) {
      return true;
    }
    llvm::APInt res;
    auto ok = name.substr(3).getAsInteger((unsigned)10, res);
    if (ok) {
      return true;
    }
    uint64_t num = res.getLimitedValue(nbr_);
    if (num == nbr_) {
      return true;
    }
    values_[num].first = decl->evaluateValue();
    values_[num].second = decl->getType().getTypePtr();
    return true;
  }
};

class MacroValueConsumer : public ASTConsumer {
  CompilerInstance& ci_;
  std::vector<std::string>& names_;
  ValVecTy& values_;
  MacroValueVisitor visitor_;

 public:
  explicit MacroValueConsumer(CompilerInstance& ci,
                              std::vector<std::string>& names, ValVecTy& values)
      : ci_(ci),
        names_(names),
        values_(values),
        visitor_(names.size(), values) {
    values_.resize(names_.size());
  }
  void HandleTranslationUnit(ASTContext& context) override {
    visitor_.TraverseDecl(context.getTranslationUnitDecl());
    for (size_t idx = 0, idx1 = names_.size(); idx < idx1; ++idx) {
      APValue* apv = values_[idx].first;
      const Type* ty = values_[idx].second;
      llvm::outs() << names_[idx] << ": ";
      if (apv->isInt()) {
        (void)ty;
        // TODO: output integer value correctly based on type
        llvm::APSInt& apsint = apv->getInt();
        llvm::outs() << apsint.getExtValue() << "\n";
      } else if (apv->isLValue()) {
        APValue::LValueBase base = apv->getLValueBase();
        const Expr* expr = base.get<const Expr*>();
        const StringLiteral* str_lit = llvm::dyn_cast<StringLiteral>(expr);
        llvm::outs() << str_lit->getString() << "\n";
      }
    }
  }
};

class MacroDefinitionAction : public ASTFrontendAction {
  std::string& output_;
  std::vector<std::string>& names_;

 public:
  explicit MacroDefinitionAction(std::string& output,
                                 std::vector<std::string>& names)
      : output_(output), names_(names) {}
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci,
                                                 llvm::StringRef) override {
    return llvm::make_unique<MacroDefinitionConsumer>(ci, output_, names_);
  }
};

class MacroValueAction : public ASTFrontendAction {
  std::vector<std::string>& names_;
  ValVecTy& values_;

 public:
  explicit MacroValueAction(std::vector<std::string>& names, ValVecTy& values)
      : names_(names), values_(values) {}
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& ci,
                                                 llvm::StringRef) {
    return llvm::make_unique<MacroValueConsumer>(ci, names_, values_);
  }
};

int main(int argc, char* argv[]) {
  std::string file1(argv[1]);
  std::vector<std::string> names;
  ValVecTy values;
  if (argc > 1) {
    tooling::runToolOnCode(new MacroDefinitionAction(file1, names), argv[1]);
    values.resize(names.size());
    tooling::runToolOnCode(new MacroValueAction(names, values), file1);
  }
}

