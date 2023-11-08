##
## EPITECH PROJECT, 2023
## Gomoku
## File description:
## makefile
##

TARGET = pbrain-hikarunogomoku

all: $(TARGET)

$(TARGET):
	cmake -S . -B build
	cmake --build build

clean:
	cmake --build build --target clean

fclean: clean
	echo "Remove the folder 'build' yourself"

re: fclean all
