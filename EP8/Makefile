BUILD_DIR := build
PROJECT_NAME := EP8

C_SOURCES := $(wildcard */*.c)
C_INCLUDES := -I. -Iinc
OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

C_FLAGS := $(C_INCLUDES) -O2 -Wall -Wextra -lpthread

all: $(BUILD_DIR) $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJECTS)
	@echo "Creating executable"
	gcc -o $@ $^ $(C_FLAGS)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	gcc -c $< -o $@ $(C_FLAGS)
	@echo 

$(BUILD_DIR): 
	@echo "Creating build directory"
	@mkdir -p $@
	@echo 

run:
ifeq ($(OS),Windows_NT)
	@./$(PROJECT_NAME).exe
else
	@./$(PROJECT_NAME)
endif

clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
ifeq ($(OS),Windows_NT)
	rm $(PROJECT_NAME).exe
else
	rm $(PROJECT_NAME)
endif 


.PHONY: all clean run