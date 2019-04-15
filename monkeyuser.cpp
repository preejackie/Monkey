#include "MonkeyJIT.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"


#include "llvm/ExecutionEngine/ExecutionEngine.h"

using namespace llvm;
using namespace llvm::orc;

int main()
{
    InitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    using JTMB = JITTargetMachineBuilder;
    auto  jtmb = JTMB::detectHost();
    if (!jtmb)
         jtmb.takeError();
    
    auto DL = jtmb->getDefaultDataLayoutForTarget();
    if(!DL)
        DL.takeError();
    
    auto jit = MonkeyJIT::Create(*jtmb,*DL);
    if(!jit)
        jit.takeError();
    
    LLVMContext ContextIn;

    SMDiagnostic error;
    std::unique_ptr<Module> Module = parseIRFile("non_instru.ll",error,ContextIn);
    ThreadSafeContext TCtx(llvm::make_unique<LLVMContext>());
	ThreadSafeModule  TMod(std::move(Module),TCtx);

    auto Rerror = (*jit)->addIRModuletoJDL(std::move(TMod));
    auto Result = (*jit)->lookup("main");
    if(!Result)
        Result.takeError();
    
    using FuncTy = int (void);
	auto JITAddr = (*Result).getAddress();
	auto v = ((FuncTy *) JITAddr) ();
	llvm::outs() << v;

    return 0;
}