#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    char *shared_mem = malloc(4096);

    // Fork the process
    int pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0)
    { // Child process
        printf("Child process\n");
        int pid = getpid();
        strcpy(shared_mem, "Hello daddy");
        sleep(7); // Simulate some work
        printf("Before unmap Memsize: %d\n", memsize());
        printf("unmapping\n");
        unmap_shared_pages(pid, (uint64)shared_mem, 4096);
        printf("Affter unmap before malloc Memsize: %d\n", memsize());
        char *new_mem = malloc(409600);
        strcpy(new_mem, "Unmapped successfully \n");
        printf("Affter malloc Memsize: %d\n", memsize());
        printf("new_mem: %s\n", new_mem);
        free(new_mem);
        exit(0);
    }
    else
    { // Parent process
        sleep(2);
        printf("Parent process\n");
        int ppid = getpid();
        printf("memory size before map: %d\n", memsize());
        uint64 addr = map_shared_pages(pid, ppid, (uint64)shared_mem, 4096);
        printf("memory size after map: %d\n", memsize());
        printf("Massege from my child: %s\n", (char *)addr);
        wait(0);
    }
    free(shared_mem);
    exit(0);
}