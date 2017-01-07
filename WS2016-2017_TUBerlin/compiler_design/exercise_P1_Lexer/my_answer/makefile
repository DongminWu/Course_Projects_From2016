
CC = gcc
LEX_SOURCE = $(wildcard *.l)
C_OBJECT = $(LEX_SOURCE:.l=.yy.c)
FINAL_PROGRAM = $(LEX_SOURCE:.l=)

.PHONY : all init 

all: init $(C_OBJECT) $(FINAL_PROGRAM) finish
	@echo DONE!!!!!!!


init: 
	@if [ ! -d /directory/to/check ]; then mkdir -p ./obj ;fi
	


$(C_OBJECT) : %.yy.c : %.l 
	flex -o $(@) $(?)
	@echo "finished to compile LEX"


$(FINAL_PROGRAM) : % : %.yy.c
	gcc -o $(@) $(?)
	@echo "finished to compile C"

finish:
	mv *.yy.c ./obj


.PHONY : clean

clean:
	@rm -f *.yy.c
	@rm -f $(FINAL_PROGRAM)
	@rm -rf ./obj


