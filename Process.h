#include <random>
#include <iostream>
using namespace std;

class Process {
  private:
    int id, memReq;
    long long int burst, arrival, end;
  public:
    Process(mt19937::result_type seed, int counter);
    int getMem();
    int getID();
    long long int getBurst();
    long long int getArrival();
    long long int getEnd();
    void setArriv(long long int arrTime);
    void setEnd(long long int newEnd);
    string toString() const;
};
