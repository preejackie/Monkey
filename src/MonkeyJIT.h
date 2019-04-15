#include "llvm/ExecutionEngine/Orc/Layer.h"
#include "llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/IRTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/ObjectTransformLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/Support/ThreadPool.h"

#include <functional>
/* 
    What we want:
    1. Ability to add IR to JIT
    2. Compile the IR to native code
    3. dump native code to disk
*/

// We are replicating the LLJIT stack, but with a object transform layer
// Minimal Public interface for client JIT stack can be  {addModule, look} methods.
namespace llvm
{
namespace orc {
class MonkeyJIT
{
public:
~MonkeyJIT();

static Expected<std::unique_ptr<MonkeyJIT>> Create(JITTargetMachineBuilder JTMB, DataLayout DL,
unsigned compilerthreads = 0);

ExecutionSession&  getExecutionSession() {return *ES;}

JITDylib& getMainJITDylib() { return maindylib;}

//Why returing reference ? 
JITDylib& createJITDylibwithName(std::string Name)
{
   return ES->createJITDylib(Name);
}

Error addIRModuletoJDL(JITDylib& JDL, ThreadSafeModule TSM);

Error addIRModuletoJDL(ThreadSafeModule TSM)
{
    return addIRModuletoJDL(maindylib,std::move(TSM));
}

Error addObjectFiletoJDL(JITDylib& JDL, std::unique_ptr<MemoryBuffer> objmem);

Error addObjectFiletoJDL(std::unique_ptr<MemoryBuffer> objmem)
{
    return addObjectFiletoJDL(maindylib,std::move(objmem));
}

// look for mangled symbol names 
Expected<JITEvaluatedSymbol> lookmangledname(JITDylib& JD,
StringRef sym_name);
// look for unmangled symbol names
Expected<JITEvaluatedSymbol> lookup(StringRef sym_name)
{
    return lookmangledname(maindylib, do_mangling(sym_name));
}

// why these methods ? 
Error runCtors() {return CtorsR.run();}
Error runDtors() {return DtorsR.run();}

IRCompileLayer& getCompileLayer()
{
    return Compiler;
}

ObjectTransformLayer& getObjectDumper() {
    return ObjectDumper;
}

RTDyldObjectLinkingLayer& getLinkLayer() {
    return Linker;
}

private:

MonkeyJIT(std::unique_ptr<ExecutionSession> ES, 
std::unique_ptr<TargetMachine> TM, DataLayout DL);

MonkeyJIT(std::unique_ptr<ExecutionSession> ES,
JITTargetMachineBuilder JTMB, DataLayout DL, 
unsigned CompilerThrds = 0);

static Expected<std::unique_ptr<MemoryBuffer>> DumptoDisk(std::unique_ptr<MemoryBuffer> obj){
        std::string cacheName ("NativeCode.o");
        std::error_code EC;
        raw_fd_ostream outfile(cacheName,EC,sys::fs::F_None);
        outfile.write(obj->getBufferStart(),obj->getBufferSize());
        outfile.close();
        return obj;
}

std::string do_mangling(StringRef sym_name);

Error applyDataLayoutFor(Module & M);

void RecordsCtorsDtorsFor(Module& M);

std::unique_ptr<ExecutionSession> ES;
JITDylib& maindylib;

DataLayout DL;
std::unique_ptr<ThreadPool> CompileThreads; 

IRCompileLayer              Compiler; 
ObjectTransformLayer        ObjectDumper; 
RTDyldObjectLinkingLayer    Linker;

using FContainerTy = std::function<Expected<std::unique_ptr<MemoryBuffer>>(
          std::unique_ptr<MemoryBuffer>)>;
    
CtorDtorRunner CtorsR, DtorsR;
};

}}