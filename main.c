#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "twofish.h"

uint8_t *read_file(const char *filename, long *size) {
  FILE* f = fopen(filename, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  *size = ftell(f);
  fseek(f, 0, SEEK_SET);

  uint8_t *buffer = malloc(*size + 16);
  if (buffer) {
    fread(buffer, 1, *size, f);
  }
  fclose(f);
  return buffer;
}

void write_file(const char *filename, uint8_t *data, int len) {
  FILE *f = fopen(filename, "wb");
  if (!f) {
    fprintf(stderr, "Could not open output file\n");
    return;
  }
  fwrite(data, 1, len, f);
  fclose(f);
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("Usage: %s input_file output_file e/d", argv[0]);
    return 1;
  }

  char *infile = argv[1];
  char *outfile = argv[2];
  char mode = argv[3][0];

  Twofish_ctx ctx;
  uint8_t key[16] = "1234567890123456";
  uint8_t iv[16] = "9876543210987654";

  // key setup
  twofish_init(&ctx, key, 128);
  memcpy(ctx.iv, iv, 16);

  // load file
  long file_size = 0;
  uint8_t *buffer = read_file(infile, &file_size);
  if (!buffer) {
    fprintf(stderr, "Error reading input file");
    return 1;
  }

  int final_len = 0;

  switch (mode) {
  case 'e':
    printf("MODE: Encryption\n");
    twofish_encrypt_cbc(&ctx, buffer, (int) file_size, &final_len);
    write_file(outfile, buffer, final_len);
    printf("Encrypted %d bytes\n", final_len);
    break;
  case 'd':
    printf("MODE: Decryption\n");
    twofish_decrypt_cbc(&ctx, buffer, (int) file_size, &final_len);
    write_file(outfile, buffer, final_len);
    printf("Decrypted %d bytes", final_len);
    break;
  }
  
  free(buffer);
  return 0;
}
