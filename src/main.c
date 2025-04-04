#include "termcolor.h"
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

enum lang { Python, Rust, Java, JS, Go, None };
char *lang_names[7] = {"Python", "Rust", "Java", "JavaScript", "Go", ""};
char *lang_symbol[7] = {"🐍 ", "🦀 ", "☕ ", "JS ", "GO ", "  "};

int is_directory(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return !S_ISREG(path_stat.st_mode);
}

int dont_show(char *path) {
    return (0 == strcmp(path, ".") || 0 == strcmp(path, ".."));
}

int has_git(char *path) {
    char git_path[60];
    strcpy(git_path, path);
    strcat(git_path, "/.git");

    return access(git_path, F_OK) == 0;
}

int is_local(char *path) {
    char git_path[60];
    FILE *fp;
    char buffer[256];
    char new_buf[20];
    const char search_chars[4] = "url";

    strcpy(git_path, path);
    strcat(git_path, "/.git/config");

    fp = fopen(git_path, "r");
    if (fp == NULL) {
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, search_chars) != NULL) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return 1;
}

int has_sub_file(char *path, char *sub) {
    char sub_path[60];
    strcpy(sub_path, path);
    strcat(sub_path, sub);

    return access(sub_path, F_OK) == 0;
}

enum lang get_lang(char *path) {
    if (has_sub_file(path, "/setup.py")) {
        return Python;
    } else if (has_sub_file(path, "/package.json")) {
        return JS;
    } else if (has_sub_file(path, "/Cargo.toml")) {
        return Rust;
    } else if (has_sub_file(path, "/pom.xml")) {
        return Java;
    } else if (has_sub_file(path, "/go.mod")) {
        return Go;
    }

    return None;
}

void print_dir(char *name, int *zp) {
    if (*zp == FG_CYAN) {
        (*zp)--;
    } else {
        (*zp)++;
    }

    if (is_directory(name) && !dont_show(name)) {
        int check_has_git = has_git(name);
        if (check_has_git) {
            cprint("git", FG_GREEN);
        } else {
            cprint("---", *zp);
        }

        if (check_has_git && is_local(name)) {
            cprint(" local", FG_YELLOW);
        } else {
            cprint(" -----", *zp);
        }

        int lang_type = get_lang(name);
        char *lang_name = lang_names[lang_type];
        char *lang_sym = lang_symbol[lang_type];
        if (lang_type != None) {
            printf(" %-12s", lang_name);
            printf(" %s", lang_sym);
        } else {
            cprint(" ------------", *zp);
            cprint(" -- ", *zp);
        }

        printf("%s\n", name);
    }
}

void show_mode() {
    struct dirent *de;
    DIR *dr = opendir(".");
    int zebra = FG_PURPLE;

    if (dr == NULL) {
        printf("Could not be opened");
        exit(EXIT_FAILURE);
    }

    while ((de = readdir(dr)) != NULL) {
        print_dir(de->d_name, &zebra);
    }

    closedir(dr);
}

int count_dirs() {
    struct dirent *de;
    DIR *dr = opendir(".");
    int count = 0;

    if (dr == NULL) {
        printf("Could not be opened");
        exit(EXIT_FAILURE);
    }

    while ((de = readdir(dr)) != NULL) {
        char *name = de->d_name;
        if (is_directory(name) && !dont_show(name)) {
            count++;
        }
    }

    closedir(dr);
    return count;
}

void random_mode() {
    struct dirent *de;
    DIR *dr = opendir(".");
    int index = 0;

    int count = count_dirs();

    srand(time(0));
    int random_index = (rand() % (count + 1));

    if (dr == NULL) {
        printf("Could not be opened");
        exit(EXIT_FAILURE);
    }

    while ((de = readdir(dr)) != NULL) {
        char *name = de->d_name;
        if (is_directory(name) && !dont_show(name)) {
            index++;

            int color = FG_CYAN;
            if (index == random_index) {
                print_dir(name, &color);
            }
        }
    }

    closedir(dr);
}

int main(int argc, char *argv[]) {

    int opt;
    enum { SHOW, RANDOM } mode = SHOW;

    while ((opt = getopt(argc, argv, "sr")) != -1) {
        switch (opt) {
        case 's':
            mode = SHOW;
            break;
        case 'r':
            mode = RANDOM;
            break;
        default:
            fprintf(stderr, "Usage: %s [-sr]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    switch (mode) {
    case SHOW:
        show_mode();
        break;

    case RANDOM:
        random_mode();
        break;
    }

    return 0;
}
