config ?= release
options ?= COLORIZED

SRC_DIR := $(shell pwd)/src/
OBJ_DIR := $(shell pwd)/.int/
BIN_DIR := $(shell pwd)/bin/
OBJS := $(patsubst $(SRC_DIR)%.c, $(OBJ_DIR)%.o, $(shell find $(SRC_DIR) -type f -regex '.*\.c$$'))

CC := gcc
AR := ar
CFLAGS := 
INCLUDES := include/

ifeq ($(config), debug)
    CFLAGS += -g2 -O0 -Wall -D DEBUG    
else ifeq ($(config), release)
    CLFAGS += -O2 -D NODEBUG
else
    $(error Unknown config.)
endif

ifeq ($(verbose), yes)
    SILENT :=  
else
    SILENT := @
endif

BIN_DIR := $(BIN_DIR)$(config)/
TARGET := $(BIN_DIR)libset.a

all: $(TARGET)

test: $(TARGET)
	$(SILENT) exec $(CC) testtest.c $(CFLAGS) $(INCLUDES:%=-I%) $(options:%=-D%) -L $(BIN_DIR) -lset -o $@
	$(SILENT) ./$@

clean:
	@echo "=== Cleaning up ==="
	$(SILENT) rm -rf $(OBJ_DIR) 
	$(SILENT) rm -rf $(BIN_DIR) 

ensure-path:
	$(SILENT) exec mkdir -p $(BIN_DIR)
	$(SILENT) exec mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c | ensure-path
	@echo "Building ($(config)): $@"
	$(SILENT) $(CC) $(CFLAGS) $(INCLUDES:%=-I%) $(options:%=-D%) -c $? -o $@

$(TARGET): $(OBJS)
	@echo "Archiving ($(config)): $(TARGET)"
	$(SILENT) $(AR) rcs $@ $?

.PHONY: all clean test
