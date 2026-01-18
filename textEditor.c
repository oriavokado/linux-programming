/*
 * Line-based Text Editor (ed-style)
 *
 * Commands:
 *  *      : Display all lines
 *  n      : Display line n
 *  n-m    : Display lines from n to m
 *  n,m,.. : Display multiple specific lines
 *  p n    : Print line n
 *
 *  a n    : Insert a new line at position n
 *  d n    : Delete line n
 *  s n    : Replace the content of line n
 *
 *  q      : Quit the editor
 */

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define SIZE 50

void print_line(char input[], char* lines[], int total_lines);
void save_file(char* filename, char* lines[], int total_lines);

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("error");
        return 1;
    }

    char input[SIZE];
    char buffer[SIZE];
    FILE* fp;
    int line = 0;
    char* lines[15] = { NULL };

    if ((fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "error\n");
        return 1;
    }

    while (fgets(buffer, SIZE, fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        lines[line] = strdup(buffer);
        line++;
    }
    fclose(fp);

    int n;
    char cmd;
    while (1) {
        printf(">>");
        scanf(" %[^\n]%*c", input);
  

        if (strcmp(input, "q") == 0)
            break;
        else if (strcmp(input, "*") == 0)
            print_line(input, lines, line);
        else if (isdigit(input[0]))
            print_line(input, lines, line);
        else if (strstr(input, "p")) {
            sscanf(input, "p %s", input);
            print_line(input, lines, line);
        }
        else if (strstr(input, "d")) {
            sscanf(input, "d %d", &n);
            if (n > 0 && n <= line && lines[n - 1] != NULL) {

                for (int i = n - 1; i < line - 1; i++) {
                    lines[i] = lines[i + 1];
                }
                lines[line - 1] = NULL;
                line--;
                save_file(argv[1], lines, line);
                printf("Deleted line %d\n", n);
            }
            else {
                printf("Invalid number\n");
            }
        }
        else if (strstr(input, "a")) {
            sscanf(input, "a %d", &n);

            if (n > 0 && n <= line + 1) {
                printf("Enter text to insert at line %d: ", n);
                fgets(buffer, SIZE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                for (int i = line; i >= n; i--) {
                    lines[i] = lines[i - 1];
                }
                lines[n - 1] = strdup(buffer);
                line++;
                save_file(argv[1], lines, line);
                printf("Line inserted at %d\n", n);
            }
            else {
                printf("Invalid number\n");
            }
        }
        else if (strstr(input, "s")) {
            sscanf(input, "s %d", &n) == 1;
            

            if (n > 0 && n <= line && lines[n - 1] != NULL) {
                printf("Enter replacement text for line %d: ", n);
                fgets(buffer, SIZE, stdin);
                buffer[strcspn(buffer, "\n")] = '\0';
                free(lines[n - 1]);
                lines[n - 1] = strdup(buffer);
                save_file(argv[1], lines, line);
                printf("Line %d updated\n", n);
            }
            else {
                printf("Invalid number\n");
            }
        }
        else
            printf("error\n");
    }

    for (int i = 0; i < line; i++)
        free(lines[i]);

    return 0;
}

void print_line(char input[], char* lines[], int total_lines) {
    int i = 0;

    if (strstr(input, "-")) {
        int start, end;
        if (sscanf(input, "%d-%d", &start, &end) == 2) {
            for (i = start; i <= end; i++) {
                if (i > 0 && i <= total_lines && lines[i - 1] != NULL) {
                    printf("%d: %s\n", i, lines[i - 1]);
                }
            }
        }
        else {
            printf("Invalid range\n");
        }
    }
    else if (strcmp(input, "*") == 0) {
        for (i = 0; lines[i] != NULL; i++)
            printf("%d: %s\n", i + 1, lines[i]);
    }
    else if (strchr(input, ',')) {
        char* token = strtok(input, ",");
        while (token != NULL) {
            int num;
            sscanf(token, "%d", &num);
            if (num > 0 && num <= total_lines && lines[num - 1] != NULL) {
                printf("%d: %s\n", num, lines[num - 1]);
            }
            token = strtok(NULL, ",");
        }
    }
    else if (sscanf(input, "%d", &i) == 1) {
        if (i > 0 && i <= total_lines && lines[i - 1] != NULL) {
            printf("%d: %s\n", i, lines[i - 1]);
        }
        else {
            printf("Invalid number\n");
        }
    }
}

void save_file(char* filename, char* lines[], int total_lines) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error saving file\n");
        return;
    }

    for (int i = 0; i < total_lines; i++) {
        if (lines[i] != NULL) {
            fprintf(fp, "%s\n", lines[i]);
        }
    }

    fclose(fp);
}
