#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{

    // // int ppid = getpid();
    // char *shared_mem = malloc(4096);

    // // Fork the process
    // int pid = fork();
    // if (pid < 0)
    // {
    //     printf("fork failed\n");
    //     exit(1);
    // }

    // if (pid == 0)
    // {
    //     printf("Child process\n");
    //     int pid = getpid();
    //     printf("pid value: %d\n", pid);
    //     strcpy(shared_mem, "Hello daddy");
    //     sleep(7);
    //     printf("unmapping \n");
    //     pid = getpid();
    //     unmap_shared_pages(pid, (uint64)shared_mem, 4096);
    //     printf("affter unmapping \n");
    //     char *new_mem = malloc(4096);
    //     strcpy(new_mem, "Unmapped successfully\n");
    //     printf("new_mem: %s\n", new_mem);
    //     printf("child exiting\n");
    //     exit(0);
    // }
    // else
    // {
    //     sleep(2);
    //     printf("Parent process\n");
    //     int ppid = getpid();
    //     uint64 addr = map_shared_pages(pid, ppid, (uint64)shared_mem, 4096);
    //     printf("Shared memory content: %s\n", (char *)addr);
    //     wait(0);
    //     printf("Parent process is exiting\n");
    // }
    // exit(0);
    //-------------------------------------------------------------------------------------

    char *shared_mem = malloc(4096);
    // printf("Child process size before mapping: %d\n",   get_size());

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
        // printf("Child process size after mapping: %d\n", get_size());

        sleep(7); // Simulate some work
        printf("unmapping\n");
        unmap_shared_pages(pid, (uint64)shared_mem, 4096);
        // printf("Child process size after unmapping: %d\n", get_size());

        char *new_mem = malloc(4096);
        strcpy(new_mem, "Unmapped successfully\n");
        printf("new_mem: %s\n", new_mem);
        // printf("Child process size after malloc: %d\n", get_size());
        printf("child exiting\n");
        exit(0);
    }
    else
    { // Parent process
        sleep(2);
        printf("Parent process\n");
        int ppid = getpid();
        uint64 addr = map_shared_pages(pid, ppid, (uint64)shared_mem, 4096);
        printf("Shared memory content: %s\n", (char *)addr);
        wait(0);
        printf("Parent process is exiting\n");
    }
    exit(0);
}