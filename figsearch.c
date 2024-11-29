#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations of our "classes"
typedef struct Bitmap Bitmap;
typedef struct SearchResult Search_result;
typedef struct Config Config;

// Structure definitions
// Search result structure
struct SearchResult {
    struct {
        unsigned x, y;
    } start, end;
    unsigned size;
};

// Bitmap structure
struct Bitmap {
    unsigned rows;
    unsigned cols;
    bool *data;
};

// Config structure
struct Config {
    enum {
        MODE_TEST,
        MODE_HLINE,
        MODE_VLINE,
        MODE_SQUARE
    } mode;

    FILE *file;
};

// Help string for the program fig search
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

// Function to search for a figure in a bitmap
int figsearch(int argc, char **argv);

// "Methods" of the SearchResult class
// Create constructor-like function for the configuration
Config *Config_create(int argc, char **argv);
// Destructor-like function for the configuration
void Config_destroy(Config *cfg);

// "Methods" of the Bitmap class
// Create a new bitmap from the file
Bitmap *Bitmap_load_from_file(FILE *file);
// Create a new bitmap
Bitmap *bitmap_create(unsigned rows, unsigned cols);
// Destructor-like function for the bitmap
void Bitmap_destroy(Bitmap *bmp);
// Get the value of a bit in the bitmap
bool Bitmap_get_bit(const Bitmap *bmp, unsigned row, unsigned col);
// Set the value of a bit in the bitmap
void Bitmap_set_bit(Bitmap *bmp, unsigned row, unsigned col, bool value);

// "Methods" of the SearchResult class
// Find the longest horizontal line in the bitmap
void find_hline(Search_result *result, const Bitmap *bmp);
// Find the longest vertical line in the bitmap
void find_vline(Search_result *result, const Bitmap *bmp);
// Check if the row is all ones
void find_square(Search_result *result, const Bitmap *bmp);
// constructor-like functions
Search_result *search_result_create();
// Search result "methods" setter
void search_result_set(Search_result *result, unsigned size,
                       unsigned start_col, unsigned start_row,
                       unsigned end_col, unsigned end_row);
// Destructor-like functions
void Search_result_destroy(Search_result *result);
// validate the input for the search functions
bool validate_search_input(const Bitmap *bmp, const Search_result *result);

int main(int argc, char *argv[]) {
    return figsearch(argc, argv);
}

// Function to destroy the objects
void figsearch_destroy(Bitmap *bmp,Config *cfg, Search_result *result) {
    // Free memory
    Bitmap_destroy(bmp); Config_destroy(cfg); Search_result_destroy(result);
}

// Function to search for a figure in a bitmap
int figsearch(int argc, char *argv[]) {
    Config *cfg = Config_create(argc, argv);
    if (!cfg) return EXIT_FAILURE;

    Bitmap *bmp = Bitmap_load_from_file(cfg->file);
    Search_result *result = search_result_create();
    if (!result) {
        figsearch_destroy(bmp, cfg, result); return EXIT_FAILURE;
    }

    if (cfg->mode == MODE_TEST) {
        if (!bmp) {
            fprintf(stderr, "Invalid\n");
        } else {
            fprintf(stdout, "Valid\n");
        }
        figsearch_destroy(bmp, cfg, result); return EXIT_SUCCESS;
    }
    if (!bmp) {
        fprintf(stderr, "Invalid\n");
        figsearch_destroy(bmp, cfg, result); return EXIT_FAILURE;
    }

    switch (cfg->mode) {
        case MODE_HLINE: find_hline(result, bmp); break;
        case MODE_VLINE: find_vline(result, bmp); break;
        case MODE_SQUARE: find_square(result, bmp); break;
        default:
            fprintf(stderr, "Error: Invalid mode\n");
            Search_result_destroy(result); Bitmap_destroy(bmp);
            Config_destroy(cfg); return EXIT_FAILURE;
    }

    // Print the result y = row, x = col
    printf("%u %u %u %u\n",
           result->start.y, result->start.x,
           result->end.y, result->end.x);

    Search_result_destroy(result); Bitmap_destroy(bmp); Config_destroy(cfg); return EXIT_SUCCESS;
}

// Constructor-like functions
Search_result *search_result_create() {
    Search_result *result = malloc(sizeof(Search_result));
    if (result) {
        result->size = 0;
        result->start.x = 0;
        result->start.y = 0;
        result->end.x = 0;
        result->end.y = 0;
    }
    return result;
}

// Search result "methods" setter
void search_result_set(Search_result *result, unsigned size,
                       unsigned start_col, unsigned start_row,
                       unsigned end_col, unsigned end_row) {
    if (result) {
        result->size = size;
        result->start.x = start_col;
        result->start.y = start_row;
        result->end.x = end_col;
        result->end.y = end_row;
    }
}

// Bitmap constructor
Bitmap *bitmap_create(unsigned rows, unsigned cols) {
    if (rows == 0 || cols == 0) {
        fprintf(stderr, "Output bitmap must have positive dimensions\n");
        return NULL;
    }

    // Allocate memory for the bitmap
    Bitmap *bmp = malloc(sizeof(Bitmap));
    if (!bmp) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Allocate memory for the bitmap data
    bmp->data = malloc(rows * cols * sizeof(bool));
    if (!bmp->data) {
        fprintf(stderr, "Memory allocation failed\n");
        free(bmp);
        return NULL;
    }

    // Initialize the bitmap
    bmp->rows = rows;
    bmp->cols = cols;
    return bmp;
}

Config *Config_create(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: Not enough arguments.\n");
        return NULL;
    }

    Config *cfg = malloc(sizeof(Config));
    if (!cfg) return NULL;

    // Parse command line arguments
    if (strcmp(argv[1], "test") == 0) {
        cfg->mode = MODE_TEST;
    } else if (strcmp(argv[1], "hline") == 0) {
        cfg->mode = MODE_HLINE;
    } else if (strcmp(argv[1], "vline") == 0) {
        cfg->mode = MODE_VLINE;
    } else if (strcmp(argv[1], "square") == 0) {
        cfg->mode = MODE_SQUARE;
    } else if (strcmp(argv[1], "--help") == 0) {
        printf("%s", HELP_STRING);
        free(cfg); exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "Error: Invalid method specified.\n");
        free(cfg); return NULL;
    }

    // Check if the file can be opened
    cfg->file = fopen(argv[2], "r");
    if (!cfg->file) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[2]);
        free(cfg); return NULL;
    }
    return cfg;
}

// Destructor-like functions
void Search_result_destroy(Search_result *result) {
    free(result);
}

// Destructor-like functions for Bitmap
void Bitmap_destroy(Bitmap *bmp) {
    if (bmp) {
        free(bmp->data);
        free(bmp);
    }
}

// Destructor-like functions for Config
void Config_destroy(Config *cfg) {
    if (cfg) {
        if (cfg->file) {
            fclose(cfg->file);
        }
        free(cfg);
    }
}

// Get the value of a bit in the bitmap
bool Bitmap_get_bit(const Bitmap *bmp, unsigned row, unsigned col) {
    if (!bmp || row >= bmp->rows || col >= bmp->cols) {
        fprintf(stderr, "Error: Invalid bitmap access\n");
        exit(EXIT_FAILURE);
    }
    return bmp->data[row * bmp->cols + col];
}

// Set the value of a bit in the bitmap
void Bitmap_set_bit(Bitmap *bmp, unsigned row, unsigned col, bool value) {
    if (bmp && row < bmp->rows && col < bmp->cols) {
        bmp->data[row * bmp->cols + col] = value;
    }
}

// Load a bitmap from a file
Bitmap *Bitmap_load_from_file(FILE *file) {
    // Read the dimensions of the bitmap
    unsigned rows, cols;
    if (fscanf(file, "%u %u", &rows, &cols) != 2) return NULL;

    // Create the bitmap
    Bitmap *bmp = bitmap_create(rows, cols);
    if (!bmp) return NULL;

    // Load the bitmap
    bool has_set_pixels = false;
    unsigned row = 0; unsigned loaded_cols = 0;
    for (; row < rows; row++) {
        for (unsigned col = 0; col < cols; col++) {
            char c = getc(file);
            // If the file ends before the bitmap is loaded, return NULL
            if (c == EOF) {
                Bitmap_destroy(bmp); return NULL;
            }
            // Skip spaces and newlines
            if (c == ' ' || c == '\n') {
                col--; continue;
            }
            // Check if the character is a valid bit
            if (c != '0' && c != '1') {
                Bitmap_destroy(bmp); return NULL;
            }
            // Set the bit in the bitmap
            bool value = (c == '1');
            if (value) has_set_pixels = true;
            Bitmap_set_bit(bmp, row, col, value);
        }
        loaded_cols += cols;
    }

    // Check if the bitmap is valid and has set pixels
    if ((row == rows && loaded_cols == cols && !has_set_pixels) || !has_set_pixels) {
        Bitmap_destroy(bmp); return NULL;
    }
    return bmp;
}

// Validate the input
bool validate_inputs(const Search_result *result, const Bitmap *bmp) {
    return bmp != NULL && result != NULL;
}

// Search for horizontal lines in the bitmap
void find_hline(Search_result *result, const Bitmap *bmp) {
    if (!validate_inputs(result, bmp)) return;
    for (unsigned row = 0; row < bmp->rows; row++) {
        unsigned length = 0;
        for (unsigned col = 0; col < bmp->cols; col++) {
            if (Bitmap_get_bit(bmp, row, col)) {
                length++;
                if (length > result->size) {
                    search_result_set(result, length, col - length + 1, row,
                                      col, row);
                }
            } else {
                length = 0;
            }
        }
    }
}

//Finds the longest vertical line in the bitmap.
void find_vline(Search_result *result, const Bitmap *bmp) {
    if (!validate_inputs(result, bmp)) return;
    for (unsigned col = 0; col < bmp->cols; col++) {
        unsigned length = 0;
        for (unsigned row = 0; row < bmp->rows; row++) {
            if (Bitmap_get_bit(bmp, row, col)) {
                length++;
                if (length > result->size) {
                    search_result_set(result, length, col, row - length + 1,
                                      col, row);
                }
            } else {
                length = 0;
            }
        }
    }
}

// Check if the row is all ones
bool is_row_ones(const Bitmap *bmp, unsigned row, unsigned col, unsigned size) {
    for (unsigned i = 0; i <= size; i++) {
        if (!Bitmap_get_bit(bmp, row, col + i)) return false;
    }
    return true;
}

// Check if the column is all ones
bool is_col_ones(const Bitmap *bmp, unsigned row, unsigned col, unsigned size) {
    for (unsigned i = 0; i <= size; i++) {
        if (!Bitmap_get_bit(bmp, row + i, col)) return false;
    }
    return true;
}

// Finds the largest square in the bitmap.
void find_square(Search_result *result, const Bitmap *bmp) {
    if (!validate_inputs(result, bmp)) return;
    for (unsigned row = 0; row < bmp->rows; row++) {
        for (unsigned col = 0; col < bmp->cols; col++) {
            if (!Bitmap_get_bit(bmp, row, col)) continue;

            // Check if the square can be big enough
            unsigned max_size = (bmp->rows - row < bmp->cols - col ?
                bmp->rows - row: bmp->cols - col);

            if (max_size < result->size) continue;

            // Search for the square
            for (unsigned size = 1; size <= max_size; ++size) {
                unsigned offset = size - 1;

                bool right = Bitmap_get_bit(bmp, row, col + offset);
                bool bottom = Bitmap_get_bit(bmp, row + offset, col);
                bool bottom_right = Bitmap_get_bit(bmp, row + offset, col + offset);

                // Check if the bottom and right are valid
                if (!right || !bottom) break;

                // If the square is valid, and it is the biggest one, save it
                if (is_row_ones(bmp, row + offset, col, offset) &&
                    is_col_ones(bmp, row, col + offset, offset) && size > result->size &&
                    bottom_right) {
                    search_result_set(result, size, col, row,
                                      col + offset, row + offset);
                }
            }
        }
    }
}
