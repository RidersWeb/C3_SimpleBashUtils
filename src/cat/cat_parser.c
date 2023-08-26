#include "s21_cat.h"

int parser(int argc, char *argv[], flags *flag) {
  // массив для опций, которые могут быть длинными
  const struct option long_options[] = {
      {"number-nonblank", no_argument, NULL, 'b'},
      {"number", no_argument, NULL, 'n'},
      {"squeeze-blank", no_argument, NULL, 's'},
      {NULL, 0, NULL, 0}};

  int opt = 0;

  // обработка всех опций, переданных в командной строке
  while ((opt = getopt_long(argc, argv, "+beEnstTv?", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'b':
        flag->bflag =
            1;  // включаем опцию -b для нумерации только непустых строк
        break;
      case 'e':
        flag->eflag = flag->vflag =
            1;  // включаем опции -e и -v для распознавания регулярных выражений
        break;
      case 'E':
        flag->eflag = 1;  // включаем опцию -E для распознавания регулярных
                          // выражений без опции -v
        break;
      case 'n':
        flag->nflag = 1;  // включаем опцию -n для нумерации всех строк
        break;
      case 's':
        flag->sflag = 1;  // включаем опцию -s для удаления пустых строк
        break;
      case 't':
        flag->tflag = flag->vflag =
            1;  // включаем опции -t и -v для распознавания символов табуляции
        break;
      case 'T':
        flag->tflag =
            1;  // включаем опцию -T для вывода символов табуляции в виде ^I
        break;
      case 'v':
        flag->vflag =
            1;  // включаем опцию -v для инвертирования результата поиска
        break;
      case '?':
        flag->errorflag =
            1;  // устанавливаем флаг ошибки, если неизвестная опция
        break;
    }
  }
  return optind;  // возвращаем индекс первого аргумента не-опции
}

// Функция cat принимает путь к файлу и флаги
void cat(char *path, flags flag) {
  // Проверяем, существует ли файл
  if (access(path, F_OK) == 0) {
    // Открываем файл
    FILE *filename;
    filename = fopen(path, "r");

    // Объявляем переменные для хранения текущего и предыдущего символов,
    // текущего номера строки и флага сжатия
    char ch, prev;
    int current_line = 1;
    int squeeze = 0;

    // Читаем символы из файла, пока не достигнем конца файла
    for (prev = '\n'; (ch = getc(filename)) != EOF; prev = ch) {
      // Если флаг сжатия включен, пропускаем все повторяющиеся пустые строки
      if (flag.sflag == 1) {
        if (ch == '\n' && prev == '\n') {
          if (squeeze == 1) {
            continue;
          }
          squeeze++;
        } else
          squeeze = 0;
      }

      // Если флаг b включен, выводим номера строк, пропуская пустые строки
      if (flag.bflag == 1) {
        if (prev == '\n' && ch != '\n') {
          printf("%*d\t", 6, current_line);
          current_line++;
        }
      }

      // Если флаг n включен и флаг b выключен, выводим номера строк для всех
      // строк
      if (flag.nflag == 1 && flag.bflag == 0 && prev == '\n') {
        printf("%*d\t", 6, current_line);
        current_line++;
      }

      // Если флаг e включен, заменяем символ переноса строки на знак доллара
      if (flag.eflag == 1 && ch == '\n') {
        putchar('$');
      }

      // Если флаг t включен, заменяем символ табуляции на знак "^I"
      if (flag.tflag == 1 && ch == '\t') {
        printf("^");
        ch = 'I';
      }

      // Если флаг v включен, заменяем управляющие символы на соответствующие
      // символы
      if (flag.vflag == 1) {
        if ((ch >= 0 && ch <= 31) && ch != '\t' && ch != '\n') {
          printf("^");
          ch = ch + 64;

        } else if (ch == 127) {
          printf("^");
          ch = ch - 64;
        }
      }
      putchar(ch);
    }
    fclose(filename);
  } else {
    // Если файл не существует, выводим сообщение об ошибке
    fprintf(stderr, "No such file or directory: %s\n", path);
  }
}