# ConsumerProducerOS
## Consumer/Producer programs for Operating Systems @ Kent State University

Ben Leber<br>
CS33211 - Operating Systems<br>
Dr. Guan<br>
March 28, 2025<br>

This is the submission for project 1, the Consumer-Producer Problem. For this submission, I used <code>sys/mman.h</code> for shared memory access, so the program is only usable in a linux environment.

## Compiling the Processes

There are two ways to go about compiling:

### Makefile:
```bash
make clean
make
```

### Manual
```bash
g++ consumer.cpp -pthread -lrt -o consumer
g++ producer.cpp -pthread -lrt -o producer
```

## Running the Processes

To run the processes at the same time, use:

```bash
./producer & ./consumer &
```

## Killing the Processes

Once the consumer is done, use <code>CTRL + C</code> to end the consumer program. The producer will still be running in the background, so in order to stop the process you need to use <code>ps</code> to list all processes running. Once you find "producer," use the PID from <code>ps</code> to run <code>kill INSERT_PID</code>. 

Additionally, you can run <code>make kill</code>, to kill both processes at once.