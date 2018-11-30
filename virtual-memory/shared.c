#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define MEM_SZ 1024

int main() {
 	char* parent_message = "hi son";
  	char* child_message = "sup mom";

	void *mem = mmap(NULL, MEM_SZ, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	if(mem == NULL) {
        printf("Failed to map memory: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
	}

    pid_t pid = fork();

    memcpy(mem, parent_message, sizeof(parent_message));

    while(1) {
        char* message = (char*) mem;
	    if (pid == 0 && strcmp(mem, parent_message) == 0) {
    	    printf("Child read: %s\n", message);
            memcpy(mem, child_message, sizeof(child_message));
	    } else if (strcmp(mem, child_message) == 0) {
        	printf("Parent read: %s\n", (char*) mem);
            memcpy(mem, parent_message, sizeof(parent_message));
 	    }
    }
}