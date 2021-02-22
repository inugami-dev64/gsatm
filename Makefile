CC = gcc
LD_FLAGS = -lcurl
FLAGS = -g
TARGET = atm
OBJ = 	hashmap.c.o \
	  	parse_csv.c.o \
		atm.c.o \
		converter.c.o 

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LD_FLAGS)

hashmap.c.o: hashmap.c
	$(CC) -c hashmap.c -o hashmap.c.o $(FLAGS)

parse_csv.c.o: parse_csv.c
	$(CC) -c parse_csv.c -o parse_csv.c.o $(FLAGS)

atm.c.o: atm.c
	$(CC) -c atm.c -o atm.c.o $(FLAGS)

converter.c.o: converter.c
	$(CC) -c converter.c -o converter.c.o $(FLAGS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -rf $(TARGET)
