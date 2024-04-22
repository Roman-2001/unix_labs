#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
	char *myfile = argv[1];
	char *lock_file;
	sprintf(lock_file, "%s.lck", myfile);
	int fdlck = -1;
	while (fdlck == -1) {
		fdlck = open(lock_file, O_CREAT | O_EXCL | O_RDWR, 0777);
		sleep(0.1);
	}
	int cur_pid = getpid();
//	printf("%d\n", cur_pid);
	write(fdlck, &cur_pid, sizeof(cur_pid));
	close(fdlck);
	int fdin = open(myfile, O_RDWR);
	if (fdin == -1) {
		remove(lock_file);
		printf("Can`t open file \"%s\"\n", myfile);
		return 1;
	}
	sleep(1);
	lseek(fdlck, 0, SEEK_SET);
	int read_pid;
	fdlck = open(lock_file, O_RDWR);
//	printf("%d\n", read_pid);
	close(fdin);
	if (fdlck == -1) {
                printf("File %s was deleted before finish program\n", lock_file);
                return 1;
        }
	read(fdlck, &read_pid, sizeof(read_pid));
	close(fdlck);
	if (read_pid != cur_pid) {
		printf("File %s was occupied other process\n", lock_file);
		return 1;
	}
	remove(lock_file);
	return 0;
}
