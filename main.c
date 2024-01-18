#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fileops.jsdrisco.h"

int main() {
    // Open file. Create if it does not exist.
    FILE *fp;
    const char filename[] = "../file.txt";

    fp = (FILE *) fopen(filename, "w+");
    char *jsd = malloc(5 * sizeof(char));
    strcpy(jsd, "John");

    char *ldd = malloc(5 * sizeof(char));
    strcpy(ldd, "Liam");

    insertWord(fp, jsd);
    insertWord(fp, ldd);
    strcpy(ldd, "Lie");
    insertWord(fp, ldd);
    strcpy(ldd, "Lol");
    insertWord(fp, ldd);

    int *j = malloc(sizeof(int));
    countWords(fp, 'J', j);
    printf("%d\n", *j);
    countWords(fp, 'l', j);
    printf("%d\n", *j);
    printf("%s", getWord(fp, 'L', 0));

    free(jsd);
    free(ldd);
    fclose(fp);
    free(j);
    return 0;
}
