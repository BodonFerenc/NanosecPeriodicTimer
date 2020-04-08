#include <iostream>
#include "KDBPublisher.hpp"

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
    // explicit flush is required before close if async messages were sent.
    // https://code.kx.com/q/ref/hopen/#hclose
    k(-socket, (char *) "[]", (K)0);      
    kclose(socket);   
    cout << "KDB+ connection closed" <<  endl; 
}
