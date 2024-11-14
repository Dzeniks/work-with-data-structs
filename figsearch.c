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
    bool hline, vline, square;
    FILE *file;
} config;

// Constructor for bitmap structure
bitmap *ctor_bitmap(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    bitmap *bmp = malloc(sizeof(bitmap));
    if (bmp == NULL) {
        return NULL;
    }

    bool *data = malloc(width * height * sizeof(bool));
    if (data == NULL) {
        free(bmp);
        return NULL;
    }

    bmp->rows = width;
    bmp->cols = height;
    bmp->data = data;

    return bmp;
}

bool *get_item(bitmap *bmp, int x, int y) {
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
result *find_hline(bitmap *bmp);
result *find_vline(bitmap *bmp);
result *find_square(bitmap *bmp);

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
    printf("Hello, World!\n");
    const config cfg = parse_args(argc, argv);
    bitmap *bmp = load_bitmap(cfg.file);
    if (bmp == NULL) {
        fprintf(stderr, "Error loading bitmap\n");
        dtor_figsearch(bmp, cfg, NULL);
        return 1;
    }

    // TODO: Remove later prints only for debugging
    // Print bitmap dimensions
    printf("Bitmap dimensions: %d x %d\n", bmp->rows, bmp->cols);
    // Print bitmap data
    for (int row = 0; row < bmp->rows; row++) {
        for (int col = 0; col < bmp->cols; col++) {
            bool *item = get_item(bmp, row, col);
            printf("%d ", *item);
        }
        printf("\n");
    }
    printf("\n");

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

    printf("%d %d %d %d\n", result->start.x, result->start.y, result->end.x, result->end.y);

    dtor_figsearch(bmp, cfg, result);
    return 0;
}

config parse_args(int argc, char *argv[]) {
    config cfg = {false, false, false, NULL};
    int POS_METHOD = 1;
    int POS_FILE = 2;

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
    if (strcmp(argv[POS_METHOD], "hline") == 0) {
        cfg.hline = true;
    } else if (strcmp(argv[POS_METHOD], "vline") == 0) {
        cfg.vline = true;
    } else if (strcmp(argv[POS_METHOD], "square") == 0) {
        cfg.square = true;
    }

    // Check if method was specified
    if ((cfg.hline + cfg.vline + cfg.square) == 0) {
        fprintf(stderr, "Error: No method specified.\n");
        exit(EXIT_FAILURE);
    }

    // Check if more than one method was specified
    if ((cfg.hline + cfg.vline + cfg.square) > 1) {
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
            int value;
            fscanf(file, "%d", &value);
            if (value == 1) {
                is_bitmap_empty = true;
            }
            bmp->data[i * rows + j] = value;
        }
    }

    if (!is_bitmap_empty) {
        fprintf(stderr, "Error: Bitmap is empty\n");
        return NULL;
    }

    return bmp;
}

// Find the longest horizontal line in the bitmap
result *find_hline(bitmap *bmp) {
    result *longest_row = malloc(sizeof(result));
    if (longest_row == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

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
result *find_vline(bitmap *bmp) {
    result *longest_column = malloc(sizeof(result));
    if (longest_column == NULL) {
        fprintf(stderr, "Error allocating memory\n");
        free(longest_column);
        return NULL;
    }

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

result *find_square(bitmap *bmp) {
    return NULL;
}
