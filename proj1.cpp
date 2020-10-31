#include <studio.h>
#include <time.h>
#include <ctime.h>
#include <stdlib.h>
#include <random.h>



void fillProcess() {
  Process p;
  for(int i = 1; i <= 40; i++){
    srand((unsigned) time(0));
    mt19937::result_type seed = time(0);
    auto burst_rand = std::bind(std::uniform_int_distribution<unsigned long long int>(10000000,10000000000000),
    mt19937(seed));
    numRand =  (rand() % 8) + 1;
    p.ID = i;
    p.memReq = numRand;
    p.burst = burst_rand;
    p.arrival = 0;
  }
}

struct Process {
  int ID;
  int memReq;
  unsigned long long int burst;
  unsigned long long int arrival;
}
