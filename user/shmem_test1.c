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
       
        uint64 addr = map_shared_pages(ppid, pid, (uint64)shared_mem, 4096);
        printf("Massege from my daddy: %s\n", (char*)addr);
        
        exit(0);

    } else {

        printf("Parent process\n");
        strcpy(shared_mem, "Hello child");
        wait(0);

     }
    free(shared_mem);
    exit(0);
}