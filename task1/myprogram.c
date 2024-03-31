#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int is_zeroed(char *block, int size) {
	for (int i=0; i<size; i++) {
		if (block[i]!=0) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int fdin = 0;
	int fdout = 0;
	int block_size = 4096;
	char *block;

	
	if (argc == 1) { 
		printf("Not enough parameters!");
		return 0;
	}
	if (argc ==2) {
		fdout = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	}
	if (argc == 3) {
		fdin = open(argv[1], O_RDONLY);
		fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRUSR);
	}
	if (argc == 4) {	
		fdin = open(argv[1], O_RDONLY); 
                fdout = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRUSR);
		block_size = atoi(argv[3]);
		if (block_size < 0) {
			printf("Negative block size!");
			close(fdin);
			close(fdout);
			return 1;
		}
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
