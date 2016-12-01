
CC = gcc
LEX_SUBFIX=.lex
BISION_SUBFIX=.y
C_OBJECT_FLEX_SUBFIX=.flex.yy.c
C_OBJECT_BISION_SUBFIX=.yy.c
H_OBJECT_BISION_SUBFIX=.yy.h



LEX_SOURCE = $(wildcard *$(LEX_SUBFIX))
C_OBJECT_FLEX = $(LEX_SOURCE:$(LEX_SUBFIX)=$(C_OBJECT_FLEX_SUBFIX))


C_OBJECT_BISON = $(LEX_SOURCE:$(LEX_SUBFIX)=$(C_OBJECT_BISION_SUBFIX))
H_OBJECT_BISON = $(LEX_SOURCE:$(LEX_SUBFIX)=$(H_OBJECT_BISION_SUBFIX))


FINAL_PROGRAM = $(LEX_SOURCE:$(LEX_SUBFIX)=)

.PHONY : all init 

all: init $(C_OBJECT_BISON) $(C_OBJECT_FLEX) $(FINAL_PROGRAM) finish
	@echo DONE!!!!!!!


init: 
	@if [ ! -d ./obj ]; then mkdir -p ./obj ;fi
	

$(C_OBJECT_BISON) : %$(C_OBJECT_BISION_SUBFIX) : %$(BISION_SUBFIX) 
	echo $(?)
	bison -tvd -o $(@) $(?) 
	@echo "finished to compile through BISON"

$(C_OBJECT_FLEX) : %$(C_OBJECT_FLEX_SUBFIX) : %$(LEX_SUBFIX) 
	flex -o $(@) $(?)
	@echo "finished to compile LEX"


$(FINAL_PROGRAM) : % : %.c
	gcc -o $(@) $(@)$(C_OBJECT_FLEX_SUBFIX) $(@)$(C_OBJECT_BISION_SUBFIX)
	@echo "finished to compile C"

finish:
	mv *$(C_OBJECT_FLEX_SUBFIX) ./obj
	mv *$(C_OBJECT_BISION_SUBFIX) ./obj
	mv *$(H_OBJECT_BISION_SUBFIX) ./obj


.PHONY : clean

clean:
	@rm -f *.c
	@rm -f *.yy.output
	@rm -f *.h
	@rm -f $(FINAL_PROGRAM)
	@rm -rf ./obj


