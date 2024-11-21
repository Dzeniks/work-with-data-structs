#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x, y;
} vector;

typedef struct {
    vector start, end;
    int size;
} result;

typedef struct {
    int rows, cols;
    bool *data;
} bitmap;

typedef struct {
    bool test, hline, vline, square;
    FILE *file;
} config;

const char *HELP_STRING =
        "Usage: [method] [filename]\n"
        "\n"
        "Methods:\n"
        "  hline    Find horizontal lines in the input image\n"
        "  vline    Find vertical lines in the input image\n"
        "  square   Find squares in the input image\n"
        "\n"
        "Arguments:\n"
        "  method      Processing method (hline, vline, or square)\n"
        "  filename    Path to input bitmap file in txt format\n"
        "\n"
        "Options:\n"
        "  --help    Display this help message\n"
        "\n"
        "Example:\n"
        "  hline img.txt    # Find horizontal lines in img.txt\n";

// run the figsearch function ("program")
int figsearch(int argc, char *argv[]);

config parse_args(int argc, char *argv[]);

bitmap *load_bitmap(FILE *file);

result *find_hline(const bitmap *bmp);

result *find_vline(const bitmap *bmp);

result *find_square(const bitmap *bmp);

void dtor_bitmap(bitmap *bmp);

bool *get_item(const bitmap *bmp, int x, int y);


// Call the figsearch function to start the program
int main(int argc, char *argv[]) {
    return figsearch(argc, argv);
}

void dtor_figsearch(bitmap *bmp, config cfg, result *result) {
    dtor_bitmap(bmp);
    if (cfg.file != NULL) {
        fclose(cfg.file);
    }
    free(result);
}

// run the figsearch function ("program") and return 0 if successful and 1 if not
int figsearch(int argc, char *argv[]) {
    const config cfg = parse_args(argc, argv);
    bitmap *bmp = load_bitmap(cfg.file);
    // If bitmap is not null and test is true print valid and return 0
    if (bmp != NULL && cfg.test) {
        printf("Valid\n");
        dtor_figsearch(bmp, cfg, NULL);
        return 0;
    }
    // If bitmap is null and test is true print invalid and return 1
    if (bmp == NULL && cfg.test) {
        printf("Invalid\n");
        dtor_figsearch(bmp, cfg, NULL);
        return 1;
    }
    // If bitmap is null and test is false print error and return 1
    if (bmp == NULL) {
        fprintf(stderr, "Not found\n");
        dtor_figsearch(bmp, cfg, NULL);
        return 1;
    }

    // Method selection and execution
    result *result = NULL;
    if (cfg.hline) {
        result = find_hline(bmp);
    } else if (cfg.vline) {
        result = find_vline(bmp);
    } else if (cfg.square) {
        result = find_square(bmp);
    }

    if (result == NULL) {
        fprintf(stderr, "Error finding result\n");
        dtor_figsearch(bmp, cfg, NULL);
        return 1;
    }

    // Print result
    printf("%d %d %d %d\n", result->start.x, result->start.y, result->end.x, result->end.y);

    // Cleanup
    dtor_figsearch(bmp, cfg, result);
    return 0;
}

config parse_args(int argc, char *argv[]) {
    config cfg = {false, false, false, false, NULL};
    const int POS_METHOD = 1;
    const int POS_FILE = 2;

    // Check if some arguments are provided
    if (argc < 2) {
        fprintf(stderr, "Error: Not enough arguments.\n");
        exit(EXIT_FAILURE);
    }

    // Check if help is requested
    if (strcmp(argv[POS_METHOD], "--help") == 0) {
        printf("%s", HELP_STRING);
        exit(EXIT_SUCCESS);
    }

    // Parse method

    if (strcmp(argv[POS_METHOD], "test") == 0) {
        cfg.test = true;
    }
    else if (strcmp(argv[POS_METHOD], "hline") == 0) {
        cfg.hline = true;
    } else if (strcmp(argv[POS_METHOD], "vline") == 0) {
        cfg.vline = true;
    } else if (strcmp(argv[POS_METHOD], "square") == 0) {
        cfg.square = true;
    }

    // Check if method was specified
    if ((cfg.test + cfg.hline + cfg.vline + cfg.square) == 0) {
        fprintf(stderr, "Error: No method specified.\n");
        exit(EXIT_FAILURE);
    }

    // Check if more than one method was specified
    if ((cfg.test + cfg.hline + cfg.vline + cfg.square) > 1) {
        fprintf(stderr, "Error: Only one method can be specified.\n");
        exit(EXIT_FAILURE);
    }

    // Check if file exists
    FILE *file = fopen(argv[POS_FILE], "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(EXIT_FAILURE);
    }
    cfg.file = file;
    return cfg;
}


// Constructor for bitmap structure
bitmap *ctor_bitmap(int width, int height) {
    // Check if dimensions are valid
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    // Allocate memory for bitmap
    bitmap *bmp = malloc(sizeof(bitmap));
    if (bmp == NULL) {
        return NULL;
    }

    // Allocate memory for bitmap data
    bool *data = malloc(width * height * sizeof(bool));
    if (data == NULL) {
        free(bmp);
        return NULL;
    }

    // Initialize bitmap structure with provided values
    bmp->rows = width;
    bmp->cols = height;
    bmp->data = data;
    return bmp;
}

// Get item from bitmap on specified coordinates
bool *get_item(const bitmap *bmp, const int x, const int y) {
    if (x < 0 || x >= bmp->rows || y < 0 || y >= bmp->cols) {
        return NULL;
    }
    return &bmp->data[x * bmp->cols + y];
}

// Destructor for bitmap structure
void dtor_bitmap(bitmap *bmp) {
    if (bmp == NULL) {
        return;
    }

    if (bmp->data != NULL) {
        free(bmp->data);
        bmp->data = NULL;
    }
    free(bmp);
}

// Read bitmap from file
bitmap *load_bitmap(FILE *file) {
    // Read dimensions from first line
    int rows, cols;
    if (fscanf(file, "%d %d", &rows, &cols) != 2) {
        fprintf(stderr, "Error reading dimensions\n");
        fclose(file);
        return NULL;
    }

    // Allocate memory for bitmap
    bitmap *bmp = ctor_bitmap(rows, cols);
    if (bmp == NULL) {
        return NULL;
    }

    // Read data from file
    bool is_bitmap_empty = false;
    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            char char_value;
            int value = -1;
            fscanf(file, " %c", &char_value);
            if (char_value == '0' || char_value == '1') {
                // Char to int
                value = char_value - '0';
            } else if (char_value != ' ') {
                fprintf(stderr, "Error: Invalid value in bitmap\n");
                return NULL;
            }
            if (value == 1) {
                is_bitmap_empty = true;
            }
            if (value != 0 && value != 1) {
                fprintf(stderr, "Error: Invalid value in bitmap\n");
                return NULL;
            }
            bmp->data[i * rows + j] = value;
        }
    }

    // If bitmap is empty return error (bitmap is not in correct format)
    if (!is_bitmap_empty) {
        return NULL;
    }

    // If program did not reach end of file return error (bitmap is not in correct format)
    if (fgetc(file) != EOF) {
        return NULL;
    }

    return bmp;
}

// Find the longest horizontal line in the bitmap
result *find_hline(const bitmap *bmp) {
    result *longest_row = malloc(sizeof(result));
    if (longest_row == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    // Go through all rows and find the longest row
    for (int row = 0; row < bmp->rows; row++) {
        int length = 0;
        for (int col = 0; col < bmp->cols; col++) {
            bool *item = get_item(bmp, row, col);
            if (item != NULL && *item) {
                length++;
            }
            if (length > longest_row->size) {
                longest_row->size = length;
                longest_row->start.x = row;
                longest_row->start.y = col - length + 1;
                longest_row->end.x = row;
                longest_row->end.y = col;
            }
            if (item == NULL || !*item) {
                length = 0;
            }
        }
    }
    return longest_row;
}

// Find the longest vertical line in the bitmap
result *find_vline(const bitmap *bmp) {
    result *longest_column = malloc(sizeof(result));
    if (longest_column == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        free(longest_column);
        return NULL;
    }

    // Go through all columns and find the longest column
    for (int col = 0; col < bmp->cols; col++) {
        int length = 0;
        for (int row = 0; row < bmp->rows; row++) {
            bool *item = get_item(bmp, row, col);
            if (item != NULL && *item) {
                length++;
            }
            if (length > longest_column->size) {
                longest_column->size = length;
                longest_column->start.x = row - length + 1;
                longest_column->start.y = col;
                longest_column->end.x = row;
                longest_column->end.y = col;
            }
            if (item == NULL || !*item) {
                length = 0;
            }
        }
    }
    return longest_column;
}

// Check if specified row is all 1's from start column to size
bool check_if_row_is_ones(const bitmap *bmp, const int row, const int col, const int size) {
    for (int i = 0; i < size; i++) {
        bool *item = get_item(bmp, row, col + i);
        if (item == NULL || !*item) {
            return false;
        }
    }
    return true;
}

// Check if specified column is all 1's from start row to size
bool check_if_col_is_ones(const bitmap *bmp, const int row, const int col, const int size) {
    for (int i = 0; i < size; i++) {
        bool *item = get_item(bmp, row + i, col);
        if (item == NULL || !*item) {
            return false;
        }
    }
    return true;
}

result *find_square(const bitmap *bmp) {
    result *square = malloc(sizeof(result));
    // Set initial size to -1
    square->size = -1;
    for (int row = 0; row < bmp->rows; row++) {
        for (int col = 0; col < bmp->cols; col++) {
            // Get start item of possible square (Top left corner)
            bool *start_item = get_item(bmp, row, col);

            // If item is not valid or not 1 continue to next start item
            if (start_item == NULL || !*start_item) {
                continue;
            }
            // If start item is valid we found first item of possible square
            int size = 0;

            // Check if can be found on current row bigger square
            if ((row - bmp->rows < square->size)) {
                continue;
            }

            // While in bounds search for square
            while (row + size < bmp->rows && col + size < bmp->cols) {
                bool *right = get_item(bmp, row + size, col);
                bool *bottom = get_item(bmp, row, col + size);
                bool *bottom_right = get_item(bmp, row + size, col + size);

                // Check if all conditions are true to continue to search if not break
                if (!(right != NULL && *right && bottom != NULL && *bottom)) {
                    break;
                }

                // if found possible and bigger square - check right column and bottom row of possible square are valid
                if (bottom_right != NULL && *bottom_right && size > square->size) {
                    bool is_valid_right = check_if_col_is_ones(bmp, row, col + size, size + 1);
                    bool is_valid_bottom = check_if_row_is_ones(bmp, row + size, col, size + 1);
                    // If valid and size is bigger than current square save it and continue to search
                    if (is_valid_right && is_valid_bottom && size > square->size) {
                        square->size = size;
                        square->start.x = row;
                        square->start.y = col;
                        square->end.x = row + size;
                        square->end.y = col + size;
                    }
                }
                size++;
            }
        }
    }
    return square;
}
