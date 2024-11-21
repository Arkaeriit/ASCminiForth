# Flags
CFLAGS ?= -Wall -Wextra -g -Wno-error=cpp

# Files lists
C_SRC := hash.c dictionary.c forth_state.c C_func.c user_words.c parser.c utils.c sef_stack.c
FRT_SRC := base_forth_func.frt file_forth_func.frt string_forth_func.frt programming_forth_func.frt
C_HEADER := sef_config.h sef_io.h SEForth.h C_func.h dictionary.h errors.h forth_state.h hash.h parser.h user_words.h utils.h sef_debug.h private_api.h
TARGET := seforth
C_AUTO_SRC := $(FRT_SRC:%.frt=%.c)
C_SRC += $(C_AUTO_SRC)
C_OBJS := $(C_SRC:%.c=%.o)

EXEC_SCR := sef_io.c main.c
EXEC_OBJS := $(EXEC_SCR:%.c=%.o)

# Install targets
TARGET_DIR_BIN := /usr/local/bin
TARGET_BIN := $(TARGET_DIR_BIN)/$(TARGET)

# Commands
HASGCC := $(shell command -v $(CROSS_COMPILE)gcc 2> /dev/null)
ifdef HASGCC
	CC := $(CROSS_COMPILE)gcc
else
	HASCLANG := $(shell command -v $(CROSS_COMPILE)clang 2> /dev/null)
	ifdef HASCLANG
		CC := $(CROSS_COMPILE)clang
	else
		CC := $(CROSS_COMPILE)cc
	endif
endif
AR := $(CROSS_COMPILE)ar
CP := cp -f
RM := rm -rf

all : $(TARGET).bin
#all : $(C_OBJS)

%.o : %.c $(C_HEADER)
	$(CC) -c $< $(CFLAGS) -o $@

%.c : %.frt
	name=$$(echo $< | sed s:.frt*::); \
		 echo "const char* $$name = " > $@
	cat $< | sed 's:( [^)]*): :g; s:\s\+\([^"]\): \1:g; s:\\ .*::;  s:":\\":g; s:^:":; s:$$:\\n":;' | grep -v '" \?\\n"'  >> $@
	echo ';' >> $@

SEForth_template.h.o: SEForth_template.h
	gcc -o $@ -E $< $(CFLAGS)

SEForth.h: SEForth_template.h.o
	cat $< | sed 's:# .*::; s:£:#:g; s:__::g; s:>>://:' | uniq > $@

$(TARGET).bin : $(EXEC_OBJS) lib$(TARGET).a
	$(CC) $(EXEC_OBJS) -L. -l$(TARGET) $(CFLAGS) -o $@

lib$(TARGET).a : $(C_OBJS)
	$(AR) -rcs $@ $^
install :
	mkdir -p $(TARGET_DIR_BIN)
	$(CP) $(TARGET).bin $(TARGET_BIN)

uninstall :
	$(RM) $(TARGET_BIN)

clean : 
	$(RM) *.bin
	$(RM) *.bin.*
	$(RM) *.a
	$(RM) $(C_OBJS)
	$(RM) $(EXEC_OBJS)
	$(RM) $(C_AUTO_SRC)
	$(RM) test.txt
	$(RM) SEForth.h
	$(RM) *_template.h.o

test : $(TARGET).bin
	cd ./non-regression-tests && \
		./run-test.sh && \
		rm -f test.txt

