// This file is a part of Julia. License is MIT: https://julialang.org/license

#ifndef LLVM_LATE_GC_HELPERS_H
#define LLVM_LATE_GC_HELPERS_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

// A data structure that contains references to platform-agnostic GC lowering
// types, metadata and functions.
struct GCLoweringRefs {
    llvm::Type *T_prjlvalue;
    llvm::Type *T_ppjlvalue;
    llvm::Type *T_size;
    llvm::Type *T_int8;
    llvm::Type *T_int32;
    llvm::Type *T_pint8;
    llvm::Type *T_pjlvalue;
    llvm::Type *T_pjlvalue_der;
    llvm::Type *T_ppjlvalue_der;
    llvm::MDNode *tbaa_gcframe;
    llvm::MDNode *tbaa_tag;
    llvm::Function *ptls_getter;
    llvm::Function *gc_flush_func;
    llvm::Function *gc_preserve_begin_func;
    llvm::Function *gc_preserve_end_func;
    llvm::Function *pointer_from_objref_func;
    llvm::Function *alloc_obj_func;
    llvm::Function *typeof_func;
    llvm::Function *write_barrier_func;
    llvm::Function *push_new_gc_frame_func;

    // Creates a GC lowering refs structure. Type and function pointers
    // are set to `nullptr`. Metadata nodes are initialized.
    GCLoweringRefs();

    // Populates a GC lowering refs structure by inspecting a module.
    void initAll(llvm::Module &M);

    // Initializes a GC lowering refs structure's functions only.
    void initFunctions(llvm::Module &M);

    // Gets a call to the `julia.ptls_states` intrinisc in the entry
    // point of the given function, if there exists such a call.
    // Otherwise, `nullptr` is returned.
    llvm::CallInst *getPtls(llvm::Function &F) const;
};

// A namespace for getting and creating intrinsics.
namespace jl_intrinsics {
    // Gets or creates an intrinsic that creates and pushes a new GC frame.
    // The intrinsic gets added to the module if it doesn't exist already.
    llvm::Function* getOrDefinePushNewGCFrame(const GCLoweringRefs &refs, llvm::Module &M);
}

#endif
