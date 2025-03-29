// Ben Leber
// 3/28/25
// Consumer Producer Problem using shared memory and semaphores.
// Consumer program that reads from a shared buffer and prints the consumed items to the console.

#include <iostream>
#include <thread>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <cstdlib>

using namespace std;

struct SharedBuffer {
    // SharedBuffer contains a buffer with 2 items, and an int that tracks the amount of items in the buffer
    // additionally, there is a semaphore to control access to the buffer
    int buffer[2];
    int bufferAmount = 0;
};

int main() {

    // Similar to the producer, we need to use the same names for shared memory and semaphore
    const char* shm_name = "/shared_memory";
    const char* sem_name = "/semaphore";

    // Create and open shared memory using shm_open
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cout << "shm_open failed to open shared memory" << std::endl; 
        exit(1);
    }

    // Truncates shared memory to the size of SharedBuffer
    ftruncate(shm_fd, sizeof(SharedBuffer));

    // Mapping shared data
    SharedBuffer *shared_buffer = (SharedBuffer *)mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_buffer == MAP_FAILED) {
        std::cout << "mmap failed to map shared memory" << std::endl;
        exit(1);
    }
    
    // Open shared semaphore
    // We use 0 to indicate that we are opening an existing semaphore
    // The other difference is that there is no O_CREAT flag or 0664 permissions
    // This is because the semaphore was already made by the producer
    sem_t *semaphore = sem_open(sem_name, 0);
    if (semaphore == SEM_FAILED) {
        std::cout << "sem_open failed to open semaphore" << std::endl;
        exit(1);
    }

    // Consumption Loop
    // For this example the loop will run until itemsConsumed = 10

    int itemsConsumed = 0;
    const int maxItemsConsumed = 100;
    do {
        // Check if we have consumed 10 items
        // If we have, break
        if (itemsConsumed >= maxItemsConsumed) {
            break; 
        }
        // itemsConsumed < 10, continue consuming
        else {
            // Wait for the semaphore to be available
            sem_wait(semaphore);

            // Once available, the consumer can access the shared buffer
            // Check if there are items in the buffer to consume
            if (shared_buffer->bufferAmount > 0) {
                // If there are, we decrease the bufferAmount. 
                // The item is then taken and printed to the console
                // itemsConsumed is then incremented
                int index = shared_buffer->bufferAmount - 1;
                int item = shared_buffer->buffer[--shared_buffer->bufferAmount];
                std::cout << "Consumed item: " << item << " from index: [" << index << "]" << std::endl;
                itemsConsumed++;
            } 
            // If there are no items in the buffer, the consumer will wait until next time
            else {
                // std::cout << "Nothing to Consume!" << std::endl;
            }

            // Release the semaphore
            sem_post(semaphore);
        }
    } while (true); 
    std::cout << "Done Consuming." << std::endl;
}
