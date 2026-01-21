# Twofish

Implementation of twofish encryption algorithm in C. Twofish is a 128-bit block cipher with support for 128-, 192-, 256-bit key lengths.

# Building

To build this project, simply use `make` command. You can specify arguments like `clean` to remove *.o files and `twofish` or `test` to build individual programs.
To build the project, you will need compiler, make and standard headers.

# Usage

To use the example program, please use the syntax:

```bash
./twofish input_file output_file e/d
```

Where e/d stands for encryption/decryption. You can also run test program to check if it passes KATs.
