#include "MonkeyJIT.h"
#include "llvm/ExecutionEngine/Orc/OrcError.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/Mangler.h"



namespace{
    class SimpleCompilerToEmitMC : public llvm::orc::SimpleCompiler{
        public:
            SimpleCompilerToEmitMC(std::unique_ptr<llvm::TargetMachine> TM)
            : llvm::orc::SimpleCompiler(*TM), TM(std::move(TM)) {}
        private:
            std::shared_ptr<llvm::TargetMachine> TM;
    };
}
namespace llvm
{
    namespace orc
    {
        // why compilers have to run when this instance going to die
    MonkeyJIT::~MonkeyJIT()  {
            if(CompileThreads)
                CompileThreads->wait();
        }
    
    Expected<std::unique_ptr<MonkeyJIT>>
    MonkeyJIT::Create(JITTargetMachineBuilder JTMB, DataLayout DL,
              unsigned compilerthreads) {

        if (compilerthreads == 0) {
        // If NumCompileThreads == 0 then create a single-threaded LLJIT instance.
        auto TM = JTMB.createTargetMachine();
        if (!TM)
            return TM.takeError();
        return std::unique_ptr<MonkeyJIT>(new MonkeyJIT(llvm::make_unique<ExecutionSession>(),
                                            std::move(*TM), std::move(DL)));
        }

        return std::unique_ptr<MonkeyJIT>(new MonkeyJIT(llvm::make_unique<ExecutionSession>(),
                                          std::move(JTMB), std::move(DL),
                                          compilerthreads));   
    }

    Error MonkeyJIT::addIRModuletoJDL(JITDylib& JD, ThreadSafeModule TSM){
        assert (TSM && "Can not add null module");
        // why we need this ? 
        if (auto Err = applyDataLayoutFor(*TSM.getModule()))
            return Err;
        return Compiler.add(JD, std::move(TSM), ES->allocateVModule());
    }

    Error MonkeyJIT::addObjectFiletoJDL(JITDylib& JD,std::unique_ptr<MemoryBuffer> obj_mem){
        assert (obj_mem && "Can not add null object to JITDylib");
        return Linker.add(JD,std::move(obj_mem),ES->allocateVModule());
    }

    Expected<JITEvaluatedSymbol> MonkeyJIT::lookmangledname(JITDylib& JD, StringRef sym_name){
        return ES->lookup(JITDylibSearchList({{&JD, true}}), ES->intern(sym_name));
    }

   

    // single thread variant 
    MonkeyJIT::MonkeyJIT(std::unique_ptr<ExecutionSession> ES,
    std::unique_ptr<TargetMachine> TM, DataLayout DL) :
        ES(std::move(ES)), maindylib(this->ES->getMainJITDylib()),
        DL(std::move(DL)), 
        Linker(*this->ES,[](){
            return llvm::make_unique<SectionMemoryManager>();
        }),
        ObjectDumper(*this->ES,Linker,DumptoDisk),
        Compiler(*this->ES,ObjectDumper,SimpleCompilerToEmitMC(std::move(TM))),
        CtorsR(maindylib), DtorsR(maindylib) {}

    MonkeyJIT::MonkeyJIT(std::unique_ptr<ExecutionSession> ES, JITTargetMachineBuilder JTMB,
             DataLayout DL, unsigned CompilerThrds) :
        ES(std::move(ES)), maindylib(this->ES->getMainJITDylib()), DL(std::move(DL)),
        Linker(*this->ES,
              []() { return llvm::make_unique<SectionMemoryManager>(); }),
        ObjectDumper(*this->ES,Linker,DumptoDisk),
         
        Compiler(*this->ES, Linker,ConcurrentIRCompiler(std::move(JTMB))),
        CtorsR(maindylib), DtorsR(maindylib) {
            assert(CompilerThrds != 0 &&
            "Multithreaded LLJIT instance can not be created with 0 threads");
        Compiler.setCloneToNewContextOnEmit(true);

        // Create a thread pool to compile on and set the execution session
        // dispatcher to use the thread pool.
        CompileThreads = llvm::make_unique<ThreadPool>(CompilerThrds);
        this->ES->setDispatchMaterialization(
            [this](JITDylib &JD, std::unique_ptr<MaterializationUnit> MU) {
            // FIXME: Switch to move capture once we have c++14.
            auto SharedMU = std::shared_ptr<MaterializationUnit>(std::move(MU));
            auto Work = [SharedMU, &JD]() { SharedMU->doMaterialize(JD); };
            CompileThreads->async(std::move(Work));
            });
        }

    std::string MonkeyJIT::do_mangling(StringRef sym_name){
        std::string MangledName;
        raw_string_ostream MangleNameStream(MangledName);
        Mangler::getNameWithPrefix(MangleNameStream,sym_name,DL);
        return MangledName;
    }
    
    // what is this?
    Error MonkeyJIT::applyDataLayoutFor(Module& M){
        if (M.getDataLayout().isDefault())
            M.setDataLayout(DL);

        if (M.getDataLayout() != DL)
            return make_error<StringError>(
                "Added modules have incompatible data layouts",
                inconvertibleErrorCode());

        return Error::success();
    }

    void MonkeyJIT::RecordsCtorsDtorsFor(Module& M){
        CtorsR.add(getConstructors(M));
        DtorsR.add(getDestructors(M));
    }

} }// namespace llvm
