#ifndef FILES_FILEOPS_JSDRISCO_H
#define FILES_FILEOPS_JSDRISCO_H

// Library includes -------------------------------------------------------------
#include <stdio.h>

// Global variable definitions ---------------------------------------------------
#define NUMLETTERS 26
#define MAXWORDLEN 31
#define HEADER_SIZE 416

// Struct definitions -------------------------------------------------------------
typedef struct {
    long long counts[NUMLETTERS];
    long long startPositions[NUMLETTERS];
} FileHeader;

typedef struct {
    char word[1 + MAXWORDLEN];
    long long nextpos;
} WordRecord;

// Function templates --------------------------------------------------------------
int insertWord(FILE *fp, char *word);

int countWords(FILE *fp, char letter, int *count);

char *getWord(FILE *fp, char letter, int index);

int checkWord(char *word);

int convertToLower(char *word, char *convertedWord);

#endif //FILES_FILEOPS_JSDRISCO_H