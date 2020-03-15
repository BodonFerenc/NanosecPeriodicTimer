#pragma once

// never using namespace in header
using namespace std;

class Task {    
    public:
        virtual bool run(const TIME&, const TIME&) = 0; 
        // default implementation should be used since C++11 in this case
        virtual ~Task() = default;
};
