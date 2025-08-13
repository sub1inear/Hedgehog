#include "hstdlib.h"

int main() {
  h_float x = 20.0f;
  println("%Ff", x.get_mpf_t());
  return 0;
}