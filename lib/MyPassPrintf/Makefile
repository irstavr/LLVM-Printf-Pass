#===- lib/Transforms/MyPassPrintf/Makefile -----------------------*-
#     Makefile -*-===##
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMMyPassPrintf
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
