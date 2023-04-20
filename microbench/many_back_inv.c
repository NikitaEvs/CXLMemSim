#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {
    int *shared_memory;
    pid_t pid;

    // Create shared memory region
    shared_memory = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Spawn child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    } else if (pid == 0) {
        // Child process
        while (1) {
            printf("Child process reading value: %d\n", *shared_memory);
            sleep(1);
        }
    } else {
        // Parent process
        int i = 0;
        while (1) {
            printf("Parent process writing value: %d\n", i);
            *shared_memory = i;
            i++;
            sleep(1);
        }
        wait(NULL);
    }

    // Cleanup
    munmap(shared_memory, sizeof(int));
    return 0;
}