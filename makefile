CC      	= /usr/bin/g++
IFLAGS  	= -I./
LIBS 	  	= -lm

include flags.mk

# Flags for optimized compile
OPTIM    = -O3
CFLAGS_OPTIM  = $(CFLAGS) $(OPTIM)
OBJ_DIR_OPTIM = obj

# Flags for non-optimized compile
CFLAGS 	+= -g -pg -Wall -Wno-reorder -Wno-sign-compare
OBJ_DIR  = obj_debug

SRC_DIR  = src
SRC  		= $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/*/*.cpp)
OBJ    	= $(SRC:.cpp=.o)
DS  		= $(SRC:.cpp=.d)
DEPS     = $(DS:$(SRC_DIR)%=$(OBJ_DIR)%) $(DS:$(SRC_DIR)%=$(OBJ_DIR_OPTIM)%)
HEADERS 	= $(wildcard $(SRC_DIR)/*.h $(SRC_DIR)/*/*.h) 
INCS    	= makefile $(HEADERS)

COMPONENTS = DetMetrics Averagers Calibrators Continuous Correctors Discretes Distributions Downscalers Drivers Estimators Finders Inputs Metrics Outputs ParameterIos Selectors Smoothers Tools Variables VarSelectors Loggers Measures Interpolators

SELECTORS 		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Selectors/*.h))
DOWNSCALERS		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Downscalers/*.h))
INPUTS     		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Inputs/*.h))
DET_METRICS    = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/DetMetrics/*.h))
AVERAGERS  		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Averagers/*.h))
CALIBRATORS    = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Calibrators/*.h))
CONFIGURATIONS = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Configurations/*.h))
CORRECTORS 		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Correctors/*.h))
CONTINUOUS 		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Continuous/*.h))
DISCRETES		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Discretes/*.h))
FINDERS  		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Finders/*.h))
BASEDISTRIBUTIONS 	= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/BaseDistributions/*.h))
ESTIMATORS 		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Estimators/*.h))
METRICS  		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Metrics/*.h))
OUTPUTS  		= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Outputs/*.h))
PARAMETERIOS   = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/ParameterIos/*.h))
VARIABLES      = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Variables/*.h))
UNCERTAINTIES   = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Uncertainties/*.h))
SMOOTHERS      = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Smoothers/*.h))
INTERPOLATORS  = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Interpolators/*.h))
MEASURES       = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Measures/*.h))
LOGGERS        = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Loggers/*.h))
OTHERS         = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Others/*.h))
VAR_SELECTORS  = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/VarSelectors/*.h))
QCS            = $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Qcs/*.h))
CORE 				= $(patsubst src/%.h,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.h))
TESTS				= $(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/Tests/*.cpp))
EXE_TESTS		= $(patsubst src/Tests/%.cpp,Tests/%.exe,$(wildcard $(SRC_DIR)/Tests/*.cpp))

default : comps_debug.exe comps.exe
all     : comps convert info
comps:    comps.exe comps_debug.exe
anVars:   anVars.exe anVars_debug.exe
convert:  convert.exe convert_debug.exe
info:     info.exe

.SUFFIXES: .d #.cpp .exe .o .d
.PHONY: test doxygen 

# Optimized targets
$(OBJ_DIR)/Inputs/Netcdf.o: $(SRC_DIR)/Inputs/Netcdf.cpp $(OBJ_DIR)/Inputs/Netcdf.d $(SRC_DIR)/Inputs/Netcdf.h
	$(CC) $(OPTIM) $(CFLAGS) $(IFLAGS) -c $< -o $@
$(OBJ_DIR_OPTIM)/Inputs/Netcdf.o: $(SRC_DIR)/Inputs/Netcdf.cpp $(OBJ_DIR_OPTIM)/Inputs/Netcdf.d $(SRC_DIR)/Inputs/Netcdf.h
	$(CC) $(OPTIM) $(CFLAGS_OPTIM) $(IFLAGS) -c $< -o $@

# Testing targets
$(OBJ_DIR)/Tests/%.o: $(SRC_DIR)/Tests/%.cpp $(OBJ_DIR)/Tests/%.d
	$(CC) $(CFLAGS) $(IFLAGS) $(IGTEST) $< -c -o $@

$(OBJ_DIR_OPTIM)/Tests/%.o: $(SRC_DIR)/Tests/%.cpp $(OBJ_DIR_OPTIM)/Tests/%.d
	$(CC) $(OPTIM) $(CFLAGS_OPTIM) $(IFLAGS) $(IGTEST) $< -c -o $@

# Regular targets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR)/%.d $(SRC_DIR)/%.h
	@$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@
	@echo "\033[31mRebuilding \033[0m$@"

$(OBJ_DIR_OPTIM)/%.o: $(SRC_DIR)/%.cpp $(OBJ_DIR_OPTIM)/%.d $(SRC_DIR)/%.h
	@$(CC) $(CFLAGS_OPTIM) $(IFLAGS) -c $< -o $@
	@echo "\033[31mRebuilding \033[0m$@"

$(OBJ_DIR)/%.E : $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(IFLAGS) -E $< > $@

# Test objects don't have header files
$(OBJ_DIR)/Tests/%.d: $(SRC_DIR)/Tests/%.cpp 
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$<)' $< -MF $@
$(OBJ_DIR_OPTIM)/Tests/%.d: $(SRC_DIR)/Tests/%.cpp 
	$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.cpp,$(OBJ_DIR_OPTIM)/%.o,$<)' $< -MF $@

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	@$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.cpp,$(OBJ_DIR)/%.o,$<)' $< -MF $@
	@echo "\033[33mDependency \033[0m$@"
$(OBJ_DIR_OPTIM)/%.d: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.h
	@$(CC) $(CFLAGS) -MM -MT '$(patsubst src/%.cpp,$(OBJ_DIR_OPTIM)/%.o,$<)' $< -MF $@
	@echo "\033[33mDependency \033[0m$@"

clean: cleanO
	rm -f $(OBJ_DIR)/*.d $(OBJ_DIR)/*.o $(OBJ_DIR)/*/* *.exe $(OBJ_DIR)/*.gcda $(OBJ_DIR)/*.gcov $(OBJ_DIR)/*.gcno *~ makefile.bak core $(SRC_DIR)/tags gmon.out tests/*.exe

cleanO:
	rm -f $(OBJ_DIR_OPTIM)/*.d $(OBJ_DIR_OPTIM)/*.o $(OBJ_DIR_OPTIM)/*/* *.exe $(OBJ_DIR_OPTIM)/*.gcda $(OBJ_DIR_OPTIM)/*.gcov $(OBJ_DIR_OPTIM)/*.gcno *~ makefile.bak core $(SRC_DIR)/tags gmon.out tests/*.exe

cleanobjs:
	rm -fr $(OBJ_DIR)/*

cleantests:
	rm -fr tests/*.exe

count:
	wc -l src/*.cpp src/*/*.cpp src/*/*.h src/*.h 

tags: makefile $(SRC) $(HEADERS)
	cd src; ctags -R --c++-kinds=+pl --fields=+iaS --extra=+q -f tags ./ ./*

COMPS_OBJ =\
			 $(AVERAGERS)\
			 $(CALIBRATORS)\
			 $(CONTINUOUS)\
			 $(CONFIGURATIONS)\
			 $(CORRECTORS)\
			 $(DET_METRICS)\
			 $(DISCRETES)\
			 $(QCS)\
			 $(BASEDISTRIBUTIONS)\
			 $(DOWNSCALERS)\
			 $(ESTIMATORS)\
			 $(FINDERS)\
			 $(INPUTS)\
			 $(INTERPOLATORS)\
			 $(LOGGERS)\
			 $(OTHERS)\
			 $(METRICS)\
			 $(OUTPUTS)\
			 $(PARAMETERIOS)\
			 $(SELECTORS)\
			 $(MEASURES)\
			 $(SMOOTHERS)\
			 $(UNCERTAINTIES)\
			 $(VARIABLES)\
          $(OBJ_DIR)/Field.o\
          $(OBJ_DIR)/Location.o\
          $(OBJ_DIR)/Namelist.o\
          $(OBJ_DIR)/Options.o\
          $(OBJ_DIR)/Data.o\
          $(OBJ_DIR)/Scheme.o\
          $(OBJ_DIR)/Ensemble.o\
          $(OBJ_DIR)/Global.o\
          $(OBJ_DIR)/Value.o\
          $(OBJ_DIR)/Entity.o\
          $(OBJ_DIR)/Cache.o\
          $(OBJ_DIR)/Key.o\
          $(OBJ_DIR)/DataKey.o\
          $(OBJ_DIR)/Distribution.o\
          $(OBJ_DIR)/Member.o\
          $(OBJ_DIR)/Component.o\
          $(OBJ_DIR)/Probabilistic.o\
          $(OBJ_DIR)/Parameters.o\
          $(OBJ_DIR)/Ensembler.o\
          $(OBJ_DIR)/Obs.o
COMPS_OBJ_OPTIM = $(patsubst $(OBJ_DIR)/%,$(OBJ_DIR_OPTIM)/%,$(COMPS_OBJ))

comps_debug.exe: $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Comps.o
	$(CC) $(CFLAGS) $(LFLAGS) $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Comps.o -o $@ $(LIBS)  

comps.exe: $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Comps.o
	$(CC) $(CFLAGS_OPTIM) $(LFLAGS) $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Comps.o -o $@ $(LIBS)  

convert_debug.exe: $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Convert.o
	$(CC) $(CFLAGS) $(LFLAGS) $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Convert.o -o $@ $(LIBS)
convert.exe: $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Convert.o
	$(CC) $(CFLAGS_OPTIM) $(LFLAGS) $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Convert.o -o $@ $(LIBS)

timing.exe: $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Timing.o
	$(CC) $(CFLAGS_OPTIM) $(LFLAGS) $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Timing.o -o $@ $(LIBS)

timing_debug.exe: $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Timing.o
	$(CC) $(CFLAGS) $(LFLAGS) $(COMPS_OBJ) $(OBJ_DIR)/Drivers/Timing.o -o $@ $(LIBS)

info.exe: $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Info.o
	$(CC) $(CFLAGS_OPTIM) $(LFLAGS) $(COMPS_OBJ_OPTIM) $(OBJ_DIR_OPTIM)/Drivers/Info.o -o $@ $(LIBS)  

-include branchMakefile

# Testing targets
# $^ gives a list of all dependencies, regardless if they are newer than the target or not
#tests/%.exe: $(OBJ_DIR_OPTIM)/Tests/%.o $(COMPS_OBJ) 
#	$(CC) $(CFLAGS_OPTIM) $(LFLAGS) $(COMPS_OBJ_OPTIM) $< -o $@ $(LIBS) $(LGTEST)
tests/%.exe: $(OBJ_DIR)/Tests/%.o $(COMPS_OBJ) 
	$(CC) $(CFLAGS) $(LFLAGS) $(COMPS_OBJ) $< -o $@ $(LIBS) $(LGTEST)

# Runs all test cases
test: $(EXE_TESTS)

runTest:
	$(foreach program,$(EXE_TESTS),\
		./$(program);)

coverage.info: runTest
	lcov -d ./obj -b ./ --capture --output-file coverage.info

coverage: coverage.info
	genhtml coverage.info -o coverage/

doxygen : doxygen/config 
	cd src;doxygen ../doxygen/config

profile.png:
	gprof2png comps.exe

-include $(DEPS)
