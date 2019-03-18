# for PC test

CPP      = g++
CFLAGS  = -g3 -MMD -MP -DPC -DUSE_SDL  -Wall -Wextra -Og
LIBS    = -lm -lSDL2 -lpthread
SRC_DIR = .
OBJ_DIR = ./build
BUILD_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS +=  $(wildcard $(SRC_DIR)/*.cpp) 
OBJS    = $(SRCS:%=$(OBJ_DIR)/%.o)
TARGET  = a.out
DEPENDS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CPP) $(OBJS) $(LIBS)

$(OBJ_DIR)/%.c.o: %.c
	mkdir -p $(OBJ_DIR);
	$(CPP) $(CFLAGS) $(INCLUDE) -o $@ -c $<

$(OBJ_DIR)/%.cpp.o: %.cpp 
	mkdir -p $(OBJ_DIR);
	$(CPP) $(CFLAGS) $(INCLUDE) -o $@ -c $< 

clean:
	rm $(OBJS) $(TARGET) $(DEPENDS)

-include $(DEPENDS)

.PHONY: all clean
