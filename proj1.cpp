#include <stdio.h>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <random>
#include <queue>
#include <inttypes.h>
#include <iostream>
#include <unistd.h>
#include <tuple>
#include <sys/wait.h>
#include <vector>
#include "Process.h"

void fifo(int scenario);
void sjf(int scenario);

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
    printf("Scheduling Simulator (Enter -1 to quit)\n1. FIFO\n2. SJF\n");
    scanf("%d", &method);
  }
}

void fifo(int scenario){
  int p1Mem = 8, p2Mem = 8, p3Mem = 8, p4Mem = 8;
  long long int p1Speed = 3000000000, p2Speed = 3000000000, p3Speed = 3000000000, p4Speed = 3000000000;
  long long int p1Count = 0,p2Count = 0, p3Count = 0,  p4Count = 0;
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

  Process dummy(-1,-1);
  tuple<Process, long long int> p1 = make_tuple(dummy, -1);
  tuple<Process, long long int> p2 = make_tuple(dummy, -1);
  tuple<Process, long long int> p3 = make_tuple(dummy, -1);
  tuple<Process, long long int> p4 = make_tuple(dummy, -1);

  if (scenario != 4){
    srand(seed);
    for (int i = 0; i < 40; i++){
        Process temp = Process(rand(), i);
        readyQueue.push(temp);
    }
    if (scenario != 2) {
      p1 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
      readyQueue.pop();
      p2 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
      readyQueue.pop();
      p3 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
      readyQueue.pop();
      p4 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
      readyQueue.pop();
    }
  }

  int i;
  if (scenario == 4){
   i = 0;
  }

  while(procCount != 40){
    printf("1: %s\n", get<0>(p1).toString().c_str());
    printf("2: %s\n", get<0>(p2).toString().c_str());
    printf("3: %s\n", get<0>(p3).toString().c_str());
    printf("4: %s\n", get<0>(p4).toString().c_str());
    if (get<1>(p1) == -1 && !readyQueue.empty()){
      if (scenario != 2){
        p1 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
        readyQueue.pop();
      }
    }
    else if (get<1>(p1) != -1){
      p1Count += p1Speed;
      long long int remTime = get<1>(p1);
      Process temp = get<0>(p1);
      printf("1: %d\n", temp.getID());
      remTime -= p1Speed;
      if (remTime <= 0){
        temp.setEnd(p1Count-remTime); //Set the end to when it actually ended
        p1Count -= remTime; //Roll clock back to when it actually ended
        resultQueue.push(temp); //Add it to resultQueue
        p1 = make_tuple(dummy, -1); //Add dummy back
        procCount++; //Increment
      } else {
        p1 = make_tuple(temp, remTime);
      }
    }

    if (get<1>(p2) == -1 && !readyQueue.empty()){
      if (scenario != 2){
        p2 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
        readyQueue.pop();
      }
    }
    else if (get<1>(p2) != -1){
      p2Count += p2Speed;
      long long int remTime = get<1>(p2);
      Process temp = get<0>(p2);
      printf("2: %d\n", temp.getID());
      remTime -= p2Speed;
      if (remTime <= 0){
        temp.setEnd(p2Count-remTime); //Set the end to when it actually ended
        p2Count -= remTime; //Roll clock back to when it actually ended
        resultQueue.push(temp); //Add it to resultQueue
        p2 = make_tuple(dummy, -1); //Add dummy back
        procCount++; //Increment
      } else {
        p2 = make_tuple(temp, remTime);
      }
    }

    if (get<1>(p3) == -1 && !readyQueue.empty()){
      if (scenario != 2){
        p3 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
        readyQueue.pop();
      }
    }
    else if (get<1>(p3) != -1) {
      p3Count += p3Speed;
      long long int remTime = get<1>(p3);
      Process temp = get<0>(p3);
      printf("%d\n", temp.getID());
      remTime -= p3Speed;
      if (remTime <= 0){
        temp.setEnd(p3Count-remTime); //Set the end to when it actually ended
        p3Count -= remTime; //Roll clock back to when it actually ended
        resultQueue.push(temp); //Add it to resultQueue
        p3 = make_tuple(dummy, -1); //Add dummy back
        procCount++; //Increment
      } else {
        p3 = make_tuple(temp, remTime);
      }
    }

    if (get<1>(p4) == -1 && !readyQueue.empty()){
      if (scenario != 2){
        p4 = make_tuple(readyQueue.front(), readyQueue.front().getBurst());
        readyQueue.pop();
      }
    }
    else if (get<1>(p4) != -1){
      p4Count += p4Speed;
      long long int remTime = get<1>(p4);
      Process temp = get<0>(p4);
      printf("%d\n", temp.getID());
      remTime -= p4Speed;
      if (remTime <= 0){
        temp.setEnd(p4Count-remTime); //Set the end to when it actually ended
        p4Count -= remTime; //Roll clock back to when it actually ended
        resultQueue.push(temp); //Add it to resultQueue
        p4 = make_tuple(dummy, -1); //Add dummy back
        procCount++; //Increment
      } else {
        p4 = make_tuple(temp, remTime);
      }
    }
    if (scenario == 4){
      Process temp = Process(rand(), i);
      readyQueue.push(temp);
      i++;
      temp.setArriv(p1Count);
    }
  }

  while(!resultQueue.empty()){
    Process temp = resultQueue.front();
    printf("%s\n", temp.toString().c_str());
    resultQueue.pop();
  }
  printf("\n");
}
void sjf(int scenario){

}
