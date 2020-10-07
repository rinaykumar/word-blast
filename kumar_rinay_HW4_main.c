/**************************************************************
* Class:  CSC-415-01 Fall 2020
* Name: Rinay Kumar
* Student ID: 913859133
* GitHub ID: rinaykumar
* Project: Assignment 4 – Word Blast
*
* File: kumar_rinay_HW4_main.c
*
* Description: This program takes a file from the command line
* and n number of threads, then splits the data into n chunks
* and creates n threads which process the data and find all 
* occurrences of words 6 or more characters long. 
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MAX_SIZE 2000 // Number of words to hold in shared array
#define WORD_SIZE 10  // Size of each word

// Very Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

int count = 0;     // To keep track of elements in array
int initFlag = 0;  // For array initialization
int leftOver = 0;  // For the remaining bytes to use with last pthread
int fd, chunkSize;
pthread_mutex_t lock;

// Data structure to conatin word and word frequency
typedef struct WordsAndCount {
    char * word;
    int count;
} WordsAndCount;

// Global array of structures for up to MAX_SIZE words
struct WordsAndCount wordArray[MAX_SIZE];

// Initialize array of structures
void initWordArray() {
    for (int i = 0; i < MAX_SIZE; i++) {
        wordArray[i].word = malloc(WORD_SIZE);
        wordArray[i].count = 0;
    }
    initFlag = 1;
}

// Thread function called by each thread to process file in chunks
void *wordFunc(void *ptr) {  
    int res;
    char * buffer;
    char * token;
    
    // Allocate buffer with chunkSize + leftOver
    buffer = malloc(chunkSize+leftOver);

    // Read into buffer
    read(fd, buffer, chunkSize+leftOver);

    // Tokenize with strtok_r and loop through buffer
    while (token = strtok_r(buffer, delim, &buffer)) {       
        // Check if token is 6+ characters
        if ((int)strlen(token) > 5) {   
            // Check if token already in wordArray, increase count if yes
            for (int i = 0; i < MAX_SIZE; i++) {
                res = strcasecmp(wordArray[i].word, token);
                if (res == 0) {
                    pthread_mutex_lock(&lock);   // Start critical section
                    wordArray[i].count++;
                    pthread_mutex_unlock(&lock); // End critical section
                    break;
                } 
            }
            // If token not in wordArray, add and increase count
            if (res != 0) {
                if (count < MAX_SIZE) {
                    pthread_mutex_lock(&lock);            // Start critical section
                    strcpy(wordArray[count].word, token);
                    wordArray[count].count++;
                    pthread_mutex_unlock(&lock);          // End critical section
                    count++;
                }
            }
        }
    }
}

int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, open file, divide by threads
    //             Allocate and Initialize and storage structures
    int ret1, ret2, fileSize;
    int numOfThreads = strtol(argv[2], NULL, 10);  // Number of threads from args
    
    // Initialize wordArray
    if (initFlag == 0) {
        initWordArray();
    }
    
    // Initialize mutex lock
    if (ret1 = pthread_mutex_init(&lock, NULL)) {
        printf("ERROR: Mutex init failed [%d]\n", ret1);
    }
    
    // Open file
    fd = open(argv[1], O_RDONLY);

    // Get file size with lseek
    fileSize = lseek(fd, 0, SEEK_END);

    // Set file position back to beginning
    lseek(fd, 0, SEEK_SET);

    // Chunk size is file size divided by number of threads
    chunkSize = fileSize / numOfThreads;

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish
    // Declare pthread
    pthread_t thread[numOfThreads];

    // Create threads in a loop to run in parallel
    for (int i = 0; i < numOfThreads; i++) {
        // For last thread, adjust leftOver with bytes remaining
        if (i == numOfThreads-1) {
            leftOver = fileSize % numOfThreads;
        }
        // Create thread with some error checking
        if (ret2 = pthread_create(&thread[i], NULL, wordFunc, (void*) &i)) {
            printf("ERROR: Thread creation failed [%d]\n", ret2);
            exit(EXIT_FAILURE);
        }
    }

    // Join threads after they run
    for (int i = 0; i < numOfThreads; i++) {
        pthread_join(thread[i], NULL);
    }

    // ***TO DO *** Process TOP 10 and display
    WordsAndCount temp;

    // Sort wordArray in decreasing order
    for (int i = 0; i < MAX_SIZE; i++) {
        for (int j = i+1; j < MAX_SIZE; j++) {
            if (wordArray[i].count < wordArray[j].count) {
                temp = wordArray[i];
                wordArray[i] = wordArray[j];
                wordArray[j] = temp;
            }
        }
    }

    // Output top 10 6+ character words
    printf("\n\n");
    printf("Word Frequency Count on %s with %d threads\n", argv[1], numOfThreads);
    printf("Printing top 10 words 6 characters or more.\n");

    for (int i = 0; i < 10; i++) {
        printf("Number %d is %s with a count of %d\n", i+1, wordArray[i].word, wordArray[i].count);
    }

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************

    // ***TO DO *** cleanup
    // Close file
    close(fd);

    // Destory mutex lock
    pthread_mutex_destroy(&lock);

    // Free wordArray buffers
    for (int i = 0; i < MAX_SIZE; i++) {
        free(wordArray[i].word);
    }

    return 0;
    }
