#include <iostream>
#include "KDBPublisher.hpp"

using namespace std;

KDBPublisher::KDBPublisher(unsigned long triggerNr, string host, int port) {
    socket = khpu(S(host.c_str()), port, S("Administrator:password")); 
    if (socket < 0) {
        /* problem connecting */
        cerr << "Problem connecting to kdb at " << host << ":" << port 
             << ". Error code " << socket << endl;
        exit(1);
    }
    cout << "Connection to " << host << ":" << port << " was successful" << endl;
}
  

KDBPublisher::~KDBPublisher() {
    // explicit flush is required before close if async messages were sent.
    // https://code.kx.com/q/ref/hopen/#hclose
    k(-socket, (char *) "[]", (K)0);      
    kclose(socket);   
    cout << "KDB+ connection closed" <<  endl; 
}
