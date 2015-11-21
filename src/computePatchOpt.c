
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

int computePatchOpt(FILE *originalFile, FILE *targetFile) {
    char buffer[];
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

	patchFile = fopen(argv[1] , "r" );
	originalFile = fopen(argv[2] , "r" );
	
	if (originalFile==NULL) {fprintf(stderr, "!!!!! Error opening originalFile !!!!! \n"); exit(EXIT_FAILURE);}
	if (targetFile==NULL) {fprintf (stderr, "!!!!! Error opening targetFile !!!!!\n"); exit(EXIT_FAILURE);}

        computePatchOpt(originalFile, targetFile);

	fclose(originalFile);
	fclose(targetFile);
	return 0;
}
