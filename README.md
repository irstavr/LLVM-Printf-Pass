# llvm_pass

For a taste of the most widely used compilers, we implemented a simple pass  
inside LLVM to output on a file everything that is printf­ed on the stdout. 

LLVM 
 
Install Clang + LLVM 
 
LLVM (http://www.llvm.org) is a modern compiler infrastructure widely used in 
both academia and industry. CLang is a LLVM front end. Here we use Clang+LLVM 3.4. 
It is not the most recent LLVM but it was sufficient for our project. 
 
We checked out Clang+LLVM source code, compiled and installed it on our system. 
 
 
Add the MyPassPrintf pass 
 
In the path /lib/MyPassPrintf you are provided with a file MyPassPrintf.cpp, 
which has code to first add a call fopen() at the beginning of the main function, 
then add a call fprintf() after each printf() with the same arguments to write 
to the file whatever is printf­ed on the stdout; and finally add a call fclose() 
at the exit of the program. 
 
Put this file into {llvm­project}/{llvm­source­code}/lib/Transforms/MyPassPrintf 
 
For testing, we use the test file in directory 
/build/lib/Transforms/MyPassPrintf/mypasstest.c
