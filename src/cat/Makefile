CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11

all: s21_cat

rebuild: clean all

s21_cat: cat_parser
	gcc $(FLAGS) s21_cat.c cat_parser.c -o s21_cat
	rm cat_parser.o

cat_parser:
	$(CC) cat_parser.c -c	

test: s21_cat
	bash cat_test.sh
clean:
	rm -rf s21_cat