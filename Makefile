CC = gcc
LD_FLAGS = -lcurl
FLAGS = -g -O3
TARGET = atm
OBJ = 	hashmap.c.o \
	  	fm.c.o \
		converter.c.o \
		strparse.c.o \
		atm.c.o \
		algorithm.c.o 

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LD_FLAGS)

hashmap.c.o: hashmap.c
	$(CC) -c hashmap.c -o hashmap.c.o $(FLAGS)

fm.c.o: fm.c
	$(CC) -c fm.c -o fm.c.o $(FLAGS)

atm.c.o: atm.c
	$(CC) -c atm.c -o atm.c.o $(FLAGS)

converter.c.o: converter.c
	$(CC) -c converter.c -o converter.c.o $(FLAGS)

strparse.c.o: strparse.c
	$(CC) -c strparse.c -o strparse.c.o $(FLAGS)

algorithm.c.o: algorithm.c
	$(CC) -c algorithm.c -o algorithm.c.o $(FLAGS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(TARGET)
