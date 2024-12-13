CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = -lcunit

SRCDIR = src
TESTDIR = tests
INCDIR = include

SOURCES = $(SRCDIR)/dataframe.c
OBJECTS = $(SOURCES:.c=.o)

TEST_SOURCES = $(TESTDIR)/test_dataframe.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)

TARGET = dataframe
TEST_TARGET = run_tests

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(TARGET) $(TEST_TARGET)
