#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h> 

int is_zeroed(char *block, int size) {
	for (int i=0; i<size; i++) {
		if (block[i]!=0) {
			return 0;
		}
	}
	return 1;
}

int print_help() {
	printf("This program make a sparse-file\n");
	printf("\"this_program_name\" [key(s)] [input_file] output_file\n\n");
	printf("When running a program, you can specify a different number of arguments:\n"\
		"If the program input is given one argument - the file name,\n"\
		"then stdin is read and written to the specified file.\n\n"\
		"If there are two arguments, \n"\
		"then the first file is read and written to the last one.\n\n"\
		"Also you can use keys:\n"\
		"-h - print this message\n"\
		"-b [n] - block size for read/write in bytes\n");
	return 0;
}

int main(int argc, char *argv[]) {
	int fdin = 0;
	int fdout = 0;
	int block_size = 4096;
	char *block;
	int option;
	int option_flag = 0;
	

	while ((option = getopt(argc, argv, "hb:")) != -1) {
        	switch (option) {
        		case 'h':
        			return print_help();
            		case 'b':
               		block_size = atoi(optarg);
               		if (block_size <= 0) {
               			printf("Block size must be positive integer!\n");
               			return 1;
               		}
               		option_flag = 2;
               		break;
               	case '?':
               		printf("You must use integer argument after '-b' key!\n");
               		return 1;
               	default:
               		printf("Unallowable key(s)!\n");
               		return 1;
        	}
    	}
		
	if (argc - option_flag < 2) { 
		printf("Not enough parameters!\n");
		return 0;
	}
	if (argc == 2) {
		fdout = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	}
	else {
		if (argc - option_flag > 2) {
			fdin = open(argv[optind], O_RDONLY);
			if (fdin == -1) {
				printf("Input file doesn't exist!\n");
				return 1;
			}
			fdout = open(argv[optind+1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRUSR);
		}
		else {
			fdout = open(argv[optind], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRUSR);
		}
	}

	if (argc - option_flag > 3) {
		printf("Too many parameters!\n");
		return 1;
	}
	block = malloc(block_size);
	while (block_size = read(fdin, block, block_size)) {
		if (is_zeroed(block, block_size)) {
			lseek(fdout, block_size, SEEK_CUR);
		}
		else {
			write(fdout, block, block_size);
		}
	}
	free(block);
	close(fdin);
	close(fdout);
	return 0;
}
