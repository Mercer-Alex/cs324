#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	int pid;
	int pipefd[2];

	printf("Starting program; process has pid %d\n", getpid());

	FILE *fileName = fopen("fork-output.txt", "w+");

	fprintf(fileName, "BEFORE FORK (%d)\n", fileno(fileName));
	fflush(fileName);

	if (pipe(pipefd) == -1) {
		perror("pipe failed");
		exit(1);
	}

	if ((pid = fork()) < 0) {
		fprintf(stderr, "Could not fork()");
		exit(1);
	}

	/* BEGIN SECTION A */

	printf("Section A;  pid %d\n", getpid());
	//sleep(5);

	/* END SECTION A */
	if (pid == 0) {
		/* BEGIN SECTION B */
		
		char *newenviron[] = { NULL };
		printf("Program \"%s\" has pid %d. Sleeping.\n", argv[0], getpid());
						
		if (argc <= 1) {
			printf("No program to exec.  Exiting...\n");
			exit(0);
		}
		
		printf("Running exec of \"%s\"\n", argv[1]);
		execve(argv[1], &argv[1], newenviron);
		printf("End of program \"%s\".\n", argv[0]);
		exit(0);

	} else {
		/* BEGIN SECTION C */
		fprintf(fileName, "SECTION C (%d)\n", fileno(fileName));
		close(pipefd[1]);
		char buffer[128];
		ssize_t bytesRead = read(pipefd[0], buffer, sizeof(buffer) -1);
		
		if (bytesRead > 0) {
		       	buffer[bytesRead] = '\0';
		       	printf("Read %ld bytes from pipe: %s", bytesRead, buffer);
	       	} else {
			printf("No data read from pipe\n");
		}
	
		close(pipefd[0]);
		printf("Section C\n");
		wait(NULL);
		//sleep(30);
		//printf("Section C done sleeping\n");
		fclose(fileName);
		exit(0);

		/* END SECTION C */
	}
	/* BEGIN SECTION D */

	printf("Section D\n");
	sleep(30);

	/* END SECTION D */
}


