PROJECT := CALC3
SHELL = bash


# == Directories ==

SRC=lexyacc-code/
OBJ=obj/
BIN=bin/

SRC_LIB=src/
BIN_LIB=lib/

COMMON_SRC = $(OBJ)y.tab.o $(OBJ)lex.yy.o

TARGETS := $(BIN)calc3a $(BIN)calc3b $(BIN)calc3i $(BIN)calc3.c $(BIN)calc3g $(BIN_LIB)libmath.a $(BIN)libmath-implementation-test

# Compiler stuff
CC ?= gcc
FLEX ?= flex
BISON ?= bison
AR ?= ar
CFLAGS := -I$(SRC) -I$(OBJ) -m64
CFLAGS_LIB := -nostdlib -m64 -std=gnu99
LFLAGS :=
ARFLAGS := rcs

# == Fun color output ==
OFF = \033[1;0m
BOLD =\033[1;1m
BLUE = $(BOLD)\033[1;34m
GREEN = $(BOLD)\033[1;32m
RED = $(BOLD)\033[1;31m
YELLOW = $(BOLD)\033[1;33m

BEG =	echo -e -n "$(1)$(2)$(OFF) \033[1m$(3)...$(OFF)" ; echo -n > /tmp/.`whoami`-build-errors
END =	if [[ -s /tmp/.`whoami`-build-errors ]] ; then \
		if cut -d':' -f4 /tmp/.`whoami`-build-errors | grep -q error; then \
			echo -e -n "\r$(RED)$(2)$(OFF) \033[1m$(3)   $(OFF)\n"; \
			cat /tmp/.`whoami`-build-errors; \
			rm /tmp/.`whoami`-build-errors || true; \
			exit -1; \
		else \
			echo -e -n "\r$(YELLOW)$(2)$(OFF) \033[1m$(3)   $(OFF)\n"; \
			cat /tmp/.`whoami`-build-errors; \
			rm /tmp/.`whoami`-build-errors || true; \
		fi \
	else \
		echo -e -n "\r$(1)$(2)$(OFF) \033[1m$(3)$(OFF)\033[K\n"; \
	fi

INFO = echo -e -n "$(GREEN)$(1) $(2)$(OFF)\n"

ERRORS = 2>>/tmp/.`whoami`-build-errors || true
ERRORSS = >>/tmp/.`whoami`-build-errors || true

# == Build rules ==

.PHONY: all clean init test test-lib

# Prevents Make from removing intermediary files on failure
.SECONDARY:
# Disable built-in rules
.SUFFIXES:

all: init $(TARGETS)
	@$(call INFO,"::","Building $(PROJECT)... Done!");

init:
	@$(call INFO,"::","Building $(PROJECT)...");

testprogram=looptest
test: $(foreach testProgram,$(shell find tests -iname "*.calc"), $(testProgram:.calc=))


tests/%: all test-lib tests/%.calc
	@$(call INFO,"::","Building $(patsubst tests/%,%.calc,$@)...");
	@$(call BEG,$(BLUE),"  -\>","Running x86-64-driver.sh...");
	@./check.sh a . x86-64_$(patsubst tests/%,%,$@) $(patsubst tests/%,%,$@) $(ERRORS);
	@$(call END,$(BLUE),"  -\>","Running x86-64-driver.sh...");
	@$(call BEG,$(BLUE),"  ---\>","Running generated executable...");
	@./$(patsubst tests/%,%,$@) > /tmp/.$(patsubst tests/%,%,$@)-a && rm $(patsubst tests/%,%,$@) $(patsubst tests/%,%,$@).S $(ERRORS);
	@$(call END,$(BLUE),"  ---\>","Running generated executable...");
	@$(call BEG,$(BLUE),"  -\>","Running c-driver.sh...");
	@./check.sh c . c_$(patsubst tests/%,%,$@) $(patsubst tests/%,%,$@) $(ERRORS);
	@$(call END,$(BLUE),"  -\>","Running c-driver.sh...");
	@$(call BEG,$(BLUE),"  ---\>","Running generated executable...");
	@./$(patsubst tests/%,%,$@) > /tmp/.$(patsubst tests/%,%,$@)-c && rm $(patsubst tests/%,%,$@) $(patsubst tests/%,%,$@).c $(ERRORS);
	@$(call END,$(BLUE),"  ---\>","Running generated executable...");
	@$(call BEG,$(BLUE),"  -\>","Checking differance...");
	@diff /tmp/.$(patsubst tests/%,%,$@)-{a,c} $(ERRORSS);
	@$(call END,$(BLUE),"  -\>","Checking differance...");
	@$(call BEG,$(BLUE),"  -\>","Cleanup...");
	@$(RM) /tmp/.$(patsubst tests/%,%,$@)-{a,c}
	@$(call END,$(BLUE),"  -\>","Cleanup...");

test-lib: $(BIN)libmath-implementation-test
	@$(call INFO,"::","Testing the libmath.a implementation...");
	@$(call BEG,$(BLUE),"  -\> libmath-implementation-test","")
	@$(BIN)/libmath-implementation-test $(ERRORSS)
	@$(call END,$(BLUE),"  -\> libmath-implementation-test","")

$(BIN)libmath-implementation-test: $(SRC_LIB)implementation-test.c $(BIN_LIB)libmath.a
	@$(call BEG,$(BLUE),"  -\> CC","$@ \<-- $<")
	@$(CC) $^ -lm -o $@ $(ERRORS)
	@$(call END,$(BLUE),"  -\> CC","$@ \<-- $<")

$(OBJ)y.tab.c $(OBJ)y.tab.h: $(SRC)calc3.y
	@$(call BEG,$(BLUE),"  -\> BISON","$@ \<-- $<")
	@mkdir -p $(dir $@)
	@$(BISON) -y -d -o $@ $^ $(ERRORS)
	@$(call END,$(BLUE),"  -\> BISON","$@ \<-- $<")

$(OBJ)lex.yy.c: $(SRC)calc3.l
	@$(call BEG,$(BLUE),"  -\> FLEX","$@ \<-- $<")
	@mkdir -p $(dir $@)
	@$(FLEX) -o $@ $^ $(ERRORS)
	@$(call END,$(BLUE),"  -\> FLEX","$@ \<-- $<")

$(OBJ)%.o: $(SRC)%.c
	@$(call BEG,$(BLUE),"  -\> CC","$@ \<-- $<")
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(CFLAGS) $(ERRORS)
	@$(call END,$(BLUE),"  -\> CC","$@ \<-- $<")

$(OBJ)%.o: $(OBJ)%.c
	@$(call BEG,$(BLUE),"  -\> CC","$@ \<-- $<")
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(CFLAGS) $(ERRORS)
	@$(call END,$(BLUE),"  -\> CC","$@ \<-- $<")

$(OBJ)lib/%.o: $(SRC_LIB)%.S
	@$(call BEG,$(BLUE),"  -\> AS","$@ \<-- $<")
	@mkdir -p $(dir $@)
	@$(CC) -c $< -o $@ $(CFLAGS_LIB) $(ERRORS)
	@$(call END,$(BLUE),"  -\> AS","$@ \<-- $<")

$(BIN)calc3%: $(COMMON_SRC) $(OBJ)calc3%.o
	@$(call BEG,$(BLUE),"  -\> LD","$@ \<-- $^")
	@mkdir -p $(dir $@)
	@$(CC) $^ -o $@ $(LFLAGS) $(ERRORS)
	@$(call END,$(BLUE),"  -\> LD","$@ \<-- $^")

$(BIN_LIB)libmath.a: $(OBJ)lib/fact.o $(OBJ)lib/gcd.o $(OBJ)lib/lntwo.o
	@$(call BEG,$(BLUE),"  -\> AR","$@ \<-- $^")
	@mkdir -p $(dir $@)
	@$(AR) $(ARFLAGS) $@ $^ $(ERRORS)
	@$(call END,$(BLUE),"  -\> AR","$@ \<-- $^")

clean:
	@$(call INFO,"::","Removing generated files...");
	@$(call BEG,$(BLUE),"  -\> RM","$(TARGETS)")
	@$(RM) -rf $(TARGETS)
	@$(call END,$(BLUE),"  -\> RM","$(TARGETS)")
	@$(call BEG,$(BLUE),"  -\> RM","$(OBJ)")
	@$(RM) -rf $(OBJ)
	@$(call END,$(BLUE),"  -\> RM","$(OBJ)")
