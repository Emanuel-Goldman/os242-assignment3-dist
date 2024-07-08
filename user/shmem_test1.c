#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    // Allocate memory in parent
    char *shared_mem = malloc(4096);
    strcpy(shared_mem, "Hello child");

    // Fork the process
    int pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        // Child process
        // Wait for the parent to share the memory
        sleep(1); 

        // Print the shared memory content
        printf("Child received: %s\n", shared_mem);

        exit(0);
    } else {
        // Parent process
        // Share memory with child process
        uint64 child_shared_va = map_shared_pages(getpid(), pid, (uint64)shared_mem, 4096);
        if(child_shared_va == (uint64)-1){
            printf("Failed to map shared memory\n");
            exit(1);
        }

        // Wait for child to finish
        wait(0);

        printf("Parent process\n");
    }

    exit(0);
}