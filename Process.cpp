#include <stdio.h>
#include <time.h>
#include <ctime>
#include <stdlib.h>
#include <random>
#include <queue>
#include <iostream>
#include <sstream>
#include <functional>
#include "Process.h"

Process::Process(mt19937::result_type seed, int counter){
  srand((unsigned) seed);
  auto burst_rand = std::uniform_int_distribution<unsigned long long int>(100000,100000000000);
  mt19937 gen(seed);
  int numRand =  (rand() % 8) + 1;
  id = counter;
  memReq = numRand;
  burst = burst_rand(gen);
  arrival = 0;
}
int Process::getMem(){
  return memReq;
}
int Process::getID(){
  return id;
}
unsigned long long int Process::getBurst(){
  return burst;
}
unsigned long long int Process::getArrival(){
  return arrival;
}
unsigned long long int Process::getEnd(){
  return end;
}
void Process::setArriv(unsigned long long int arrTime){
  arrival = arrTime;
}
void Process::setEnd(unsigned long long int newEnd){
  end = newEnd;
}
string Process::toString() const{
  stringstream out;
  out << "Process ID: " << id  << " Service Time: " << burst << " Memory Required: " << memReq << " Wait Time: " << (end-arrival) - burst << " Total Turnaround Time: " << end - arrival;
  return out.str();
}
