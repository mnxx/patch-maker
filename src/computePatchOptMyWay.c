
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
#define STRING_MAX 100

/* define a pseudo infinite (cost) value */
#define INFINITY INT_MAX

/* enumeration of possible operators */
enum operator {
	ADD, SUB, DEL, MULTI_DEL
};

/* struct previous allows the user to determine how to access a specified (i, j) state */
typedef struct previous {
	unsigned int originalLine;
	unsigned int targetLine;
} *PREVIOUS;

/* return price of adding line sortie to the patch */
int price_line(char* sortie) {
    return 10 + strlen(sortie);// -1 pour \n?
}

/* simply returns 0 if strings are equals or 10 + Length of sortie */
int price(char* entree, char* sortie) {
    if(!strcmp(entree, sortie)) {
        return 0;
    }
    return price_line(sortie);
}

void print_patch(PREVIOUS *path, char** originalBuffer, char** targetBuffer, unsigned int i, unsigned int j) {
    unsigned int previousOriginalLine = path[i][j].originalLine;
    unsigned int previousTargetLine = path[i][j].targetLine;
    if( (i + j) != 0 ) {
        print_patch(path, originalBuffer, targetBuffer, previousOriginalLine, previousTargetLine);
    } else {
        return;
    }
    /* now print the corresponding action */
    /* ADD */
    if ((i == previousOriginalLine) && (j == previousTargetLine + 1)) {
        printf("+ %d\n", i);
        printf("%s", targetBuffer[j]);
    /* DEL */
    } else if ((i == previousOriginalLine + 1) && (j == previousTargetLine)) {
        printf("d %d\n", i);
    /* SUB/COPY */
    } else if ((i == previousOriginalLine + 1) && (j == previousTargetLine + 1)) {
        /* SUB */
        if(strcmp(originalBuffer[i],targetBuffer[j])) {
            printf("= %d\n", i);
            printf("%s", targetBuffer[j]);
        }
    /* MULTI_DEL */
    } else {
        printf("D %d %d\n", previousOriginalLine + 1, i - previousOriginalLine);
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
    int stringMaxOriginal = 0;
    int stringMaxTarget = 0;
    int stringMax = 0;
    /* go through both files - count number of lines and find maximum line length */
    while(fgets(buffer, STRING_MAX, originalFile) != NULL) {
	    nbOriginalLines++;
	    if(strlen(buffer) > stringMaxOriginal){
		    stringMaxOriginal = strlen(buffer);
	    }
    }
    while(fgets(buffer, STRING_MAX, targetFile) != NULL) {
	    nbTargetLines++;
	    if(strlen(buffer) > stringMaxTarget){
		    stringMaxTarget = strlen(buffer);
	    }

    }
    /* files are empty, exit program */
    if(nbOriginalLines == 0 && nbTargetLines == 0) {
	    perror("FILES EMPTY");
	    return EXIT_FAILURE;
    }
    /* find maximum length of a line used in the files */
    stringMax = stringMaxOriginal;
    if(stringMaxOriginal < stringMaxTarget){
	    stringMax = stringMaxTarget;
    }
    /* rewind the files */
    rewind(originalFile);
    rewind(targetFile);
    /* create arrays stocking the string (for comparison) of a line (for cost calculation, we get the length with strlen) */
    unsigned int counter = 0;
    char** originalBuffer = calloc(nbOriginalLines + 1, sizeof(*originalBuffer));
    for(counter = 0; counter < nbOriginalLines + 1; counter++) {
	    originalBuffer[counter] = calloc(stringMax, sizeof(**originalBuffer));
    }
    /* */
    char** targetBuffer = calloc(nbTargetLines + 1, stringMax * sizeof(*targetBuffer));
    for(counter = 0; counter < nbTargetLines + 1; counter++) {
	    targetBuffer[counter] = calloc(stringMax, sizeof(**targetBuffer));
    }
    /* */
    counter = 1;
    while(fgets(originalBuffer[counter++], stringMax + 1, originalFile) != NULL) {
    }
    counter = 1;
    while(fgets(targetBuffer[counter++], stringMax + 1, targetFile) != NULL) {
    }
    
    /* insert random explanation of what is going on here */
    /* path matric to find out where we are coming from */
    PREVIOUS *path = calloc(nbOriginalLines + 1, sizeof(*path)); 
    for(counter = 0; counter < nbOriginalLines + 1; counter++) {
	    path[counter] = calloc(nbTargetLines + 1, sizeof(**path));
    }
    
    /* cost matrix (nbOriginalLines, 2) to calculate prices */
    unsigned int **cost = calloc(nbOriginalLines + 1, sizeof(*cost)); 
    for(counter = 0; counter < nbOriginalLines + 1; counter++) {
	    cost[counter] = calloc(2, sizeof(**cost));
    }
    
    /* this initialize the first column of our price matrix */
    /* modify accordingly if not taking multiple destructions into account */
    cost[0][0] = 0;
    path[0][0].originalLine = 0;
    path[0][0].targetLine = 0;
    cost[1][0] = 10; // simple destruction
    path[1][0].originalLine = 0;
    path[1][0].targetLine = 0;
    for(counter = 2; counter < nbOriginalLines + 1; counter++) {
	    cost[counter][0] = 15;
        path[counter][0].originalLine = 0;
        path[counter][0].targetLine = 0;
        // cost[counter][0] = 10 * counter;
    }
    
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int lineNumber;
    unsigned int optionnalCost;
    unsigned int previousIfDel;
    /* used to store the optimal value during the process */
    unsigned int minCost;
	unsigned int lineIn;
	unsigned int lineOut;
    
    for(lineNumber = 1; lineNumber < nbTargetLines + 1; lineNumber++) {
        j = (lineNumber) % 2;
        k = (lineNumber + 1) % 2;
        minCost = price_line(targetBuffer[lineNumber]);
        cost[0][j] = cost[0][k] + minCost; // adding new line to target file
        /* calculate cost to reach (i, lineNumber) */
        for(i = 1; i < nbOriginalLines + 1; i++) {
            /* initialize with ADD, (a, b - 1) + ADD = (a, b)*/
		    minCost = cost[i][k] + price_line(targetBuffer[lineNumber]);
			lineIn = i;
			lineOut = lineNumber - 1;
            /* check for SUB/COPY, (a - 1, b - 1) + SUB/COPY = (a, b) */
            optionnalCost = cost[i-1][k] + price(originalBuffer[i], targetBuffer[lineNumber]);
		    if(optionnalCost < minCost) {
			    minCost = optionnalCost;
			    lineIn = i - 1;
			    lineOut = lineNumber - 1;
		    }
		    /* check for DEL, (a - 1, b) + DEL = (a, b) */
            optionnalCost = cost[i-1][j] + 10;
		    if(optionnalCost < minCost) {
			    minCost = optionnalCost;
			    lineIn = i - 1;
			    lineOut = lineNumber;
		    }
            /* check for MULTI_DEL, (a - k, b) + MULTIDEL(k) = (a, b) */
            optionnalCost = INFINITY;
            for(counter = 0; counter < i - 1; counter++) {
                if(cost[counter][j] < optionnalCost) {
                    optionnalCost = cost[counter][j];
                    previousIfDel = counter;
                }
            }
            if((optionnalCost != INFINITY) && ((optionnalCost + 15) < minCost)) {
                minCost = optionnalCost + 15;
                lineIn = previousIfDel;
                lineOut = lineNumber;
            }
            
            /* minCost now contains the smalled value possible, we store it in the array */
            cost[i][j] = minCost;
            path[i][lineNumber].originalLine = lineIn;
			path[i][lineNumber].targetLine = lineOut;
        }
    }
    
    /* print patch file in standart output */
    print_patch(path, originalBuffer, targetBuffer, nbOriginalLines, nbTargetLines);
    
    /* free all the reserved memory */
    free(originalBuffer);
    free(targetBuffer);
    for(counter = 0; counter < nbOriginalLines; counter++) {
	    free(cost[counter]);
    }
    free(cost);
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
    
