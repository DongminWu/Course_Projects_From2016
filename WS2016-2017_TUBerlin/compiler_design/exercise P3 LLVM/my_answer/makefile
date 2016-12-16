CC = gcc
C_SOURCE = $(wildcard *.c)
BC_OBJECT = $(C_SOURCE:.c=.bc)
S_OBJECT = $(C_SOURCE:.c=.s)
FINAL_PROGRAM = $(C_SOURCE:.c=)

.PHONY : all init 

all: init $(BC_OBJECT) $(S_OBJECT) $(FINAL_PROGRAM) finish
	@echo DONE!!!!!!!


init: 
	@echo hello!
	


$(BC_OBJECT) : %.bc : %.c 
	clang $(?) -o $(@) -c -emit-llvm 
	@echo "finished to compile LLVM IR bitcode"

$(S_OBJECT) : %.s : %.bc 
	/usr/local/Cellar/llvm/3.9.0/bin/llc -o $(@) $(?)
	@echo "finished to compile Bytecode"

$(FINAL_PROGRAM) : % : %.s
	gcc -o $(@) $(?)
	@echo "finished to compile C"

finish:
	@echo "finished"


.PHONY : clean

clean:
	@rm -f *.bc
	@rm -f *.s
	@rm -f $(FINAL_PROGRAM)


