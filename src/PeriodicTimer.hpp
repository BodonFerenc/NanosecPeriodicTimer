#ifndef PERIODICTIMER_H
#define PERIODICTIMER_H


template<class T>
class PeriodicTimer {
    protected:    
        T& task;    
    public: 
        PeriodicTimer(T& t) : task{t} {}

        virtual void run(unsigned long wait, unsigned long nr) = 0; 
        virtual ~PeriodicTimer() {};
};

#endif

