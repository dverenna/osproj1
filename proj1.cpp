#include <studio.h>
#include <time.h>
#include <ctime.h>
#include <stdlib.h>
#include <random.h>



void fillProcess(Process *p, result_type seed, int counter) {
    srand((unsigned) time(0));
    auto burst_rand = std::bind(std::uniform_int_distribution<unsigned long long int>(10000000,10000000000000),
    mt19937(seed));
    numRand =  (rand() % 8) + 1;
    p.ID = counter;
    p.memReq = numRand;
    p.burst = burst_rand;
    p.arrival = 0;
    counter = counter + 1;
}

struct Process {
  int ID;
  int memReq;
  unsigned long long int burst;
  unsigned long long int arrival;
}

