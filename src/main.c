#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int is_directory(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return !S_ISREG(path_stat.st_mode);
}

int dont_show(char *path) {
    return (0 == strcmp(path, ".") || 0 == strcmp(path, ".."));
}

int main() {
    struct dirent *de;

    DIR *dr = opendir(".");

    if (dr == NULL) {
        printf("Could not be opened");
        return 0;
    }

    while ((de = readdir(dr)) != NULL) {
        if (is_directory(de->d_name) && !dont_show(de->d_name)) {
            printf("%s\n", de->d_name);
        }
    }

    closedir(dr);

    return 0;
}
