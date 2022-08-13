#======================================================================================================================#
# VARIABLES
#======================================================================================================================#

EXECUTABLE     := k_lang
INCLUDE_FOLDER := 
LIB_FOLDER     := 
BUILD_FOLDER   := build
SRC_FOLDER     := src
LIBS           := 
FLAGS          := -Wall -Wno-missing-braces
DEFS           := 

#======================================================================================================================#
# BUILD PROCESS
#======================================================================================================================#

CC            := gcc $(FLAGS) $(DEFS) $(INCLUDE_FOLDER) $(LIB_FOLDER)
MV            := mv
RM            := rm -rf
CP            := cp
MKDIR         := mkdir -p
SRC_FOLDERS   := $(shell find $(SRC_FOLDER)/ -type d)
BUILD_FOLDERS := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(SRC_FOLDERS))
C_FILES       := $(shell find $(SRC_FOLDER)/ -type f -name "*.c")
O_FILES       := $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C_FILES)))

define \n


endef

.PHONY: default run clean

default: clean $(EXECUTABLE)

run: clean $(EXECUTABLE)
	./$(EXECUTABLE)

debug: CC += -g
debug: clean $(EXECUTABLE)

$(BUILD_FOLDER):
	$(MKDIR) $(BUILD_FOLDERS)

$(EXECUTABLE): $(BUILD_FOLDER)
	$(foreach C, $(C_FILES), $(CC) -c $(C) -o $(subst $(SRC_FOLDER)/,$(BUILD_FOLDER)/,$(subst .c,.o,$(C))) $(\n))
	$(CC) $(O_FILES) -o $(EXECUTABLE) $(LIBS)

clean:
	$(RM) $(EXECUTABLE) $(BUILD_FOLDER)
