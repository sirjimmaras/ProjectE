#include <iostream>
#include <fstream>

using namespace std;

struct Coordinates {
    int x;
    int y;
};

int main() {

    ifstream rf("1.dat", ios::binary);
    Coordinates a[3];
    for (int i=0; i<3; i++)
        rf.read((int*) &a[i], sizeof(Coordinates)) ;

    cout << "Numbers read:" << endl;
    for (int i=0; i<3; i++)
        cout << "number " << i << "is" << a[i] << endl;
    return 0;

}