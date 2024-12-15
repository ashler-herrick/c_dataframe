# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -g
LDFLAGS = -lcunit

# Directories
SRCDIR = src
TESTDIR = tests
INCDIR = include

# Source files and object files
LIB_SOURCES = $(SRCDIR)/dataframe.c $(SRCDIR)/dfio.c
LIB_OBJECTS = $(LIB_SOURCES:.c=.o)
LIB_TARGET = libdataframe.a

TEST_SOURCES = $(TESTDIR)/test_dataframe.c $(TESTDIR)/test_dfio.c
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
TEST_TARGETS = test_dataframe test_dfio

all: $(TEST_TARGETS)

$(LIB_TARGET): $(LIB_OBJECTS)
	# Tab used below
	ar rcs $@ $^

test_dataframe: $(TESTDIR)/test_dataframe.o $(LIB_TARGET)
	# Tab used below
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_dfio: $(TESTDIR)/test_dfio.o $(LIB_TARGET)
	# Tab used below
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_TARGETS)
	# Tab used below
	./test_dataframe
	./test_dfio

clean:
	# Tab used below
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(LIB_TARGET) $(TEST_TARGETS)
