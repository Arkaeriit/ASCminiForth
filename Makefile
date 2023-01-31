# Flags
CFLAGS ?= -Werror -Wall -Wextra -g

# Files lists
C_SRC := hash.c dictionary.c forth_state.c C_func.c main.c user_words.c parser.c amf_io.c utils.c
FRT_SRC := forth_func.frt
C_HEADER := amf_config.h amf_io.h ASCminiForth.h C_func.h dictionary.h errors.h forth_state.h hash.h parser.h user_words.h utils.h amf_debug.h
TARGET := amForth
C_AUTO_SRC := $(FRT_SRC:%.frt=%.c)
C_SRC += $(C_AUTO_SRC)
C_OBJS := $(C_SRC:%.c=%.o)

# Install targets
TARGET_DIR_BIN := /usr/local/bin
TARGET_BIN := $(TARGET_DIR_BIN)/$(TARGET)

# Commands
CC := gcc
CP := cp -f
RM := rm -rf

all : $(TARGET).bin
#all : $(C_OBJS)

%.o : %.c $(C_HEADER)
	$(CC) -c $< $(CFLAGS) -o $@

%.c : %.frt
	name=$$(echo $< | sed s:.frt*::); \
		 echo "const char* $$name = " > $@
	cat $< | sed 's:\\ .*::;  s:":\\":g; s:^:":; s:$$:\\n":'  >> $@
	echo ';' >> $@

$(TARGET).bin : $(C_OBJS)
	$(CC) $(C_OBJS) $(CFLAGS) -o $@

install :
	mkdir -p $(TARGET_DIR_BIN)
	$(CP) $(TARGET).bin $(TARGET_BIN)

uninstall :
	$(RM) $(TARGET_BIN)

clean : 
	$(RM) *.bin
	$(RM) $(C_OBJS)
	$(RM) $(C_AUTO_SRC)

test : $(TARGET).bin
	./$(TARGET).bin benchmark.frt

