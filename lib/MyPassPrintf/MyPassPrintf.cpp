#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Constants.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/InstIterator.h"
#include <iostream>
#include <cstdio>
#include "llvm/IR/Operator.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <list>
#include <stdio.h>

using namespace std;
using namespace llvm;

namespace{
    struct MyPassPrintf : public ModulePass{
        static char ID;
        FILE *fstream;
        Value *pFile;
        StructType* IO_FILE_ty;
        Type *IO_FILE_PTR_ty;
        Value *FPrintf;
        CallInst* ptr_call;
        int counter=0;


        MyPassPrintf(): ModulePass(ID) {}

        /* define a extern function "fprintf"
         * int fprintf ( FILE * stream, const char * format, ... ); */
         virtual Function* getfprint(Module &mod, LLVMContext& ctx , vector<Value*> args) {
            vector<Type*> argsTypes;

            // push type of FILE*
            argsTypes.push_back(IO_FILE_PTR_ty);
            // push rest arguments of printf instruction
            for (unsigned i = 1, e = args.size(); i != e; i++) {
                argsTypes.push_back(args[i]->getType());
            }

            /* create fprintf function */
            FPrintf = mod.getOrInsertFunction("fprintf",
                                        FunctionType::get(Type::getInt32Ty(ctx),
                                                        argsTypes,
                                                        true));

            Function *func_fprintf = cast<Function>(FPrintf);
            func_fprintf->setCallingConv(CallingConv::C);

            errs()<< "func_fprintf:"<<*func_fprintf <<"\n";

            return func_fprintf;
        }

        virtual bool insertOnMainEntryBlock(BasicBlock &F, Module &M) {
            // Returns a pointer to the first instruction in this block
            // that is not a PHINode instruction
            Instruction *inst = F.getFirstNonPHI();

            if(dyn_cast<AllocaInst> (inst)) {
                errs() << "INSERT INTO MAIN ENTRY"<< *inst <<"\n";

                /* FILE * pFile; */
                IO_FILE_ty = StructType::create(M.getContext(), "struct._IO_FILE");
                IO_FILE_PTR_ty = PointerType::getUnqual(IO_FILE_ty);

                StructType* IO_marker_ty = StructType::create(M.getContext(), "struct._IO_marker");
                PointerType* IO_marker_ptr_ty = PointerType::getUnqual(IO_marker_ty);

                std::vector<Type*> Elements;
                Elements.push_back(IO_marker_ptr_ty);
                Elements.push_back(IO_FILE_PTR_ty);
                IO_FILE_ty->setBody(Elements, false);

                std::vector<Type*> Elements2;
                Elements2.push_back(IO_marker_ptr_ty);
                Elements2.push_back(IO_FILE_PTR_ty);
                Elements2.push_back(Type::getInt32Ty(M.getContext()));;
                IO_marker_ty->setBody(Elements2, false);

                pFile = new GlobalVariable(M,
                                        IO_FILE_PTR_ty,
                                        false,
                                        GlobalValue::ExternalLinkage,
                                        NULL,
                                        "pFile");

                errs() << "pFile" << *pFile<<  "\n";

                /* FILE * fopen ( const char * , const char *  ); */
                std::vector<Type*> Params;
                Params.push_back(PointerType::getUnqual(Type::getInt8PtrTy(F.getContext())));
                Params.push_back(PointerType::getUnqual(Type::getInt8PtrTy(F.getContext())));

                Value* const_fopen = M.getOrInsertFunction("fopen",
                                                            IO_FILE_PTR_ty,
                                                            Type::getInt8PtrTy(F.getContext()),
                                                            Type::getInt8PtrTy(F.getContext()),
                                                            NULL);

                Function *func_fopen = cast<Function>(const_fopen);

                errs()<< "func_fopen:"<<*func_fopen <<"\n";

                //
                //Create a global string variable with the file's name
                //
                Constant* strfileConstant = ConstantDataArray::getString(
                                                                M.getContext(),
                                                                "test.txt",
                                                                true);

                GlobalVariable* fileStr = new GlobalVariable(M,
                                                    strfileConstant->getType(),
                                                    true,
                                                    llvm::GlobalValue::InternalLinkage,
                                                    strfileConstant,
                                                    "testTxt");
                errs()<< "fileStr:"<<*fileStr <<"\n";

                //Get the int8ptr to our message
                Constant* constZeroF = ConstantInt::get(Type::getInt32Ty(M.getContext()), 0);
                Constant* constArrayF = ConstantExpr::getInBoundsGetElementPtr(fileStr, constZeroF);
                Value* filePtr = ConstantExpr::getBitCast(constArrayF, PointerType::getUnqual(Type::getInt8Ty(M.getContext())));

                //
                //Create a global strin g variable with the format's name
                //
                Constant* strFormatConstant = ConstantDataArray::getString( M.getContext(),
                                                                            "w+",
                                                                            true);

                errs()<< "strFormatConstant:"<<*strFormatConstant <<"\n";

                GlobalVariable* formatStr = new GlobalVariable(M,
                                                strFormatConstant->getType(),
                                                true,
                                                llvm::GlobalValue::InternalLinkage,
                                                strFormatConstant,
                                                "w+");

                //Get the int8ptr to our message
                Constant* constZeroFmt  = ConstantInt::get(Type::getInt32Ty(M.getContext()), 0);
                Constant* constArrayFmt = ConstantExpr::getInBoundsGetElementPtr(formatStr, constZeroFmt);
                Value* fmtPtr = ConstantExpr::getBitCast(constArrayFmt, PointerType::getUnqual(Type::getInt8Ty(M.getContext())));

                vector<Value*> args;
                args.push_back(filePtr);
                args.push_back(fmtPtr);

                pFile = CallInst::Create(func_fopen, args, "", inst->getNextNode());
                errs()<< "pFile:"<<*pFile <<"\n";

            }
            return true;
        }

        virtual bool insertOnMainEndBlock(BasicBlock &F, Module &M) {

            Instruction *inst = F.getTerminator();
            errs() << "INSERT INTO MAIN END"<< *inst <<"\n";

            /* int fclose(FILE*); */
            Value* const_fclose = M.getOrInsertFunction("fclose",
                                                Type::getInt32Ty(F.getContext()),
                                                        IO_FILE_PTR_ty,
                                                        NULL);

            Function *func_fclose = cast<Function>(const_fclose);

            errs()<< "func_fclose:"<<*func_fclose <<"\n";

            CallInst* int32_call3 = CallInst::Create(func_fclose,
                                                    pFile,
                                                    "",
                                                    inst);

            errs()<< "int32_call3:"<<*int32_call3 <<"\n";

            int32_call3->setCallingConv(CallingConv::C);
            int32_call3->setTailCall(true);

            return true;
        }

        virtual bool runOnModule(Module &M) {
            for(Module::iterator F = M.begin(), E = M.end(); F!= E; ++F) {

                for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
                    // We know we've encountered a main module
                    // we get the entry block of it
                    if( (F->getName() == "main") ) {
                        insertOnMainEntryBlock(F->getEntryBlock(), M);
                    }

                    MyPassPrintf::runOnBasicBlock(BB, M);

                    // We know we've encountered a main module
                    // we get the end block of it
                    if (F->getName()=="main" && isa<ReturnInst>(BB->getTerminator())) {
                        insertOnMainEndBlock(*BB, M);
                    }
                }
            }
            return false;
        }

        Instruction* getNextInstruction(Instruction& i) {
            BasicBlock::iterator it(&i);
        //    it++;
            return it;
        }

        virtual bool runOnBasicBlock(Function::iterator &BB, Module &M) {
            LLVMContext& ctx = getGlobalContext();

            for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
                Instruction* I = (Instruction*) dyn_cast<Instruction>(BI);

                if (CallInst *CI = dyn_cast<CallInst>(&*BI)) {
                    Function *CF = CI->getCalledFunction();
                    // We know we've encountered a call instruction, so we
                    // need to determine if it's a call to printf or not.
                    if(CF->getName() == "printf") {
                        errs()<< ".. found printf: "<<*CF << "\n";
                        ++counter;
                        errs()<< counter<< "\n";

                        vector<Value*> args;
                        args.push_back(pFile);

                        // get arguments of printf instruction
                        CallSite CS(CI);
                        for(User::op_iterator i = CS->op_begin(), e = CS->op_end(); i != e; ++i) {
                            args.push_back(i->get());
                        }

                        // insert 'fprintf' instr after it
                        Function* funcFprintf = getfprint(M, ctx, args);

                        // create a call instr and insert it after every printf
                        Instruction* nextInstruction = getNextInstruction(*I);

                        CallInst* fprintfCall = CallInst::Create(funcFprintf,
                                                                args,
                                                                "",
                                                                nextInstruction);
                        fprintfCall->setCallingConv(CallingConv::C);
                        fprintfCall->setTailCall(true);

                        errs()<<"fprintfCall:"<< *fprintfCall <<"\n";
                    }
                }
            }
            return true;
        }
    };
}

char MyPassPrintf::ID = 0;
static RegisterPass<MyPassPrintf> X("MyPassPrintf", "MyPassPrintf Pass", false, false);
