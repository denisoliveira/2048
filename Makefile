CC = clang
CFLAGS = -pthread -Wall -Werror -Wextra -std=c99 -O3

SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = 2048
TEST_SOURCES = test_main.c test_game.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_EXECUTABLE = test_2048

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(EXECUTABLE)
	 ./$(EXECUTABLE)

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) $(TEST_OBJECTS) $(OBJECTS) -o $@

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(TEST_EXECUTABLE) $(TEST_OBJECTS)
