CXX = g++
TARGET = main
GLAD_ROOT_DIR = ../../Common/glad
SRCS = main.cpp $(GLAD_ROOT_DIR)/src/glad.c
CXXFLAGS = -I$(GLAD_ROOT_DIR)/include
LDFLAGS = -lSDL2 -ldl -lGL
all: $(TARGET)
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
clean:
	rm -f $(TARGET)
.PHONY: all clean
