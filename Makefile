# @ Unterdrückt Ausgaben des Makefile in der Shell

SRC = $(wildcard ./src/*.c)
TARGET = main

all: run

build:
	@# Baut gcc http.c main.c router.c server.c -o main
	@gcc $(SRC) -o $(TARGET)

run: build
	@./$(TARGET)

clean:
	@rm -f $(TARGET).exe
	@rm -f $(TARGET)