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

int main(int argc, char *argv[]) {
    printf("Hello, World!\n");
    config cfg = parse_args(argc, argv);
    printf(cfg.hline ? "hline\n" : "");
    printf("Good bey, World!\n");
    return 0;
}
