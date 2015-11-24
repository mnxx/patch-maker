/*! \file computePatchOpt.c
 *  \brief     Source file for the computePatchOpt program.
 *  \author    Franck Maillot
 *  \author    Manuel Roth
 *  \version   1.0
 *  \date      2015
 *  \warning   Usage: computePatchOpt originalFile targetFile
 *  \copyright MIT Public License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* define the maximum length of a line */
#define STRING_MAX 256

/* define a pseudo infinite (cost) value */
#define INFINITY INT_MAX


char** originalBuffer;
char** targetBuffer;
int** path;



/* return price of adding line sortie to the patch */
unsigned int price_line(unsigned int targetLineNumber) {
    return 10 + strlen(targetBuffer[targetLineNumber]);// -1 pour \n?
}

/* simply returns 0 if strings are equals or 10 + Length of sortie */
unsigned int price(unsigned int originalLineNumber, unsigned int targetLineNumber) {
    if(strcmp(originalBuffer[originalLineNumber], targetBuffer[targetLineNumber]) == 0) {
        return 0;
    }
    return price_line(targetLineNumber);
}

void print_patch(unsigned int i, unsigned int j) {
    int previous = path[j][i];
    if((i + j) == 0 ) {
        return;
    }
    /* now print the corresponding action */
    /* DEL */
    if (previous == -1) {
        print_patch((i - 1), j);
        printf("d %d\n", i);
    /* ADD */
    } else if (previous == 0) {
        print_patch(i, (j - 1));
        printf("+ %d\n", i);
        printf("%s", targetBuffer[j]);
    /* COPY */
    } else if (previous == 1) {
        print_patch((i - 1), (j - 1));
    /* SUB */
    } else if (previous == 2) {
        print_patch((i - 1), (j - 1));
        printf("= %d\n", i);
        printf("%s", targetBuffer[j]);
    /* MULTI_DEL */
    } else {
        print_patch((i + previous), j);
        printf("D %d %d\n", (i + previous + 1), (- previous));
    }
}


/*!
 * Function that implements computePatchOpt
 * @param  originalFile File pointer to the original file.
 * @param  targetFile File pointer to the target file.
 * \returns { 0 if succeeds, exit code otherwise}
 */
int computePatchOpt(FILE *originalFile, FILE *targetFile) {
    /* define needed variables to read the files more efficiently and to keep the # of lines */
    char buffer[STRING_MAX];
    unsigned int nbOriginalLines = 0;
    unsigned int nbTargetLines = 0;
    unsigned int counter = 0;
    /* go through both files - count number of lines and store strings for faster acces */
    while(fgets(buffer, STRING_MAX, originalFile) != NULL) {
            nbOriginalLines++;
    }
    while(fgets(buffer, STRING_MAX, targetFile) != NULL) {
            nbTargetLines++;
    }
    /* files are empty, exit program */
    if(nbOriginalLines == 0 && nbTargetLines == 0) {
            perror("FILES EMPTY");
            return EXIT_FAILURE;
    }
    /* rewind the files */
    rewind(originalFile);
    rewind(targetFile);
    /* create arrays stocking the string (for comparison) of a line (for cost calculation, we get the length with strlen) */
    originalBuffer = calloc(nbOriginalLines + 1, sizeof(*originalBuffer));
    counter = 1;
    while(fgets(buffer, STRING_MAX, originalFile) != NULL) {
            originalBuffer[counter++] = strdup(buffer);
    }
    /* */
    targetBuffer = calloc(nbTargetLines + 1, sizeof(*targetBuffer));
    counter = 1;
    while(fgets(buffer, STRING_MAX, targetFile) != NULL) {
            targetBuffer[counter++] = strdup(buffer);
    }

    /* insert random explanation of what is going on here */
    /* path matric to find out where we are coming from */
    path = malloc(((nbTargetLines + 1) * sizeof(*path)));
    for(counter = 0; counter < nbTargetLines + 1; counter++) {
            path[counter] = malloc(((nbOriginalLines + 1) * sizeof(**path)));
    }

    /* cost(nbOriginalLines, 2) to calculate prices */
    unsigned int **cost = malloc((2  * sizeof(*cost)));
    for(counter = 0; counter < 2; counter++) {
            cost[counter] = malloc(((nbOriginalLines + 1) * sizeof(**cost)));
    }

    /* this initialize the first column of our price matrix */
    /* modify accordingly if not taking multiple destructions into account */
    cost[0][0] = 0;
    path[0][0] = 0;
    cost[0][1] = 10; // simple destruction
    path[0][1] = -1;
    for(counter = 2; counter < nbOriginalLines + 1; counter++) {
        cost[0][counter] = 15;
        path[0][counter] = -counter;
        //cost[0][counter] = 10 * counter;
        //path[0][counter] = -1;
    }

    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int lineNumber;
    unsigned int optionalCost;
    /* used to store the optimal value during the process */
    unsigned int minCost;
    int path_taken;
    unsigned int optionalMultiDel;
    int counterMultiDel;

    for(lineNumber = 1; lineNumber < nbTargetLines + 1; lineNumber++) {
        j = (lineNumber) % 2;
        k = (lineNumber + 1) % 2;
        minCost = price_line(lineNumber);
        cost[j][0] = cost[k][0] + minCost; // adding new line to target file
        path[lineNumber][0] = 0;
        optionalMultiDel = INFINITY;
        /* calculate cost to reach (i, lineNumber) */
        for(i = 1; i < nbOriginalLines + 1; i++) {
            /* initialize with ADD, (a, b - 1) + ADD = (a, b)*/
            minCost = cost[k][i] + price_line(lineNumber);
            path_taken = 0;
            /* check for SUB/COPY, (a - 1, b - 1) + SUB/COPY = (a, b) */
            optionalCost = cost[k][i-1] + price(i, lineNumber);
            if(optionalCost < minCost) {
                minCost = optionalCost;
                path_taken = 1;
                /* Case SUB */
                if((minCost - cost[k][i-1]) != 0) {
                    path_taken = 2;
                }
            }
            /* check for DEL, (a - 1, b) + DEL = (a, b) */
            optionalCost = cost[j][i-1] + 10;
            if(optionalCost < minCost) {
                minCost = optionalCost;
                path_taken = -1;
            }
            /* check for MULTI_DEL, (a - k, b) + MULTIDEL(k) = (a, b) */
            if((optionalMultiDel != INFINITY) && ((optionalMultiDel + 15) < minCost)) {
                minCost = optionalMultiDel + 15;
                path_taken = counterMultiDel - i;
            }
            if(cost[j][i-1] < optionalMultiDel) {
                optionalMultiDel = cost[j][i-1];
                counterMultiDel = i - 1;
            }

            /* minCost now contains the smallest value possible, we store it in the array */
            cost[j][i] = minCost;
            path[lineNumber][i] = path_taken;
        }
    }

    /* print patch file in standart output */
    print_patch(nbOriginalLines, nbTargetLines);

    /* free all the reserved memory */
    // originalBuffer
    for(counter = 0; counter < nbOriginalLines; counter++) {
            free(originalBuffer[counter]);
    }
    free(originalBuffer);
    // targetBuffer
    for(counter = 0; counter < nbTargetLines; counter++) {
            free(targetBuffer[counter]);
    }
    free(targetBuffer);
    // cost
    for(counter = 0; counter < 2; counter++) {
            free(cost[counter]);
    }
    free(cost);
    // path
    for(counter = 0; counter < nbTargetLines + 1; counter++) {
            free(path[counter]);
    }
    return 0;
}


/**
 * Main function
 * \brief Main function
 * \param argc  A count of the number of command-line arguments
 * \param argv  An argument vector of the command-line arguments.
 * \warning Must be called with two filenames patchFile, originalFile as commandline parameters and in the given order.
 * \returns { 0 if succeeds, exit code otherwise}
 */
int main(int argc, char *argv[]) {
    FILE *originalFile;
    FILE *targetFile;

    if(argc<3){
            fprintf(stderr, "!!!!! Usage: ./computePatchOpt originalFile targetFile !!!!!\n");
            exit(EXIT_FAILURE); /* indicate failure.*/
    }

    originalFile = fopen(argv[1] , "r" );
    targetFile = fopen(argv[2] , "r" );

    if (originalFile==NULL) {fprintf(stderr, "!!!!! Error opening originalFile !!!!! \n"); exit(EXIT_FAILURE);}
    if (targetFile==NULL) {fprintf (stderr, "!!!!! Error opening targetFile !!!!!\n"); exit(EXIT_FAILURE);}

    computePatchOpt(originalFile, targetFile);

    fclose(originalFile);
    fclose(targetFile);
    return 0;
}
