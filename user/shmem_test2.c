#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


// Placeholder function to get the size of a process
uint64 getprocsize() {
    // This should call a system call to get the size of the process
    // For the sake of this example, we assume it returns a fixed value
    // Implement the corresponding system call in the kernel to get actual process size
    return 4096 * 10; // Placeholder value
}

int main(void)
{
    int pid;
    char *shared_mem;
    uint64 size_before, size_after_map, size_after_unmap;

    // Fork the process
    pid = fork();

    if(pid < 0){
        printf("fork failed\n");
        exit(1);
    }

    if(pid == 0){
        // Child process
        size_before = getprocsize();

        // Assume the parent shares the memory at this virtual address
        sleep(1); 

        // Write to the shared memory
        shared_mem = (char *)0x800000; // Arbitrary address, should be same as parent
        strcpy(shared_mem, "Hello daddy");

        // Print the shared memory content
        printf("Child wrote: %s\n", shared_mem);

        size_after_map = getprocsize();
        printf("Child size before mapping: %ld\n", size_before);
        printf("Child size after mapping: %ld\n", size_after_map);

        // Unmap the shared memory
        if(unmap_shared_pages(getpid(), (uint64)shared_mem, 4096) != 0){
            printf("Failed to unmap shared memory\n");
            exit(1);
        }

        size_after_unmap = getprocsize();
        printf("Child size after unmapping: %ld\n", size_after_unmap);

        // Allocate memory with malloc
        char *new_mem = malloc(4096);
        if(new_mem == 0){
            printf("malloc failed\n");
            exit(1);
        }
        printf("Malloc succeeded after unmapping\n");

        exit(0);
    } else {
        // Parent process
        // Allocate memory in parent
        shared_mem = malloc(4096);

        // Share memory with child process
        uint64 child_shared_va = map_shared_pages(getpid(), pid, (uint64)shared_mem, 4096);
        if(child_shared_va == (uint64)-1){
            printf("Failed to map shared memory\n");
            exit(1);
        }

        // Wait for child to finish
        wait(0);

        // Print the shared memory content
        printf("Parent read: %s\n", shared_mem);
    }

    exit(0);
}