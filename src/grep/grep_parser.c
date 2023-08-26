#include "s21_grep.h"

int poisk_patterna(int opt_ind, char *argv[], char patterns[SIZE][SIZE]) {
  int file_location = 0;

  // Если массив шаблонов пустой, то заполняем его первым шаблоном и
  // начинаем поиск файлов сразу за ним
  if (*patterns[0] == 0) {
    file_location = opt_ind + 1;
    strcpy(patterns[0], argv[opt_ind]);
  } else {
    // Если массив шаблонов уже заполнен, то начинаем поиск файлов сразу
    // после индекса опции
    file_location = opt_ind;
  }

  // Возвращаем индекс, с которого начинается поиск файлов
  return file_location;
}
int parser(int argc, char *argv[], flags *flag, grep_values *grep_value,
           char patterns[SIZE][SIZE]) {
  int opt = 0;
  opterr = 0;  // Отключаем вывод ошибок в стандартный поток

  // Обрабатываем аргументы командной строки с помощью getopt_long()
  while ((opt = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1) {
    switch (opt) {
      case 'e':  // Обработка аргумента "-e"
        // Ищем свободное место в массиве patterns
        while (*patterns[grep_value->count_pattern] != 0) {
          grep_value->count_pattern = grep_value->count_pattern + 1;
        }
        // Копируем строку optarg в массив patterns
        strcpy(patterns[grep_value->count_pattern], optarg);
        // Увеличиваем значение count_pattern и устанавливаем флаг eflag
        grep_value->count_pattern = grep_value->count_pattern + 1;
        flag->eflag = 1;
        break;
      case 'i':  // флаг i Игнорирует различия регистра.
        flag->iflag = 1;
        break;
      case 'v':  // Инвертирует смысл поиска соответствий.
        flag->vflag = 1;
        break;
      case 'c':  // Выводит только количество совпадающих строк.
        flag->cflag = 1;
        break;
      case 'l':  // Выводит только совпадающие файлы.
        flag->lflag = 1;
        break;
      case 'n':  // Предваряет каждую строку вывода номером строки из файла
                 // ввода.
        flag->nflag = 1;
        break;
      case 'h':  // Выводит совпадающие строки, не предваряя их именами файлов.
        flag->hflag = 1;
        break;
      case 's':  // Подавляет сообщения об ошибках о несуществующих или
                 // нечитаемых файлах.
        flag->sflag = 1;
        break;
      case 'f':
        // Ищем свободное место в массиве patterns
        while (*patterns[grep_value->count_pattern] != 0) {
          grep_value->count_pattern = grep_value->count_pattern + 1;
        }
        // Считываем паттерны из файла и сохраняем в массиве patterns
        f_flag(optarg, patterns, grep_value);
        flag->fflag = 1;
        break;
      case 'o':
        flag->oflag = 1;
        break;
      case '?':
        flag->errorflag = 1;
        break;
    }
  }
  return optind;  // Возвращаем индекс первого необработанного аргумента
}

void grep(grep_values grep_value, flags flag, char pattern[SIZE][SIZE]) {
  FILE *filename;
  char string[SIZE] = " ";
  int count_lines = 0;  // количество обработанных строк
  int count_matched_lines =
      0;  // количество строк, в которых было найдено совпадение с шаблоном

  int comp_flag1 =
      REG_EXTENDED;  // флаг для сравнения строк, по умолчанию используется
                     // расширенное регулярное выражение

  int stop = 0;  // флаг для остановки обработки, если задан флаг "-l"

  int result_regex = 0;  // переменная для хранения результата сравнения строк,

  regex_t
      preg;  // переменная для хранения скомпилированного регулярного выражения

  // проверка наличия флагов, запрещающих вывод обычного результата
  if (flag.vflag == 1 || flag.cflag == 1 || flag.lflag == 1) flag.oflag = 0;

  // проверка наличия файла
  if (access(grep_value.path, F_OK) == 0) {
    filename = fopen(grep_value.path, "r");

    // если не заданы флаги "-e" или "-f", ищем один шаблон по умолчанию
    if (grep_value.count_pattern == 0 && flag.eflag == 0 && flag.fflag == 0)
      grep_value.count_pattern = 1;
    // если задан флаг "-i", используем регистронезависимое сравнение
    if (flag.iflag == 1) comp_flag1 = REG_ICASE;

    // построчное чтение файла
    while ((fgets(string, SIZE, filename)) && (stop == 0)) {
      int findline = 0;  // флаг для нахождения первого совпадения в строке
      count_lines++;  // увеличиваем количество обработанных строк на 1
      int pattern_no_match =
          0;  // количество шаблонов, которые не совпали со строкой

      // если в конце строки не найден символ переноса строки, добавляем его
      if (strchr(string, '\n') == NULL) strcat(string, "\n");

      // перебираем все заданные шаблоны и ищем их в строке
      for (int i = 0; i < grep_value.count_pattern; i++) {
        int match = 0;  // флаг для обозначения нахождения совпадения

        // компилируем регулярное выражение
        regcomp(&preg, pattern[i], comp_flag1 | REG_NEWLINE);
        // сравниваем строку и шаблон
        result_regex = regexec(&preg, string, 0, NULL, 0);

        // если шаблон найден и не установлен флаг -v, устанавливаем флаг
        // совпадения
        if (result_regex == 0 && flag.vflag == 0) match = 1;

        // если шаблон не найден и установлен флаг -v, увеличиваем счетчик
        // строк, в которых нет совпадений
        if (result_regex == REG_NOMATCH && flag.vflag == 1) {
          pattern_no_match++;
          // если количество строк без совпадений равно общему количеству
          // шаблонов, устанавливаем флаг совпадения
          if (pattern_no_match == grep_value.count_pattern) match = 1;
        }

        // если установлен флаг -l и есть совпадение, выводим путь к файлу и
        // устанавливаем флаг остановки
        if (flag.lflag == 1 && match == 1 && flag.cflag == 0) {
          printf("%s\n", grep_value.path);
          match = 0;
          stop = 1;
        }

        // если задан флаг "-c", увеличиваем счетчик строк, в которых найдены
        // совпадения
        if (flag.cflag == 1 && match == 1) {
          count_matched_lines++;
          match = 0;
        }

        // если найдено первое совпадение, выводим эту строку и устанавливаем
        // соответствующий флаг
        if (findline == 0 && match == 1) {
          printing_strings(grep_value, flag, count_lines, string);
          findline++;
        }

        // если задан флаг "-o", выводим только совпадающие подстроки и их
        // позиции
        if (flag.oflag == 1) {
          flag_o(string, preg);
        }

        // освобождаем память, выделенную под regex
        regfree(&preg);
      }
    }

    // если задан флаг "-c", выводим количество строк, в которых найдены
    // совпадения
    if (flag.cflag == 1) {
      flag_c_l(grep_value, flag, count_matched_lines);
    }

    fclose(filename);
  } else if (flag.sflag == 0) {
    // если не задан флаг "-s", выводим сообщение об ошибке
    fprintf(stderr, "No such file or directory: %s\n", grep_value.path);
  }
}

// Функция для обработки флага "-f" (чтение списка шаблонов из файла)
void f_flag(char *path, char pattern[SIZE][SIZE], grep_values *grep_value) {
  FILE *filename;
  int length = 0;
  // Проверяем, существует ли файл по указанному пути
  if (access(path, F_OK) == 0) {
    // Открываем файл на чтение
    filename = fopen(path, "r");
    // Считываем шаблоны из файла, пока не достигнем конца файла
    while (!feof(filename)) {
      fgets(pattern[grep_value->count_pattern], SIZE, filename);
      length = strlen(pattern[grep_value->count_pattern]);

      // Проверяем, чтобы первый символ шаблона не был символом переноса строки,
      // а последний символ был символом переноса строки
      if (pattern[grep_value->count_pattern][0] != '\n' &&
          pattern[grep_value->count_pattern][length - 1] == '\n') {
        pattern[grep_value->count_pattern][length - 1] = '\0';
      }
      // Увеличиваем счетчик количества считанных шаблонов
      grep_value->count_pattern = grep_value->count_pattern + 1;
    }
    // Закрываем файл
    fclose(filename);
  } else {
    printf(
        "error file");  // Если файл не существует, выводим сообщение об ошибке
  }
}

// Функция для обработки флагов "-c" и "-l"
void flag_c_l(grep_values grep_value, flags flag, int count_matched_lines) {
  if (grep_value.count_files >= 1 && flag.hflag == 0) {
    printf("%s:",
           grep_value.path);  // Если обрабатываем более одного файла и не
                              // утстановлен флаг h, который скрывает имя файла,
                              // выводим имя файла перед количеством совпадений
  }
  if (flag.lflag == 0) {
    printf("%d\n", count_matched_lines);  // Выводим количество совпадений
  } else if (flag.lflag == 1) {
    if (count_matched_lines > 0) {
      printf("1\n");
      printf("%s\n", grep_value.path);  // Если есть хотя бы одно совпадение,
                                        // выводим "1" и имя файла
    } else {
      printf("0\n");  // Если совпадений нет, выводим "0"
    }
  }
}

void printing_strings(grep_values grep_value, flags flag, int count_lines,
                      char *string) {
  // если задан флаг "-h" и количество файлов >= 1, не выводим название файла
  if (grep_value.count_files >= 1 && flag.hflag == 0)
    printf("%s:", grep_value.path);
  // если задан флаг "-n", выводим номер строки
  if (flag.nflag == 1) printf("%d:", count_lines);
  // выводим содержимое строки
  if (flag.oflag == 0) printf("%s", string);
}

void flag_o(char *string, regex_t preg) {
  regmatch_t pmatch[1];
  // продолжаем поиск подстрок, пока не закончатся все вхождения регулярного
  // выражения в строку
  while (regexec(&preg, string, 1, pmatch, 0) == 0) {
    // выводим символы из найденной подстроки, используя индексы из pmatch
    for (int j = 0; j < pmatch->rm_eo; j++) {
      if (j >= pmatch->rm_so) {
        printf("%c", string[j]);
      }
      // заменяем символы из найденной подстроки на символы с кодом 127
      string[j] = 127;
    }
    printf("\n");
  }
}