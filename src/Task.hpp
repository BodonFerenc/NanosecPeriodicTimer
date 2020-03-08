#ifndef TASK_H
#define TASK_H

#include <time.h>

class Task {    
    public:
        virtual bool run(const struct timespec&, const struct timespec&) = 0; 
        virtual ~Task() {};
};

#endif
