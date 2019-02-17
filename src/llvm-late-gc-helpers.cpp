#include <llvm/IR/Function.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "llvm-version.h"
#include "codegen_shared.h"
#include "llvm-late-gc-helpers.h"

using namespace llvm;

void GCLoweringRefs::initFunctions(Module &M) {
    ptls_getter = M.getFunction("julia.ptls_states");
    gc_flush_func = M.getFunction("julia.gcroot_flush");
    gc_preserve_begin_func = M.getFunction("llvm.julia.gc_preserve_begin");
    gc_preserve_end_func = M.getFunction("llvm.julia.gc_preserve_end");
    pointer_from_objref_func = M.getFunction("julia.pointer_from_objref");
    typeof_func = M.getFunction("julia.typeof");
    write_barrier_func = M.getFunction("julia.write_barrier");
    alloc_obj_func = M.getFunction("julia.gc_alloc_obj");
}

void GCLoweringRefs::initAll(Module &M) {
    initFunctions(M);

    auto &ctx = M.getContext();
    T_size = M.getDataLayout().getIntPtrType(ctx);
    T_int8 = Type::getInt8Ty(ctx);
    T_pint8 = PointerType::get(T_int8, 0);
    T_int32 = Type::getInt32Ty(ctx);
    if (write_barrier_func) {
        T_prjlvalue = write_barrier_func->getFunctionType()->getParamType(0);
    }
    if (alloc_obj_func) {
        T_prjlvalue = alloc_obj_func->getReturnType();
        auto T_jlvalue = cast<PointerType>(T_prjlvalue)->getElementType();
        T_pjlvalue = PointerType::get(T_jlvalue, 0);
        T_ppjlvalue = PointerType::get(T_pjlvalue, 0);
        T_pjlvalue_der = PointerType::get(T_jlvalue, AddressSpace::Derived);
        T_ppjlvalue_der = PointerType::get(T_prjlvalue, AddressSpace::Derived);
    }
    else if (ptls_getter) {
        auto functype = ptls_getter->getFunctionType();
        T_ppjlvalue = cast<PointerType>(functype->getReturnType())->getElementType();
        T_pjlvalue = cast<PointerType>(T_ppjlvalue)->getElementType();
        auto T_jlvalue = cast<PointerType>(T_pjlvalue)->getElementType();
        T_prjlvalue = PointerType::get(T_jlvalue, AddressSpace::Tracked);
        T_pjlvalue_der = PointerType::get(T_jlvalue, AddressSpace::Derived);
        T_ppjlvalue_der = PointerType::get(T_prjlvalue, AddressSpace::Derived);
    }
    else {
        T_ppjlvalue = nullptr;
        T_prjlvalue = nullptr;
        T_pjlvalue = nullptr;
        T_pjlvalue_der = nullptr;
        T_ppjlvalue_der = nullptr;
    }
}
