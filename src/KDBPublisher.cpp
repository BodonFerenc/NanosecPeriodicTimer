#include <iostream>
#include "KDBPublisher.hpp"
#include "k.h"

using namespace std;

KDBPublisher::KDBPublisher(unsigned long triggerNr, const char* argv[]) {  

    socket = khpu(S(argv[0]), atoi(argv[1]), S("Administrator:password")); 
    if (socket < 0) {
        /* problem connecting */
        cerr << "Problem connecting to kdb at " << argv[0] << ":" << argv[1] 
             << ". Error code " << socket << endl;
        exit(1);
    }
    cout << "Connection to " << argv[0] << ":" << argv[1] << " was successful" << endl;
}
  

KDBPublisher::~KDBPublisher() {
    kclose(socket);   
    cout << "KDB+ connection closed" <<  endl; 
}
