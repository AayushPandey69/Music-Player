# Define the compiler
CXX := g++

# Define the paths
SFML_INCLUDE_PATH := C:/Users/Lenovo/Downloads/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/include
SFML_LIB_PATH := C:/Users/Lenovo/Downloads/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.1/lib

# Define the compiler and linker flags
CXXFLAGS := -std=c++17 -I"$(SFML_INCLUDE_PATH)"
LDFLAGS := -L"$(SFML_LIB_PATH)" -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 

# Define the target executable
TARGET := music-app.exe

# Define the source files and object files
SRCS := main.cpp GUI.cpp MusicPlayer.cpp Utilities.cpp
OBJS := $(SRCS:.cpp=.o)

# The default target to build
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile the source files into object files
%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

# Clean up build artifacts
clean:
	del /Q $(OBJS) $(TARGET)

.PHONY: all clean
