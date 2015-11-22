
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

/* link structure allows us to compare operations for two lines concerning their cost */
typedef struct link {
	unsigned int cost;
	unsigned int originalLine;
	unsigned int targetLine;
} link;

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
    char originalBuffer[nbOriginalLines][stringMax];
    char targetBuffer[nbTargetLines][stringMax];
    unsigned int counter = 0;
    while(fgets(originalBuffer[counter], stringMax, originalFile) != NULL) {
	    counter++;
    }
    counter = 0;
    while(fgets(targetBuffer[counter], stringMax, targetFile) != NULL) {
	    counter++;
    }

    /* iterative solution of the optimization problem at hand */
    /* make room for our line comparison table */
    link **lineCmp = calloc(nbOriginalLines + 1, sizeof(*lineCmp));
    for(counter = 0; counter < nbOriginalLines; counter++) {
	    lineCmp[counter] = calloc(nbTargetLines + 1, sizeof(**lineCmp));
    }
    unsigned int i;
    unsigned int j;
    lineCmp[0][0].cost = 0;
    lineCmp[0][0].originalLine = 0;
    lineCmp[0][0].targetLine = 0;
    /* go through our target file */
    for(j = 1; j < nbTargetLines; j++) {
	    unsigned int minCostDEL = INFINITY - 100;
	    unsigned int lineDEL = j;
	    /* go through our original file */
	    for(i = 1; i < nbOriginalLines; i++) {
		    unsigned int minCost = INFINITY;
		    unsigned int lineIn = 0;
		    unsigned int lineOut = 0;
		    if(i > 1) {
			    /* check for MULTI_DEL */
			    if(minCostDEL > lineCmp[i-2][j].cost) {
				    minCostDEL = lineCmp[i-2][j].cost;
				    lineDEL = i - 2;
			    }
			    if(minCostDEL + 15 < minCost) {
				    minCost = minCostDEL;
				    lineIn = lineDEL;
				    lineOut = j;
			    }
		    }
		    /* check for ADD */
		    if(minCost > lineCmp[i][j-1].cost + 10 + strlen(targetBuffer[j])) {
			    minCost = lineCmp[i][j-1].cost + 10 + strlen(targetBuffer[j]);
			    lineIn = i;
			    lineOut = j - 1;
		    }
		    /* check for SUB */
		    if(minCost > lineCmp[i-1][j-1].cost + 10 + strlen(targetBuffer[j])) {
			    minCost = lineCmp[i-1][j-1].cost + 10 + strlen(targetBuffer[j]);
			    lineIn = i - 1;
			    lineOut = j - 1;
		    }
		    /* check for DEL */
		    if(minCost > lineCmp[i-1][j].cost + 10) {
			    minCost = lineCmp[i-1][j].cost + 10;
			    lineIn = i - 1;
			    lineOut = j;
		    }
		    /* check for COPY */
		    if(!strcmp(originalBuffer[i-1], targetBuffer[j-1]) && minCost > lineCmp[i-1][j-1].cost) {
			    minCost = lineCmp[i-1][j-1].cost;
			    lineIn = i - 1;
			    lineOut = j - 1;
		    }
		    lineCmp[i][j].cost = minCost;
		    lineCmp[i][j].originalLine = lineIn;
		    lineCmp[i][j].targetLine = lineOut;
	    }
    }
    /* display the created patch */
    i = 0;
    j = 0;
    //unsigned int k = 0;
    //unsigned int l = 0;
    while(i != nbOriginalLines || j != nbTargetLines) {
	    /* ADD */
	    if ((lineCmp[i][j].targetLine == j+1) && (lineCmp[i][j].originalLine == i)) { 
		    printf("+ %d\n", i);
		    printf("%s", targetBuffer[j+1]);
	    /* DEL */
	    } else if ((lineCmp[i][j].targetLine == j) && (lineCmp[i][j].originalLine == i+1)) { 
		    printf("d %d\n", (i+1));  
	    /* SUB */
	    } else if ((lineCmp[i][j].targetLine == j+1) && (lineCmp[i][j].originalLine == i+1) && (lineCmp[i][j].cost != lineCmp[i+1][j+1].cost)) { 
		    printf("= %d\n", i+1);
		    printf("%s", targetBuffer[j+1]);
	    /* COPY */
	    } else if ((lineCmp[i][j].targetLine == j+1) && (lineCmp[i][j].originalLine == i+1)) { 
	    /* MULTI_DEL */
	    } else { 
		    printf("D %d %d\n",  i+1, lineCmp[i][j].originalLine-i);
	    }
	    i = lineCmp[i][j].originalLine;
	    j = lineCmp[i][j].targetLine;
	    //i = k;
	    //j = l;
    }
    /* free all the reserved memory */
    free(originalBuffer);
    free(targetBuffer);
    //for(counter = 0; counter < nbOriginalLines; counter++) {
	//    free(lineCmp[counter]);
    //}
    free(lineCmp);
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
