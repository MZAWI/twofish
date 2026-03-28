# Twofish

Implementation (most likely insecure) of twofish encryption algorithm in C.
Twofish is a 128-bit block cipher with support for 128-, 192-, 256-bit key lengths.
This was just a fun project.

## Building

To build this project, simply use `make` command.
You can specify arguments like `clean` to remove
*.o files and `twofish` or `test` to build individual programs.
You will need compiler, make and standard headers.

## Usage

### Main

To use the example program, please use the syntax:

```bash
./twofish input_file output_file e/d
```

Where e/d stands for encryption/decryption.

### Test

Those are original (a bit altered) KATs from [Mr Schneier website](https://www.schneier.com/academic/twofish/).
I've included it for some reference.

```bash
./test
```
