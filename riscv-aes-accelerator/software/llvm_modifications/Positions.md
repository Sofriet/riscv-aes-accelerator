# Definition of the new major opcode 0x6B (OPC_CUSTOM_4), used by RISCVInstrInfoZk.td.
# This file is not in this folder yet: copy your changed version here.
~/llvm/llvm/lib/Target/RISCV/RISCVInstrFormats.td

# Semantic checking for the C intrinsic. Just checks if round_num == [1, 10]
~/llvm/clang/lib/Sema/SemaRISCV.cpp

# Definition of the RISCV builtin at the frontend and inputs/outputs
~/llvm/clang/include/clang/Basic/BuiltinsRISCV.td

# Definition of the LLVM instrinsic and its properties for optimization
~/llvm/llvm/include/llvm/IR/IntrinsicsRISCV.td

# Definition of the new custom instruction (F function)
# Definition of the pattern to generate the instruction (only instrinsic)
~/llvm/llvm/lib/Target/RISCV/RISCVInstrInfoZk.td
