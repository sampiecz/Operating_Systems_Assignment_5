/***********************************************************
CSCI 480 - Assignment 5 - Fall 2019

Progammer: Sam Piecz
Z-ID: Z1732715
Section: 2 
TA: Jingwan Li  
Date Due: Oct 25, 2019 Purpose: Priority Scheduling Simulation. 
************************************************************/
#include "Widget.h"
#include <iostream>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
using std::cout;
using std::endl;
using std::queue;

#define P_NUMBER 7
#define C_NUMBER 5
#define P_STEPS 5
#define C_STEPS 7

// Global Vars
pthread_t consumers[C_NUMBER];
pthread_t producers[P_NUMBER];
queue<Widget*> buffer;
sem_t NOT_EMPTY;
sem_t NOT_FULL; 
pthread_mutex_t mutex;
int counter = 0;

// Prototypes
void Insert(int);
void Remove(int);
void * Produce(void * producerNum);
void * Consume(void * consumerNum);
void printBuffer();
void printStatus(bool, int);

int main()
{
	cout << "Simulation of Producer and Consumers" << endl;

  // Init sems & mutex
  sem_init(&NOT_EMPTY, 0, 35);
  sem_init(&NOT_FULL, 0, 0);
  pthread_mutex_init(&mutex, NULL);
	cout << "The semaphores and mutex have been initialized." << endl;

  // Iterate and create producers
  for (int i = 0; i < P_NUMBER - 1; i++)
  {
    pthread_create(&producers[i], NULL, Produce, (void*) i);
  }

  // Iterate and create consumers
  for (int i = 0; i < C_NUMBER - 1; i++)
  {
    pthread_create(&consumers[i], NULL, Consume, (void*) i);
  }

  // Terminate producer threads
  for (int i = 0; i < P_NUMBER - 1; i++)
  {
    pthread_join(producers[i], NULL);
  }

  // Terminate consumer threads
  for (int i = 0; i < C_NUMBER - 1; i++)
  {
    pthread_join(consumers[i], NULL);
  }

	cout << "All the producer and consumer threads have been closed." << endl;

  // Kill all semaphores and mutexes
  sem_destroy(&NOT_EMPTY);
  sem_destroy(&NOT_FULL);
  pthread_mutex_destroy(&mutex);
  pthread_exit(NULL);

	cout << "The semaphores and mutexes have been deleted." << endl;

  return 0;
}


// Function Definitions
void Insert(int threadID, int widgetNum)
{
  if (buffer.size() < 35)
  {
    pthread_mutex_lock(&mutex);
    Widget* theWidget = new Widget(threadID, widgetNum);
    buffer.push(theWidget);
    counter++;
    printStatus(true, threadID);
    pthread_mutex_unlock(&mutex);
  }
}

void Remove(int threadID)
{
  if (!buffer.empty())
  {
    pthread_mutex_lock(&mutex);
    buffer.pop();
    counter--;
    printStatus(false, threadID);
    pthread_mutex_unlock(&mutex);
  }
}

void * Produce(void * producerNum)
{
  long int threadID = (long int) producerNum;
  for (int i = 0; i < P_STEPS - 1; i++)
  {
    sem_wait(&NOT_FULL);
    Insert(threadID, i);
    sem_post(&NOT_EMPTY);
    sleep(1);
  }
  pthread_exit(NULL);
}

void * Consume(void * consumerNum)
{
  long int threadID = (long int) consumerNum;
  for (int i = 0; i < C_STEPS - 1; i++)
  {
    sem_wait(&NOT_EMPTY);
    Remove(threadID);
    sem_post(&NOT_FULL);
    sleep(1);
  }
  pthread_exit(NULL);
}

void printBuffer()
{
  queue<Widget*> printBuffer(buffer);
  while (!printBuffer.empty())
  {
    printBuffer.front()->print();
    printBuffer.pop();
  }
}

void printStatus(bool whichOne, int num)
{
  if (whichOne)
  {
    cout << "Producer " << num << " inserted one item. Total is now " << counter << "." << endl; 
    cout << "Buffer now contains :";
    printBuffer();
		cout << "\n" << endl;
  }
  else
  {
    cout << "Consumer " << num << " removed one item. Total is now " << counter << "." << endl; 
    cout << "Buffer now contains :";
    printBuffer();
		cout << "\n" << endl;
  }

}
