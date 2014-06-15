// To model a printing system
#include <iostream>
#include <queue>
#include <list>
#include <cstdlib>
#include <ctime>
using namespace std;

class Student {
  int serviceTime;
public:
  Student() : serviceTime(0) {}
  Student(int tm) : serviceTime(tm) {}
  int getTime() { return serviceTime; }
  void setTime(int newtime) {
    serviceTime = newtime;
  }
  friend ostream& 
  operator<<(ostream& os, const Student& c) {
    return os << '[' << c.serviceTime << ']';
  }
};

class printer {
  queue<Student>& Students;
  Student current;
  static const int slice = 5;
  int ttime; // Time left in slice
  bool busy; // Is printer serving a Student?
public:
  printer(queue<Student>& cq) 
    : Students(cq), ttime(0), busy(false) {}
  printer& operator=(const printer& rv) {
    Students = rv.Students;
    current = rv.current;
    ttime = rv.ttime;
    busy = rv.busy;
    return *this;
  }
  bool isBusy() { return busy; }
  void run(bool recursion = false) {
    if(!recursion)
      ttime = slice;
    int servtime = current.getTime();
    if(servtime > ttime) {
      servtime -= ttime;
      current.setTime(servtime);
      busy = true; // Still working on current
      return;
    }
    if(servtime < ttime) {
      ttime -= servtime;
      if(!Students.empty()) {
        current = Students.front();
        Students.pop(); // Remove it
        busy = true;
        run(true); // Recurse
      }
      return;
    }
    if(servtime == ttime) {
      // Done with current, set to empty:
      current = Student(0);
      busy = false;
      return; // No more time in this slice
    }
  }
};

// Inherit to access protected implementation:
class StudentQ : public queue<Student> {
public:
  friend ostream& 
  operator<<(ostream& os, const StudentQ& cd) {
    copy(cd.c.begin(), cd.c.end(), 
      ostream_iterator<Student>(os, ""));
    return os;
  }
};

int main() {
  StudentQ Students;
  list<printer> printers;
  typedef list<printer>::iterator TellIt;
  printers.push_back(printer(Students));
  srand(time(0)); // Seed random number generator
  while(true) {
    // Add a random number of Students to the
    // queue, with random service times:
    for(int i = 0; i < rand() % 5; i++)
      Students.push(Student(rand() % 15 + 1));
    cout << '{' << printers.size() << '}' 
      << Students << endl;
    // Have the printers service the queue:
    for(TellIt i = printers.begin(); 
      i != printers.end(); i++)
      (*i).run();
    cout << '{' << printers.size() << '}' 
      << Students << endl;
    // If line is too long, add another printer:
    if(Students.size() / printers.size() > 2)
      printers.push_back(printer(Students));
    // If line is short enough, remove a printer:
    if(printers.size() > 1 && 
      Students.size() / printers.size() < 2)
      for(TellIt i = printers.begin();
        i != printers.end(); i++)
        if(!(*i).isBusy()) {
          printers.erase(i);
          break; // Out of for loop
        }
  }
} 
