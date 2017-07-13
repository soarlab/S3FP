

S3FP_DIR = ../../../src
CC = g++


all: compile 


compile: ca_32 ca_64 ca_128 


ca_32: ca.cpp 
	$(CC) -DFT=float -o ca_32 ca.cpp -lm -lquadmath 


ca_64: ca.cpp 
	$(CC) -DFT=double -o ca_64 ca.cpp -lm -lquadmath 


ca_128: ca.cpp 
	$(CC) -DFT=__float128 -o ca_128 ca.cpp -lm -lquadmath 


N_VARS = 2 

TIMEOUT=1000
RESOURCE=SVE
RSEED=0

ERR_FUNC = REL 
ERR_OPT = LAST 

REL_DELTA=0.001
CHECK_UNSTABLE_ERROR=false
INPUT_RANGE_FILE = rh_ranges 
INPUTLB=1.0
INPUTUB=100.0

INPUT_FILE = random_input 
EXE_LP = ca_32
EXE_HP = ca_128 
OUTPUT_LP = output_32
OUTPUT_HP = output_128 


create-base-conf:
	@echo "==== generate s3fp_setting ====" 
	@echo N_VARS = $(N_VARS) > s3fp_setting 
	@echo TIMEOUT = $(TIMEOUT) >> s3fp_setting
	@echo RESOURCE = $(RESOURCE) >> s3fp_setting
	@echo RSEED = $(RSEED) >> s3fp_setting
	@echo ERR_FUNC = $(ERR_FUNC) >> s3fp_setting 
	@echo ERR_OPT = $(ERR_OPT) >> s3fp_setting 
	@echo REL_DELTA = $(REL_DELTA) >> s3fp_setting
	@echo UNIFORM_INPUT = false >> s3fp_setting 
	@echo INPUT_RANGE_FILE = $(INPUT_RANGE_FILE) >> s3fp_setting 
#	@echo UNIFORM_INPUT = true >> s3fp_setting
#	@echo UNIFORM_INPUTLB = $(INPUTLB) >> s3fp_setting
#	@echo UNIFORM_INPUTUB = $(INPUTUB) >> s3fp_setting
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
	-rm *.o 
	-rm ca_32 
	-rm ca_64 
	-rm ca_128 
	-rm __outdump
	-rm best_input_* 
	-rm fft-gpu_input
	-rm fft-gpu_output_*
	-rm $(INPUT_FILE) 
	-rm $(OUTPUT_LP) $(OUTPUT_HP) 
	-rm s3fp_setting 

