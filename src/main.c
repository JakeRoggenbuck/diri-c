#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "termcolor.h"

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

int main() {
    struct dirent *de;

    DIR *dr = opendir(".");

    if (dr == NULL) {
        printf("Could not be opened");
        return 0;
    }

    while ((de = readdir(dr)) != NULL) {
        char *name = de->d_name;
        if (is_directory(name) && !dont_show(name)) {
            if (has_git(name)) {
                cprint("HAS GIT ", FG_GREEN);
            }

            printf("%s\n", name);
        }
    }

    closedir(dr);

    return 0;
}
