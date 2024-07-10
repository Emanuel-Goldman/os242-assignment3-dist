#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
  
    int ppid = getpid();
    char *shared_mem = malloc(4096);

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
        printf("Child process\n");
        pid = getpid();
        printf("The shared_mem addres is: %p\n", &shared_mem);
       
        char * addr= map_shared_pages(pid, ppid, (uint64)&shared_mem, 4096);
        printf("Shared memory address: %p\n", &addr);
        printf("Shared memory content: %s\n", (char*)addr);
        
        // Print the shared memory content
        exit(0);

    } else {

        printf("Parent process\n");
        strcpy(shared_mem, "Hello child");
        printf("The shared_mem addres is %p\n", &shared_mem);
        wait(0);
        printf("End parent process\n");

        printf("Parent process\n");
    }

    exit(0);
}