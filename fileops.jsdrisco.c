#include "fileops.jsdrisco.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function definitions
int insertWord(FILE *fp, char *word) {
    // If file null, word null, or word has invalid length,
    // or word has non-alphanumeric chars, return nonzero
    if(fp == NULL || word == NULL || strlen(word) <= 0 ||
    strlen(word) > MAXWORDLEN || checkWord(word) == 1)
        return 8;

    // Convert to lowercase.
    char *lowerWord = malloc(sizeof(word));
    convertToLower(word, lowerWord);

    // Get index
    int index = (int) (lowerWord[0] - 97);

    // Check: is file empty?
    fseek(fp, 0, SEEK_END);
    int filesize = ftell(fp);

    // If empty, create a file header and write it, along with the word
    if(filesize == 0) {
        // Header creation. No positions except for initial word.
        FileHeader *f = malloc(sizeof(FileHeader));
        for(int i = 0; i < NUMLETTERS; i++) {
            f->counts[i] = 0;
            f->startPositions[i] = 0;
        }
        f->counts[index] = 1;
        f->startPositions[index] = sizeof(FileHeader);
        fwrite(f, sizeof(FileHeader), 1, fp);

        // Seek to the proper file location. Create this word, place it in the file.
        fseek(fp, sizeof(FileHeader), SEEK_SET);
        WordRecord *w = malloc(sizeof(WordRecord));
        strcpy(w->word, lowerWord);
        w->nextpos = 0;
        fwrite(w, sizeof(WordRecord), 1, fp);

        // Free memory
        free(f);
        free(w);
    }
    // If not empty, seek to the corresponding index
    else {
        // Append
        fseek(fp, 8 * index, SEEK_SET);
        long long *val = malloc(sizeof(long long));
        fread(val, sizeof(long long), 1, fp);
        *val = *val + 1;
        fseek(fp, 8 * index, SEEK_SET);
        fwrite(val, sizeof(long long), 1, fp);

        fseek(fp, 8 * index, SEEK_SET);
        fread(val, sizeof(long long), 1, fp);

        // If no other words, edit header and insert word
        if(*val == 1) {
            fseek(fp, 8 * (index + NUMLETTERS), SEEK_SET);
            long long *pos = malloc(sizeof(long long));
            *pos = filesize;
            fwrite(pos, sizeof(long long), 1, fp);

            // Seek to EOF
            fseek(fp, 0, SEEK_END);
            WordRecord *w = malloc(sizeof(WordRecord));
            strcpy(w->word, lowerWord);
            w->nextpos = 0;
            fwrite(w, sizeof(WordRecord), 1, fp);

            // Free memory
            free(w);
            free(pos);
        }
        else {
            long long *nPos = malloc(sizeof(long long));
            *nPos = filesize;
            // If there is already a word(s) beginning with this letter, update the position list
            long long *pos = malloc(sizeof(long long));
            fseek(fp, 8 * (index + NUMLETTERS), SEEK_SET);
            fread(pos, sizeof(long long), 1, fp);
            fseek(fp, *pos, SEEK_SET);
            WordRecord *w = malloc(sizeof(WordRecord));
            fread(w, sizeof(WordRecord), 1, fp);

            // Find the end of the linked list
            while(w->nextpos != 0) {
                *pos = w->nextpos;
                fseek(fp, *pos, SEEK_SET);
                fread(w, sizeof(WordRecord), 1, fp);
            }
            w->nextpos = *nPos;
            fseek(fp, *pos, SEEK_SET);
            fwrite(w, sizeof(WordRecord), 1, fp);

            // Insert new word
            fseek(fp, 0, SEEK_END);
            WordRecord *newWord = malloc(sizeof(WordRecord));
            strcpy(newWord->word, lowerWord);
            newWord->nextpos = 0;
            fwrite(newWord, sizeof(WordRecord), 1, fp);

            free(pos);
            free(nPos);
            free(w);
            free(newWord);
        }

    }

    return 0;
}

int countWords(FILE *fp, char letter, int *count) {
    // If any are NULL, return nonzero
    if(fp == NULL || count == NULL)
        return 8;

    // If non-alphanumeric, return 1
    if(checkWord(&letter) == 1)
        return 1;

    // Convert to lowercase, get index
    char *l = malloc(sizeof(char));
    strcpy(l, &letter);
    char *lowerL = malloc(sizeof(char));
    convertToLower(l, lowerL);
    int index = (int) (*lowerL - 97);

    // Seek to index, get number
    fseek(fp, 8 * index, SEEK_SET);
    long long *val = malloc(sizeof(long long));
    fread(val, sizeof(long long), 1, fp);
    *count = (int) *val;

    // Free, exit
    return 0;
}

char *getWord(FILE *fp, char letter, int index) {
    // If any NULL or out of bounds, return NULL
    if(fp == NULL || index < 0)
        return NULL;

    // Convert to lowercase, get index
    char *l = malloc(sizeof(char));
    *l = letter;

    // If non-alphanumeric, return NULL
    /*if(checkWord(l) == 1)
        return NULL;*/

    strcpy(l, &letter);
    char *lowerL = malloc(sizeof(char));
    convertToLower(l, lowerL);
    int i = (int) (*lowerL - 97);

    // If there are no words beginning with this letter, return NULL
    fseek(fp, 8 * i, SEEK_SET);
    long long *val = malloc(sizeof(long long));
    fread(val, sizeof(long long), 1, fp);
    if(*val == 0)
        return NULL;

    // Get the first position
    long long *pos = malloc(sizeof(long long));
    fseek(fp, 8 * (i + NUMLETTERS), SEEK_SET);
    fread(pos, sizeof(long long), 1, fp);

    // Read the word at the initial position
    fseek(fp, *pos, SEEK_SET);
    WordRecord *w = malloc(sizeof(WordRecord));
    fread(w, sizeof(WordRecord), 1, fp);
    int counter = 0;

    // Continue until we run out of words, or find the word.
    while(counter != index && w->nextpos != 0) {
        *pos = w->nextpos;
        fseek(fp, *pos, SEEK_SET);
        fread(w, sizeof(WordRecord), 1, fp);
        counter++;
    }

    char *rString;

    // If the indexed word exists, return it
    if(counter == index) {
        rString = malloc(sizeof(w->word));
        strcpy(rString, w->word);
    }
    // Otherwise, set it to null
    else {
        rString = NULL;
    }

    // Free, exit
    free(val);
    free(pos);
    free(w);
    return rString;
}

int checkWord(char *word) {
    int i, len;

    len = strlen(word);
    for (i=0; i<len; ++i) {
        if ( ! isalpha(word[i]) )
            return 1;
    }

    return 0;
}

int convertToLower(char *word, char *convertedWord) {
    int i, len;

    strcpy(convertedWord, word);

    len = strlen(word);
    for (i=0; i<len; ++i)
        convertedWord[i] = tolower(word[i]);

    return 0;
}
