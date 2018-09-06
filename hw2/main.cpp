#include <iostream>

#include <TwoBitCounter.h>

using namespace std;

int main(int argc, char* argv[]) {
    
    TwoBitCounter _2BC(TBC::StronglyNotTaken);

    for(int i = 0; i < 5; i++) {
        cout << _2BC(Branch::Taken) << endl;
    }
    
    return 0;
}