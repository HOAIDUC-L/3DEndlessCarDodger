# ============================================================================
# MAKEFILE - 3D ENDLESS CAR DODGER (MODULAR ARCHITECTURE)
# ============================================================================

CXX = g++
CXXFLAGS = -O2 -Wall
LIBS = -lGL -lGLU -lglut

TARGET = game

SRCS = main.cpp algo_2d.cpp algo_3d_geometry.cpp algo_shading_culling.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
