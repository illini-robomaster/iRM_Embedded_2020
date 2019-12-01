#include "bsp_error_handler.h"

void bsp_error_handler(const char* func, int line, char* msg) {
  printf("[ERROR at ");
  printf("%s:", func);
  printf("%d] ", line);
  printf("%s\r\n", msg);
  return;
}
