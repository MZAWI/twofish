CC = gcc
CFLAGS = -Wall -Wextra
OBJ = math.o key.o twofish.o
TARGET = twofish
all: $(TARGET)

test: $(OBJ) test.o
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

math.o: math.c math.h
	$(CC) $(CFLAGS) -c $<

key.o: key.c key.h
	$(CC) $(CFLAGS) -c $<

twofish.o: twofish.c twofish.h math.h key.h
	$(CC) $(CFLAGS) -c $<

test.o: test.c twofish.h math.h key.h
	$(CC) $(CFLAGS) -c $<

main.o: main.c twofish.h math.h key.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ)
