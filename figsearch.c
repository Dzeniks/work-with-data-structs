#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int x, y;
} vector;

typedef struct {
    vector start, end;
    int len;
} line;

typedef struct {
    int width, height;
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

    bmp->width = width;
    bmp->height = height;
    bmp->data = data;

    return bmp;
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

// Call the figsearch function to start the program
int main(int argc, char *argv[]) {
    return figsearch(argc, argv);
}

void dtor_figsearch(bitmap *bmp, config cfg) {
    dtor_bitmap(bmp);
    if (cfg.file != NULL) {
        fclose(cfg.file);
    }
}

int figsearch(int argc, char *argv[]) {
    printf("Hello, World!\n");
    const config cfg = parse_args(argc, argv);
    bitmap *bmp = load_bitmap(cfg.file);
    if (bmp == NULL) {
        fprintf(stderr, "Error loading bitmap\n");
        return 1;
    }

    printf("width: %d, height: %d\n", bmp->width, bmp->height);

    printf("Good bey, World!\n");
    dtor_figsearch(bmp, cfg);
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

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            int value;
            fscanf(file, "%d", &value);
            bmp->data[i * rows + j] = value;
        }
    }
    return bmp;
}