CC=g++
CFLAGS=-O3 -Wall -Wno-unknown-pragmas -I/opt/systemc/include -std=c++11
LDFLAGS=-L/opt/systemc/lib -lsystemc
EXEC=main
SRC=./src/main.cpp	\
		./src/UART_COM.cpp	\
		./src/ETHERNET_COM.cpp


OBJ= $(SRC:.cpp=.o)

all: $(EXEC)
	./bin/main

main: $(OBJ)
	$(CC) $(CFLAGS) -o ./bin/$@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean: $(OBJ)
	find ./src/ -name "*.o"  -exec rm {} \;
	find ./bin/ -name "main" -exec rm {} \;
	find  -name "*.bmp"  -exec rm {} \;


mrproper: clean
	rm $(EXEC)
