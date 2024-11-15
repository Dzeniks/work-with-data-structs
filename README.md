# Figure Search Program

A C program that analyzes bitmap images to find geometric patterns such as horizontal lines, vertical lines, and squares.

## Overview

This program processes bitmap images stored in text format and can identify:
- Longest horizontal lines
- Longest vertical lines
- Largest squares

## Building

```bash
gcc -o figsearch figsearch.c
```

## Usage

```bash
./figsearch [method] [filename]
```

### Methods:
- `hline`: Find horizontal lines in the input image
- `vline`: Find vertical lines in the input image
- `square`: Find squares in the input image
- `test`: Validate input file format

### Arguments:
- `method`: Processing method (hline, vline, square, or test)
- `filename`: Path to input bitmap file in txt format

### Options:
- `--help`: Display help message

### Examples:
```bash
./figsearch hline img.txt    # Find horizontal lines in img.txt
./figsearch vline img.txt    # Find vertical lines in img.txt
./figsearch square img.txt   # Find squares in img.txt
./figsearch test img.txt     # Validate img.txt format
```

## Input File Format

The input file should be a text file containing a binary bitmap representation:
- First line: two integers specifying dimensions (rows columns)
- Following lines: space-separated 0s and 1s representing the bitmap

Example:
```
4 5
1 1 1 0 0
0 0 1 0 0
0 0 1 1 1
0 0 0 0 0
```

## Output

The program outputs coordinates in the format:
```
start_x start_y end_x end_y
```

For example:
```bash
$ ./figsearch hline example.txt
2 2 2 4
```

## Error Handling

The program handles various error conditions:
- Invalid input file format
- File not found
- Invalid method specification
- Memory allocation failures

Error messages are printed to stderr.

## Program Structure

### Key Components:

- `bitmap`: Structure for storing the binary image data
- `vector`: Structure for storing 2D coordinates
- `result`: Structure for storing search results
- `config`: Structure for program configuration

### Main Functions:

- `figsearch()`: Main program logic
- `parse_args()`: Command line argument parsing
- `load_bitmap()`: Bitmap loading from file
- `find_hline()`: Horizontal line detection
- `find_vline()`: Vertical line detection
- `find_square()`: Square detection

## Memory Management

The program uses dynamic memory allocation and includes proper cleanup:
- `ctor_bitmap()`: Constructor for bitmap structure
- `dtor_bitmap()`: Destructor for bitmap structure
- `dtor_figsearch()`: Cleanup function for program resources

## Return Values

- 0: Successful execution
- 1: Error condition (invalid input, file not found, etc.)

## Limitations

- Maximum file size is limited by available memory
- Input must be in the specified text format
- Binary (0/1) values only

## Contributing

Please follow these guidelines when contributing:
1. Use descriptive variable names
2. Add comments for complex logic
3. Include error handling
4. Free allocated memory properly
5. Follow existing code style

## License

[Add your license information here]