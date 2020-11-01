#include <stdio.h>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <random>
#include <queue>
#include <inttypes.h>
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.h"

void fifo(int scenario);
void sjf(int scenario);
pthread_mutex_t rqPull, counter, resPush;

int main(){
  int method = 0;
  printf("Scheduling Simulator (Enter -1 to quit)\n1. FIFO\n2. SJF\n");
  scanf("%d", &method);
  while (method != -1){
    int scenario = 0;
    if (method == 1){
      //FIFO
      printf("Scenario Selection (Enter -1 to return to method select)\n1. Scenario 1: Identical Speed and Memory\n2. Scenario 2: Identical Speed and Varying Memory\n3. Scenario 3: Varying Speed and Identical Memory\n4. Scenario 4: Identical Speed and Memory, Sequential Arrival\n");
      scanf("%d", &scenario);
      while (scenario != -1){
        if (scenario < 1 || scenario > 5) {
          printf("Invalid Choice\n");
        }
        else {
          fifo(scenario);
        }
        printf("Scenario Selection (Enter -1 to return to method select)\n1. Scenario 1: Identical Speed and Memory\n2. Scenario 2: Identical Speed and Varying Memory\n3. Scenario 3: Varying Speed and Identical Memory\n4. Scenario 4: Identical Speed and Memory, Sequential Arrival\n");
        scanf("%d", &scenario);
      }
    }
    else if (method == 2){
      //SJF
      printf("Scenario Selection (Enter -1 to return to method select)\n1. Scenario 1: Identical Speed and Memory\n2. Scenario 2: Identical Speed and Varying Memory\n3. Scenario 3: Varying Speed and Identical Memory\n4. Scenario 4: Identical Speed and Memory, Sequential Arrival\n");
      scanf("%d", &scenario);
      while (scenario != -1){
        if (scenario < 1 || scenario > 5) {
          printf("Invalid Choice\n");
        }
        else {
          sjf(scenario);
        }

        printf("Scenario Selection (Enter -1 to return to method select)\n1. Scenario 1: Identical Speed and Memory\n2. Scenario 2: Identical Speed and Varying Memory\n3. Scenario 3: Varying Speed and Identical Memory\n4. Scenario 4: Identical Speed and Memory, Sequential Arrival\n");
        scanf("%d", &scenario);
      }
    }
    else {
      printf("Invalid Choice\n");
    }
    printf("Scenario Selection (Enter -1 to return to method select)\n1. Scenario 1: Identical Speed and Memory\n2. Scenario 2: Identical Speed and Varying Memory\n3. Scenario 3: Varying Speed and Identical Memory\n4. Scenario 4: Identical Speed and Memory, Sequential Arrival\n");
    scanf("%d", &method);
  }
}

void fifo(int scenario){
  int p1Mem = 8, p2Mem = 8, p3Mem = 8, p4Mem = 8;
  unsigned long long int p1Speed = 3000000000, p2Speed = 3000000000, p3Speed = 3000000000, p4Speed = 3000000000;
  unsigned long long int p1Count = 0,p2Count = 0, p3Count = 0,  p4Count = 0;
  if (scenario == 2){
    p1Mem = 2;
    p2Mem = 2;
    p3Mem = 4;
    p4Mem = 8;
  }
  else if (scenario == 3){
    p1Speed = 2000000000;
    p2Speed = 2000000000;
    p3Speed = 3000000000;
    p4Speed = 4000000000;
  }

  queue<Process> readyQueue;
  queue<Process> resultQueue;
  int procCount = 0;

  mt19937::result_type seed;
  printf("Enter the desired seed for the RNG\n");
  cin >> seed;

  if (scenario != 4){
    srand(seed);
    for (int i = 0; i < 40; i++){
        Process temp = Process(rand(), i);
        printf("%s\n", temp.toString().c_str());
        readyQueue.push(temp);
    }
  } else if (scenario == 4){
    //Set up the Sequential Arrival
  }

  int p1 = fork();
  if (p1 != 0){
    int stat1;
    waitpid(p1, &stat1, 0);
    //Print out the result queue
    printf("Done\n");
  } else {
    int p2 = fork();
    if (p2 != 0){
      while (procCount != 40){
        // Grab an item
        pthread_mutex_lock(&rqPull);
        Process temp = readyQueue.front();
        if (temp.getMem() > p1Mem){
          queue<Process> storage;
          while (temp.getMem() > p1Mem){
            storage.push(temp);
            temp = readyQueue.front();
            readyQueue.pop();
          }
          while (!readyQueue.empty()){
            storage.push(readyQueue.front());
            readyQueue.pop();
          }
          readyQueue.swap(storage);
        } else {
          temp = readyQueue.front();
          readyQueue.pop();
        }
        pthread_mutex_unlock(&rqPull);
        //Do calculations and wait
        printf("P1 Working on %d\n", temp.getID());
        unsigned long long int procTime = temp.getBurst() / p1Speed;
        printf("%lld\n", procTime);
        sleep(procTime);
        p1Count += temp.getBurst();
        temp.setEnd(p1Count);
        //Increase Counter
        pthread_mutex_lock(&counter);
        procCount++;
        pthread_mutex_unlock(&counter);
        //Put into result queue
        pthread_mutex_lock(&resPush);;
        resultQueue.push(temp);
        pthread_mutex_unlock(&resPush);;
      }
      int stat2;
      waitpid(p2, &stat2, 0);
      exit(0);
    } else {
      int p3 = fork();
      if (p3 != 0) {
        while (procCount != 40){
          // Grab an item
          pthread_mutex_lock(&rqPull);;
          Process temp = readyQueue.front();
          if (temp.getMem() > p2Mem){
            queue<Process> storage;
            while (temp.getMem() > p2Mem){
              storage.push(temp);
              temp = readyQueue.front();
              readyQueue.pop();
            }
            while (!readyQueue.empty()){
              storage.push(readyQueue.front());
              readyQueue.pop();
            }
            readyQueue.swap(storage);
          } else {
            temp = readyQueue.front();
            readyQueue.pop();
          }
          pthread_mutex_unlock(&rqPull);;
          //Do calculations and wait
          printf("P2 Working on %d\n", temp.getID());
          unsigned long long int procTime = temp.getBurst() / p2Speed;
          printf("%lld\n", procTime);
          sleep(procTime);
          p2Count += temp.getBurst();
          temp.setEnd(p2Count);
          //Increase Counter
          pthread_mutex_lock(&counter);;
          procCount++;
          pthread_mutex_unlock(&counter);;
          //Put into result queue
          pthread_mutex_lock(&resPush);;
          resultQueue.push(temp);
          pthread_mutex_unlock(&resPush);;
        }
        int stat3;
        waitpid(p3, &stat3, 0);
        exit(0);
      } else {
        int p4 = fork();
        if (p4 != 0) {
        while (procCount != 40){
          // Grab an item
          pthread_mutex_lock(&rqPull);;
          Process temp = readyQueue.front();
          if (temp.getMem() > p3Mem){
            queue<Process> storage;
            while (temp.getMem() > p3Mem){
              storage.push(temp);
              temp = readyQueue.front();
              readyQueue.pop();
            }
            while (!readyQueue.empty()){
              storage.push(readyQueue.front());
              readyQueue.pop();
            }
            readyQueue.swap(storage);
          } else {
            temp = readyQueue.front();
            readyQueue.pop();
          }
          pthread_mutex_unlock(&rqPull);;
          //Do calculations and wait
          printf("P3 Working on %d\n", temp.getID());
          unsigned long long int procTime = temp.getBurst() / p3Speed;
          printf("%lld\n", procTime);
          sleep(procTime);
          p3Count += temp.getBurst();
          temp.setEnd(p3Count);
          //Increase Counter
          pthread_mutex_lock(&counter);;
          procCount++;
          pthread_mutex_unlock(&counter);;
          //Put into result queue
          pthread_mutex_lock(&resPush);;
          resultQueue.push(temp);
          pthread_mutex_unlock(&resPush);;
        }
        int stat4;
        waitpid(p4, &stat4, 0);
        exit(0);
      } else {
        while (procCount != 40){
          // Grab an item
          pthread_mutex_lock(&rqPull);;
          Process temp = readyQueue.front();
          if (temp.getMem() > p4Mem){
            queue<Process> storage;
            while (temp.getMem() > p4Mem){
              storage.push(temp);
              temp = readyQueue.front();
              readyQueue.pop();
            }
            while (!readyQueue.empty()){
              storage.push(readyQueue.front());
              readyQueue.pop();
            }
            readyQueue.swap(storage);
          } else {
            temp = readyQueue.front();
            readyQueue.pop();
          }
          pthread_mutex_unlock(&rqPull);;
          //Do calculations and wait
          printf("P4 Working on %d\n", temp.getID());
          unsigned long long int procTime = temp.getBurst() / p4Speed;
          printf("%lld\n", procTime);
          sleep(procTime);
          p4Count += temp.getBurst();
          temp.setEnd(p4Count);
          //Increase Counter
          pthread_mutex_lock(&counter);;
          procCount++;
          pthread_mutex_unlock(&counter);;
          //Put into result queue
          pthread_mutex_lock(&resPush);;
          resultQueue.push(temp);
          pthread_mutex_unlock(&resPush);;
        }
        exit(0);
      }
    }
  }
  }
}
void sjf(int scenario){

}
