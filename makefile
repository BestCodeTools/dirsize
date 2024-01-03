# Determine the operating system
ifeq ($(OS),Windows_NT)
	OS := Windows
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OS := Linux
	endif
endif

# Set the output directory based on the operating system
ifeq ($(OS),Windows)
	OUTPUT_DIR := bin/Windows/release
	DEST_DIR := C:/dev/cmd-scripts
else
	OUTPUT_DIR := bin/Linux/release
	DEST_DIR := path/to/destination/directory/Linux
endif

# Default target
all: dirsize copy

# Create the output directory if it doesn't exist
$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Compile the dirsize program
dirsize: main.cpp | $(OUTPUT_DIR)
	g++ -o $(OUTPUT_DIR)/dirsize main.cpp

# Copy the executable to the destination directory
copy: dirsize
	mkdir -p $(DEST_DIR)
	cp $(OUTPUT_DIR)/dirsize $(DEST_DIR)

# Clean the output directory
clean:
	rm -rf $(OUTPUT_DIR)/*