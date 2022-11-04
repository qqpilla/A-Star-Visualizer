# DIRS
BUILD_DIR := ./build
C_SRC_DIRS := ./lib/src
CXX_SRC_DIRS := ./src
INC_DIRS := ./include ./lib/include
LIB_DIRS := ./lib/libs
BUILD_FILES_DIR := $(BUILD_DIR)/buildfiles

# .EXE
TARGET_EXE := $(BUILD_DIR)/program.exe

# LIBS
OPENGL_LIBS := glfw3 opengl32 gdi32

# Compiler & Linker flags
CC := gcc
CXX := g++
CXX_FLAGS := -std=c++1z
WARNINGS := -Wall
INC_FLAGS := $(addprefix -I ,$(addsuffix /,$(INC_DIRS)))
LIB_FLAGS := $(addprefix -L ,$(addsuffix /,$(LIB_DIRS))) $(addprefix -l,$(OPENGL_LIBS))

# Source & Object files
C_SRCS := $(wildcard $(C_SRC_DIRS)/*.c)
CXX_SRCS := $(wildcard $(CXX_SRC_DIRS)/*.cpp)
OBJS := $(C_SRCS:$(C_SRC_DIRS)/%=$(BUILD_FILES_DIR)/%.o) $(CXX_SRCS:$(CXX_SRC_DIRS)/%=$(BUILD_FILES_DIR)/%.o)
DEPENDS := $(OBJS:.o=.d)
vpath %.c $(C_SRC_DIRS)
vpath %.cpp $(CXX_SRC_DIRS)

# Linking
$(TARGET_EXE): $(OBJS) | $(BUILD_DIR)
	$(CXX) $^ -o $@ $(INC_FLAGS) $(LIB_FLAGS)

# C source
$(BUILD_FILES_DIR)/%.c.o : %.c | $(BUILD_FILES_DIR)
	$(CC) $(CFLAGS) $(WARNINGS) -MMD -MP -g -c $< -o $@ $(INC_FLAGS)

# CPP source
$(BUILD_FILES_DIR)/%.cpp.o : %.cpp | $(BUILD_FILES_DIR)
	$(CXX) $(CXX_FLAGS) $(WARNINGS) -MMD -MP -g -c $< -o $@ $(INC_FLAGS)

-include $(DEPENDS)

# Required directories
$(BUILD_FILES_DIR) : | $(BUILD_DIR)
	@mkdir "$(BUILD_FILES_DIR)"

$(BUILD_DIR) :
	@mkdir "$(BUILD_DIR)"


.PHONY: clean
clean:
	@del /q "$(BUILD_DIR)" "$(BUILD_FILES_DIR)"