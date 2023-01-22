#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// Global variables
pthread_barrier_t barrier;
pthread_mutex_t lock;

// Structres
typedef struct semaphore 
{
    pthread_mutex_t semLock;
    pthread_cond_t semCond;
    int semValue;
} sem_t;

struct thread
{
    char team;
};

// Semaphores
sem_t semA, semB, *semT, *sem;


// Functions
void SemValueChanger(sem_t *sem) {
    pthread_mutex_lock(&sem->semLock);
    sem->semValue++;
    pthread_mutex_unlock(&lock);
    if (sem->semValue > 0)
    {
        pthread_cond_wait(&sem->semCond, &sem->semLock);
    }
    pthread_mutex_unlock(&sem->semLock);
}

// Function threads
void* ThreadFunc(void * th) 
{  
    int captain = 0;
    struct thread * th2 = (struct thread*) th;
    pthread_mutex_lock(&lock); // Lock here
    printf("Thread ID: %ld, Team: %c, I am looking for a car\n", pthread_self(), th2->team);
    if(semT->semValue >= 1 && sem->semValue >= 2) // If there are 1 team A and 3 team B
    {
        captain = 1; // There will be a driver
        pthread_mutex_lock(&semT->semLock);
        semT->semValue--; // Put 1 A into car
        pthread_cond_signal(&semT->semCond);
        pthread_mutex_unlock(&semT->semLock);
        int count = 2;
        while(count != 0)
        {
            pthread_mutex_lock(&sem->semLock);
            sem->semValue--; // Put 3 Bs into car
            pthread_cond_signal(&sem->semCond); 
            pthread_mutex_unlock(&sem->semLock);
            count--;
        }
    }
    else if(semT->semValue >= 2 && sem->semValue >= 1) // If there are 3 team A and 1 team B
    {
        captain = 1; // There will be a driver
        pthread_mutex_lock(&sem->semLock);
        sem->semValue--; // Put 1 B into car
        pthread_cond_signal(&sem->semCond);
        pthread_mutex_unlock(&sem->semLock);
        int count = 2;
        while(count != 0)
        {
            pthread_mutex_lock(&semT->semLock);
            semT->semValue--; // Put 3 As into car
            pthread_cond_signal(&semT->semCond); 
            pthread_mutex_unlock(&semT->semLock);
            count--;
        }
    } 
    else if(semT->semValue >= 3) // If there are 4 team A
    {
		captain = 1; // There will be a driver
        int count = 3;
        while(count != 0)
        {
            pthread_mutex_lock(&semT->semLock);
            semT->semValue--; // Put 4 As into car
            pthread_cond_signal(&semT->semCond);
            pthread_mutex_unlock(&semT->semLock);
            count--;
        }
    }
    else if(sem->semValue >= 3) // If there are 4 team B
    {
		captain = 1; // There will be a driver
        int count = 3;
        while(count != 0)
        {
            pthread_mutex_lock(&sem->semLock);
            sem->semValue--; // Put 4 Bs into car
            pthread_cond_signal(&sem->semCond);
            pthread_mutex_unlock(&sem->semLock);
            count--;
        }
    }
    else // Increament teams
    {
        if (th2->team == 'A')
        {
		    SemValueChanger(semT);
	    } 
        else if (th2->team == 'B')
        {
		    SemValueChanger(sem);
	    }
    }
    printf("Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), th2->team);
    pthread_barrier_wait(&barrier);
    if(captain == 1) // If the car is formed, put fans into the car and mark the first fan who found the car first as driver
    {
        printf("Thread ID: %ld, Team: %c, I am the captain and driving the car\n", pthread_self(), th2->team);
		pthread_barrier_destroy(&barrier);
	    pthread_barrier_init(&barrier, NULL, 4);
	    pthread_mutex_unlock(&lock);
    }
}

// Main Function
int main(int argc, char* argv[])
{
    int numA = atoi(argv[1]);
    int numB = atoi(argv[2]);
    if (numA % 2 != 0 || numB % 2 != 0 || (numA + numB) % 4 != 0) // Conditions
    {
        printf("The main terminates\n");
    }
    else
    {
        pthread_mutex_init(&lock, NULL);
        pthread_t threadList[numA + numB];
        struct thread mainList[numA + numB];
        for(int k = 0; k < numA + numB; k++)
        {
            if(k < numA)
            {
                mainList[k].team = 'A';
            }
            else
            {   
                mainList[k].team = 'B';
            }
        }
        semA.semValue = 0;
        semB.semValue = 0;
        semT = &semA;
        sem = &semB;
        pthread_cond_init(&semA.semCond, NULL);
        pthread_mutex_init(&semA.semLock, NULL);
        pthread_cond_init(&semB.semCond, NULL);
        pthread_mutex_init(&semB.semLock, NULL);
        pthread_barrier_init(&barrier, NULL, 4);
        for (int k = 0; k < numA + numB; k++) // Create threads
        {
            pthread_create(&threadList[k], NULL, ThreadFunc, &mainList[k]);
        }
        for (int k = 0; k < numA + numB; k++) // Join them
        {
            pthread_join(threadList[k], NULL);
        }
        printf("The main terminates\n"); // Print at the end
    }
    return 0;
}