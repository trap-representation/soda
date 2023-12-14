#include <stdio.h>

int main(void) {
  FILE *f = fopen("a.out.wasm", "rb");
  FILE *out = fopen("a.txt", "w");
  int c;
  fprintf(out, "source = new Uint8Array([");
  while((c = fgetc(f)) != EOF) {
    fprintf(out, "%d, ", c);
  }
  fprintf(out, "]");
  fclose(f);
  fclose(out);
}
