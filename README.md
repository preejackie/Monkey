# MonkeyJIT

This repo contains both LLVM IR and Object Code from both LLVM AOT compiler and LLVM JIT compiler.

The aim is to observe what happens under the hood when the instrumented LLVM IR is compiled by JIT.

Files:

IR files are generated from clang by this command:

clang++ -S -emit-llvm -fprofile-generate sourcecode.cpp -o aot_inst_1.ll

clang++ -S -emit-llvm sourcecode.cpp -o aot_noinst_1.ll

These {aot_inst_1.ll , aot_noinst_1.ll} files are parsed by the JIT, and compiled native code is dumped to the disk by object transform layer.

With initial testing, it seems like JIT drops the profile intrinsics completely. That is, there is no calls to support functions in compiler-rt library in generated native code.

And also, JIT generates different object code for module that is instrumented (-fprofile-generate). 
You can find this difference, in jitmaindiff.png [diff image](https://github.com/preejackie/Monkey/blob/master/jitmaindiff.png). 

In AOT compiler - object code generated for instrumented IR is 5019 lines long. You can find it [here](https://github.com/preejackie/Monkey/blob/master/aot_inst_dis)
