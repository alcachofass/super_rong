DEBUG ?= 0
CC     = clang
OBJ    = main.o functions.o
LINKER_FLAGS = `pkg-config sdl3 --cflags --libs`
TARGET = superrong-sdl

WARNING_FLAGS = -Wall -Wextra -pedantic

ifeq ($(DEBUG), 1)
	LDFLAGS += -DDEBUG
else
	FLAGS  += -O2
endif


all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LINKER_FLAGS) 

main.o: main.c definitions.h functions.o
	$(CC) -c main.c $(FLAGS) $(LDFLAGS) $(WARNING_FLAGS)

functions.o: functions.c definitions.h
	$(CC) -c functions.c $(FLAGS) $(LDFLAGS) $(WARNING_FLAGS)

clean:
	rm -f $(OBJ) $(TARGET)

