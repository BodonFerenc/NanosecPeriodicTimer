#ifndef TASK_H
#define TASK_H

using namespace std;

class Task {    
    public:
        virtual bool run(const TIME&, const TIME&) = 0; 
        virtual ~Task() {};
};

#endif
