# Flags
FLAGS := -Wall -g

# Files lists
C_SRC := hash.c dictionary.c forth_state.c
C_OBJS := $(C_SRC:%.c=%.o)
TARGET := amForth

# Install targets
TARGET_DIR_BIN := /usr/local/bin
TARGET_BIN := $(TARGET_DIR_BIN)/$(TARGET)

# Commands
CC := gcc
CP := cp -f
RM := rm -rf

#all : $(TARGET).bin
all : $(C_OBJS)

%.o : %.c
	$(CC) -c $< $(FLAGS) -o $@

$(TARGET).bin : $(C_OBJS)
	$(CC) $(C_OBJS) $(FLAGS) -o $@

install :
	mkdir -p $(TARGET_DIR_BIN)
	$(CP) $(TARGET).bin $(TARGET_BIN)

uninstall :
	$(RM) $(TARGET_BIN)

clean : 
	$(RM) *.bin
	$(RM) *.o

