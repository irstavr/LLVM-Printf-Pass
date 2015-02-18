#===- lib/Transforms/MyPassPrintf/Makefile -----------------------*-
#     Makefile -*-===##
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = MyPassPrintf
LOADABLE_MODULE = 1
USEDLIBS =

# If we don't need RTTI or EH, there's no reason to export anything
# from the MyPassPrintf plugin.
ifneq ($(REQUIRES_RTTI), 1)
ifneq ($(REQUIRES_EH), 1)
EXPORTED_SYMBOL_FILE = $(PROJ_SRC_DIR)/MyPassPrintf.exports
endif
endif

include $(LEVEL)/Makefile.common


pass:
	opt -load ../../../Release+Asserts/lib/MyPassPrintf.so -MyPassPrintf < mypasstest.ll > output.ll

test:
	clang mypasstest.c -o mypasstest
	clang -o3 -emit-llvm mypasstest.c -c -o mypasstest.bc
	clang -emit-llvm -c mypasstest.c -o mypasstest.ll
	llc -march=cpp mypasstest.ll -o mypasstest.ll.cpp


runC:
	./mypasstest

runBC:
	lli output.ll

clear:
	rm mypasstest mypasstest.bc mypasstest.ll mypasstest.ll.cpp output.ll output.ll.cpp
