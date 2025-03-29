// Ben Leber
// 3/28/25
// Consumer Producer Problem using shared memory and semaphores.
// Producer program that writes to a shared buffer and prints the produced items to the console.

#include <iostream>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <cstdlib>

struct SharedBuffer {
    // SharedBuffer contains a buffer with 2 items, and an int that tracks the amount of items in the buffer
    // additionally, there is a semaphore to control access to the buffer
    int buffer[2];
    int bufferAmount = 0;
};

int main() {

    // Names for shared memory and semaphore
    const char* shm_name = "/shared_memory";
    const char* sem_name = "/semaphore";

    // Creates and opens shared memory using shm_open
    // O_CREAT creates the shared memory if it does not exist
    // O_RDWR allows for read and write access
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    // Fail case
    if (shm_fd == -1) {
        std::cout << "shm_open failed to open shared memory" << std::endl; 
        exit(1);
    }

    // Truncate the shared memory to the size of SharedBuffer
    // This ensures that the shared memory block can store the 2 items and the bufferAmount variable
    ftruncate(shm_fd, sizeof(SharedBuffer));

    // Map shared data for use by program
    SharedBuffer *shared_buffer = (SharedBuffer *)mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    // Fail case
    if (shared_buffer == MAP_FAILED) {
        std::cout << "mmap failed to map shared memory" << std::endl;
        exit(1);
    }

    // Creates a semaphore with the given name "/semaphore"
    // The 1 states that the semaphore is initialized with 1, meaning only one thread can access the critical section at a time (in this case this is the buffer)
    sem_t *semaphore = sem_open(sem_name, O_CREAT, 0644, 1);
    // Fail case
    if (semaphore == SEM_FAILED) {
        std::cout << "sem_open failed to open semaphore" << std::endl;
        exit(1);
    }

    // Production Loop
    do {
        // Produces a random integer between 0 and 99 as the item going into the buffer
        int item = rand() % 100;

        // Wait for the semaphore to be available
        sem_wait(semaphore);

        // Checks for space in the buffer
        if (shared_buffer->bufferAmount < 2) {
            // Adds the item to the buffer
            // index is calculated by the current amount of items in the buffer
            // for example, if bufferAmount = 0, we add it to index 0
            // since there are only 2 spaces, if bufferAmount = 1, we add it to index 1
            shared_buffer->buffer[shared_buffer->bufferAmount] = item;

            // Additionally, we can take the index it is inserted into and use that for the console output
            int index = shared_buffer->bufferAmount;

            // Increments the buffer amount after adding the item
            shared_buffer->bufferAmount++;
            std::cout << "Produced item: " << item << " into index: [" << index << "]" << std::endl;
        }
        // If no space, release and wait
        else {
            // std::cout << "No Buffer Space!" << std::endl;
        }

        // Since its better for the producer to produce more items for the consumer, the wait is shorter.
        sem_post(semaphore);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    }
    while (true);
}
