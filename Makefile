##
## EPITECH PROJECT, 2023
## Gomoku
## File description:
## makefile
##

TARGET = pbrain-hikarunogomoku

all: $(TARGET)

.PHONY: $(TARGET)
$(TARGET):
	cmake -S . -B build
	cmake --build build

clean:
	cmake --build build --target clean

fclean: clean
	echo "Remove the folder 'build' yourself"

re: fclean all

debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
