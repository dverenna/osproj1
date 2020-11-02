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

// Structure used to compare Processes
struct shortestBurst {
  bool operator() (Process& lhs, Process& rhs){
    long long int lhsBurst = lhs.getBurst();
    long long int rhsBurst = rhs.getBurst();
    return lhsBurst > rhsBurst;
  }
};

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
  // Setting default values for memory, and speed based on Scenario 1, with if statements to change it
  // if it is scenario 2 or 3. The amount of clock cycles for each processor is then intialized
  int p1Mem = 8, p2Mem = 8, p3Mem = 8, p4Mem = 8;
  long long int p1Speed = 3000000000, p2Speed = 3000000000, p3Speed = 3000000000, p4Speed = 3000000000;
  long long int p1Count = 0,p2Count = 0, p3Count = 0,  p4Count = 0;
  bool p1Go = true, p2Go = true, p3Go = true, p4Go = true;
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

  // The ready queue and result queues are initialized and the amount of completed processes is initialized
  queue<Process> readyQueue;
  queue<Process> resultQueue;
  int procCount = 0;

  mt19937::result_type seed;
  printf("Enter the desired seed for the RNG\n");
  cin >> seed;

  // The tuples containing the Process object and the remaining time are initialized with a dummy that will
  // act as "Null" and signify when a processor is available for a new process
  Process dummy(-1,-1);
  tuple<Process, long long int> p1 = make_tuple(dummy, -1);
  tuple<Process, long long int> p2 = make_tuple(dummy, -1);
  tuple<Process, long long int> p3 = make_tuple(dummy, -1);
  tuple<Process, long long int> p4 = make_tuple(dummy, -1);

  // If the scenario is not 4, then the 40 processes are generated and put into the queue,
  // if it is scenario 4, then the counter of how many processes have been generated is set to 0;
  int i;
  if (scenario != 4){
    srand(seed);
    for (int j = 0; j < 40; j++){
        Process temp = Process(rand(), j);
        readyQueue.push(temp);
    }
  } else {
    i = 0;
  }

  // While the 40 processes have not been completed each of the processors performs the following actions
  //  -Checks if the processor is currently set to "dummy" and thus waiting for a process to be put on.
  //   If it is set to dummy, it appropriately moves a process off the queue into the processor (from the front
  //   if the scenario is not 2, and the first process that is below the memory threshold if it is scenario 2)
  //  -Incremements the cycle counter by the processors speed, gets the current remaining time and the process object
  //   for the process that is currently on the CPU, and decrements the remaining time by the clock speed of the CPU.
  //   - If the process is not completed at that point, the tuple is updated with the new remaining time.
  //   - If the process is completed  at that point, the end time is then set to the cycle where the process actually
  //     ended, and the clock is rolled back to the final cycle of the process. The process is then put into the result
  //     queue, and the processor is set to having a dummy process so it knows it is ready for a new process.
  // Each iteration in scenario 4, a new process enters the ready queue until 40 processes are generated
  while(procCount != 40){
    if (get<1>(p1) == -1 && !readyQueue.empty()){
      Process temp = readyQueue.front();
      if (scenario != 2){
        p1 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p1Mem && !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.front();
        }
        if (!readyQueue.empty()){
          p1 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p1Go = false;
        while (!readyQueue.empty()) {
          storage.push(readyQueue.front());
          readyQueue.pop();
        }
        readyQueue.swap(storage);
      }
    }
    else if (get<1>(p1) != -1){
      p1Count += p1Speed;
      long long int remTime = get<1>(p1);
      Process temp = get<0>(p1);
      remTime -= p1Speed;
      if (remTime <= 0){
        temp.setEnd(p1Count-remTime); // Set the end to when it actually ended
        p1Count -= remTime; // Roll clock back to when it actually ended
        resultQueue.push(temp); // Add it to resultQueue
        p1 = make_tuple(dummy, -1); // Add dummy back
        procCount++; // Increment
      } else {
        p1 = make_tuple(temp, remTime);
      }
    }

    if (get<1>(p2) == -1 && !readyQueue.empty()){
      Process temp = readyQueue.front();
      if (scenario != 2){
        p2 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p2Mem && !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.front();
        }
        if (!readyQueue.empty()){
          p2 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p2Go = false;
        while (!readyQueue.empty()) {
          storage.push(readyQueue.front());
          readyQueue.pop();
        }
        readyQueue.swap(storage);
      }
    }
    else if (get<1>(p2) != -1){
      p2Count += p2Speed;
      long long int remTime = get<1>(p2);
      Process temp = get<0>(p2);
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
      Process temp = readyQueue.front();
      if (scenario != 2){
        p3 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p3Mem && !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.front();
        }
        if (!readyQueue.empty()){
          p3 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p3Go = false;
        while (!readyQueue.empty()) {
          storage.push(readyQueue.front());
          readyQueue.pop();
        }
        readyQueue.swap(storage);
      }
    }
    else if (get<1>(p3) != -1) {
      p3Count += p3Speed;
      long long int remTime = get<1>(p3);
      Process temp = get<0>(p3);
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
      Process temp = readyQueue.front();
      if (scenario != 2){
        p4 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p4Mem && !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.front();
        }
        if (!readyQueue.empty()){
          p4 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p4Go = false;
        while (!readyQueue.empty()) {
          storage.push(readyQueue.front());
          readyQueue.pop();
        }
        readyQueue.swap(storage);
      }
    }
    else if (get<1>(p4) != -1){
      p4Count += p4Speed;
      long long int remTime = get<1>(p4);
      Process temp = get<0>(p4);
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
    if (scenario == 4 && i <= 40){
      Process temp = Process(rand(), i);
      readyQueue.push(temp);
      i++;
      temp.setArriv(p1Count);
    }
  }

  // Once all 40 processes have been completed, the result queue is then emptied and it's contents are printed
  // in the order they complted
  while(!resultQueue.empty()){
    Process temp = resultQueue.front();
    printf("%s\n", temp.toString().c_str());
    resultQueue.pop();
  }
  printf("\n");
}

void sjf(int scenario){
  // Setting default values for memory, and speed based on Scenario 1, with if statements to change it
  // if it is scenario 2 or 3. The amount of clock cycles for each processor is then intialized
  int p1Mem = 8, p2Mem = 8, p3Mem = 8, p4Mem = 8;
  long long int p1Speed = 3000000000, p2Speed = 3000000000, p3Speed = 3000000000, p4Speed = 3000000000;
  long long int p1Count = 0,p2Count = 0, p3Count = 0,  p4Count = 0;
  bool p1Go = true, p2Go = true, p3Go = true, p4Go = true;
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

  // The ready queue and the result queue are created similarly to FIFO with one major difference.
  // In SJF the ready queue is a priorty queue that sorts on the shortest burst time, this allows
  // for the queue to automatically sort the shortest processes to the beginning so the shortest jobs
  // are obtained first
  priority_queue<Process, vector<Process>, shortestBurst> readyQueue;
  queue<Process> resultQueue;
  int procCount = 0;

  mt19937::result_type seed;
  printf("Enter the desired seed for the RNG\n");
  cin >> seed;

  // The tuples containing the Process object and the remaining time are initialized with a dummy that will
  // act as "Null" and signify when a processor is available for a new process
  Process dummy(-1,-1);
  tuple<Process, long long int> p1 = make_tuple(dummy, -1);
  tuple<Process, long long int> p2 = make_tuple(dummy, -1);
  tuple<Process, long long int> p3 = make_tuple(dummy, -1);
  tuple<Process, long long int> p4 = make_tuple(dummy, -1);

  // If the scenario is not 4, then the 40 processes are generated and put into the queue,
  // if it is scenario 4, then the counter of how many processes have been generated is set to 0;
  int i;
  if (scenario != 4){
    srand(seed);
    for (int j = 0; j < 40; j++){
        Process temp = Process(rand(), j);
        readyQueue.push(temp);
    }
  } else {
   i = 0;
  }

  // While the 40 processes have not been completed each of the processors performs the following actions
  //  -Checks if the processor is currently set to "dummy" and thus waiting for a process to be put on.
  //   If it is set to dummy, it appropriately moves a process off the queue into the processor (from the front
  //   if the scenario is not 2, and the first process that is below the memory threshold if it is scenario 2)
  //  -Incremements the cycle counter by the processors speed, gets the current remaining time and the process object
  //   for the process that is currently on the CPU, and decrements the remaining time by the clock speed of the CPU.
  //   - If the process is not completed at that point, the tuple is updated with the new remaining time.
  //   - If the process is completed  at that point, the end time is then set to the cycle where the process actually
  //     ended, and the clock is rolled back to the final cycle of the process. The process is then put into the result
  //     queue, and the processor is set to having a dummy process so it knows it is ready for a new process.
  // Each iteration in scenario 4, a new process enters the ready queue until 40 processes are generated
  while(procCount != 40){
    if (get<1>(p1) == -1 && !readyQueue.empty() && p1Go){
      Process temp = readyQueue.top();
      if (scenario != 2){
        p1 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p1Mem && !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.top();
        }
        if (!readyQueue.empty()){
          p1 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p1Go = false;
        while (!storage.empty()) {
          readyQueue.push(storage.front());
          storage.pop();
        }
      }
    }
    else if (get<1>(p1) != -1 && p1Go){
      p1Count += p1Speed;
      long long int remTime = get<1>(p1);
      Process temp = get<0>(p1);
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

    if (get<1>(p2) == -1 && !readyQueue.empty() && p2Go){
      Process temp = readyQueue.top();
      if (scenario != 2){
        p2 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p2Mem&& !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.top();
        }
        if (!readyQueue.empty()){
          p2 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p2Go = false;
        while (!storage.empty()) {
          readyQueue.push(storage.front());
          storage.pop();
        }
      }
    }
    else if (get<1>(p2) != -1 && p2Go){
      p2Count += p2Speed;
      long long int remTime = get<1>(p2);
      Process temp = get<0>(p2);
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

    if (get<1>(p3) == -1 && !readyQueue.empty() && p3Go){
      Process temp = readyQueue.top();
      if (scenario != 2){
        p3 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p3Mem&& !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.top();
        }
        if (!readyQueue.empty()){
          p3 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p3Go = false;
        while (!storage.empty()) {
          readyQueue.push(storage.front());
          storage.pop();
        }
      }
    }
    else if (get<1>(p3) != -1 && p3Go) {
      p3Count += p3Speed;
      long long int remTime = get<1>(p3);
      Process temp = get<0>(p3);
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

    if (get<1>(p4) == -1 && !readyQueue.empty() && p4Go){
      Process temp = readyQueue.top();
      if (scenario != 2){
        p4 = make_tuple(temp, temp.getBurst());
        readyQueue.pop();
      } else {
        queue<Process> storage;
        while (temp.getMem() > p4Mem&& !readyQueue.empty()){
          storage.push(temp);
          readyQueue.pop();
          temp = readyQueue.top();
        }
        if (!readyQueue.empty()){
          p4 = make_tuple(temp, temp.getBurst());
          readyQueue.pop();
        }
        else
          p4Go = false;
        while (!storage.empty()) {
          readyQueue.push(storage.front());
          storage.pop();
        }
      }
    }
    else if (get<1>(p4) != -1 && p4Go){
      p4Count += p4Speed;
      long long int remTime = get<1>(p4);
      Process temp = get<0>(p4);
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
    if (scenario == 4 && i <= 40){
      Process temp = Process(rand(), i);
      readyQueue.push(temp);
      i++;
      temp.setArriv(p1Count);
    }
  }

  // Once all 40 processes have been completed, the result queue is then emptied and it's contents are printed
  // in the order they complted
  while(!resultQueue.empty()){
    Process temp = resultQueue.front();
    printf("%s\n", temp.toString().c_str());
    resultQueue.pop();
  }
  printf("\n");
}
