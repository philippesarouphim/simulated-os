## Philippe Sarouphim Hochar #260885492
---------------------------------------
# Assignment 3

The code is based on my own code from Assignment 2. My code from Assignment 2 is based on my own code from assignment 1. My code from assignment 1 is based on the provided starter code.

# Assignment 2

The code is based on my own code from Assignment 1.

My code form assignment 1 is based on the starter code for assignment 1.

## Flow

- exec function:
    1. Create our PCBs (we pass in the script file name). The constructor will:
        1. Initialize PCB with default values.
        2. Generate new pid (see how below).
        3. Load the script file to memory and compute length.
        4. Return new PCB struct.
    2. Create a new scheduling queue (we pass in a scheduling policy). The constructor will:
        1. Initialize empty queue with default values.
        2. Depending on specified policy, change queue behavior by assigning different methods to the object.
        3. Return the new queue.
    3. Enqueue PCBs.
    4. Execute queue (clean up is automatically handled by the PCBs when their code has finished executing).

## Abstractions

1. PCB:
    - `constructor(char*)`: Load code into memory, and store length and locations in PCB.
    - `free_memory(this)`: Cleares process code from memory (called automatically when reaching end of code).
    - `execute_next()`: Executes next line of code.
    - `execute_next_n(int)`: Executes the next `n` lines of code.
    - `execute_until_end()`: Executes code until end of process.
2. Queue:
    - `constrcutor(Policy)`: Creates the new queue and changes behavior of methods based on selected policy.
    - `enqueue(struct pcb*)`: Enqueue a PCB. Location of PCB in the queue will depend on selected policy.
    - `dequeue()`: Dequeue head.
    - `execute()`: This will execute the processes. It will use the selected scheduling policy.

## Utils

1. Pid generation:
    - We have a key-value pair in memory with key `pid_counter`.
    - Each time we call `generate_pid()`, this pair will be retrieved from memory, the value will be incremented by 1, and then saved back to memory.
    - This newly incremented value is returned from the function.

2. Code storage in memory:
    - Each line of code is stored as a key-value pair in the shell memory.
    - The key has the following format: `process|{pid}|{#line}`.
    - Therefore, if we want to retrieve the 5th line of code of the process with pid 45, we get the value from memory with key `process|45|5`.