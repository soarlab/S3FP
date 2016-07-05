
CC=g++

S3FP_DIR=../../../src

all: compile


N_VARS = 4 
TIMEOUT = 100000
RESOURCE = SVE # or TIME
RSEED = 0
ERR_FUNC = ABS
REL_DELTA = 0.001
UNIFORM_INPUTLB = 1.0 
UNIFORM_INPUTUB = 100.0

INPUT_FILE = input_file
EXE_LP = abcd-32
EXE_HP = abcd-128 
OUTPUT_LP = abcd_32_output
OUTPUT_HP = abcd_128_output


compile: abcd-32 abcd-128 

abcd-32: 
	$(CC) -DFT=float -o abcd-32 abcd.cpp 

abcd-64: 
	$(CC) -DFT=double -o abcd-64 abcd.cpp 

abcd-128: 
	$(CC) -DFT=__float128 -o abcd-128 abcd.cpp 


create-base-conf:
	@echo "==== generate s3fp_setting ====" 
	@echo N_VARS = $(N_VARS) > s3fp_setting
	@echo TIMEOUT = $(TIMEOUT) >> s3fp_setting
	@echo RESOURCE = $(RESOURCE) >> s3fp_setting
	@echo RSEED = $(RSEED) >> s3fp_setting
	@echo ERR_FUNC = $(ERR_FUNC) >> s3fp_setting
	@echo ERR_OPT = LAST >> s3fp_setting 
	@echo REL_DELTA = $(REL_DELTA) >> s3fp_setting
	@echo UNIFORM_INPUT = true >> s3fp_setting
	@echo UNIFORM_INPUTLB = $(UNIFORM_INPUTLB) >> s3fp_setting
	@echo UNIFORM_INPUTUB = $(UNIFORM_INPUTUB) >> s3fp_setting 
	@echo INPUT_FILE = $(INPUT_FILE) >> s3fp_setting 
	@echo EXE_LP = $(EXE_LP) >> s3fp_setting 
	@echo EXE_HP = $(EXE_HP) >> s3fp_setting 
	@echo OUTPUT_LP = $(OUTPUT_LP) >> s3fp_setting 
	@echo OUTPUT_HP = $(OUTPUT_HP) >> s3fp_setting 

test-urt: create-base-conf 
	@echo RT = URT >> s3fp_setting 
	@echo "==== run s3fp ===="
	@$(S3FP_DIR)/s3fp 

test-bgrt: create-base-conf 
	@echo RT = BGRT >> s3fp_setting 
	@echo "==== run s3fp ===="
	@$(S3FP_DIR)/s3fp 

test-ils: create-base-conf 
	@echo RT = ILS >> s3fp_setting 
	@echo "==== run s3fp ===="
	@$(S3FP_DIR)/s3fp 

test-pso: create-base-conf 
	@echo RT = PSO >> s3fp_setting 
	@echo "==== run s3fp ===="
	@$(S3FP_DIR)/s3fp 

clean:
	-rm *~
	-rm s3fp_setting 
	-rm abcd-32 abcd-64 abcd-128 
	-rm abcd_32_output abcd_64_output abcd_128_output 
	-rm $(INPUT_FILE) 
	-rm __outdump
	-rm best_input_*
	-rm *-input-ranges