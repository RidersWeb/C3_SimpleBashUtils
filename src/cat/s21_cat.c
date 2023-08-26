#include "s21_cat.h"

int main(int argc, char *argv[]) {
  int number_of_files;
  flags option = {0};
  number_of_files = parser(argc, argv, &option);

  // Проверка на наличие ошибочных флагов
  if (option.errorflag == 1) {
    fprintf(stderr, "недопустимый флаг");
  } else {
    // Проход по всем файлам, указанным в аргументах
    while (number_of_files < argc) {
      char *path;
      path = argv[number_of_files];
      // Вывод содержимого файла с помощью функции cat
      cat(path, option);
      number_of_files++;
    }
  }
  return 0;
}
