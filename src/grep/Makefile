FLAGS = -Wall -Werror -Wextra -std=c11 -pedantic

all: s21_grep

rebuild: clean all

s21_grep: grep_parser
	gcc $(FLAGS) s21_grep.c grep_parser.o -o s21_grep
	rm grep_parser.o

grep_parser:
	$(CC) grep_parser.c -c	

test: s21_grep
	bash test_func_grep.sh
	
clean:
	rm -rf s21_grep
 