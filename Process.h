#include <random>
#include <iostream>
using namespace std;

class Process {
  private:
    int id, memReq;
    unsigned long long int burst, arrival, end;
  public:
    Process(mt19937::result_type seed, int counter);
    int getMem();
    int getID();
    unsigned long long int getBurst();
    unsigned long long int getArrival();
    unsigned long long int getEnd();
    void setArriv(unsigned long long int arrTime);
    void setEnd(unsigned long long int newEnd);
    string toString() const;
};
