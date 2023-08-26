#include "s21_grep.h"

int main(int argc, char *argv[]) {
  char patterns[SIZE][SIZE] = {0};  // массив для хранения паттернов
  int file_location = 0;  // индекс текущего файла
  int opt_ind = 0;  // индекс следующего параметра в argv
  grep_values grep_value = {
      0};  // структура для передачи параметров в функцию grep
  flags option = {0};  // структура для хранения флагов

  opt_ind =
      parser(argc, argv, &option, &grep_value, patterns);  // парсим аргументы

  // Проверяем, что переданы нужные аргументы и не произошла ошибка при парсинге
  if (argc >= 3 && option.errorflag != 1 && opt_ind != argc) {
    file_location = poisk_patterna(opt_ind, argv,
                                   patterns);  // находим индекс первого файла
    while (file_location < argc) {
      if (argv[file_location + 1] != NULL) {
        grep_value.count_files = grep_value.count_files +
                                 1;  // увеличиваем счетчик обработанных файлов
      }
      grep_value.path =
          argv[file_location];  // устанавливаем путь к текущему файлу
      grep(grep_value, option,
           patterns);  // вызываем функцию grep для текущего файла
      file_location++;  // переходим к следующему файлу
    }
  }
}
