#include <iostream>
#include <fstream>

using namespace std;

int main() {

    const char* file_path = "1.dat";
    ifstream file("1.dat",ios::binary);

    cin.unsetf(ios::dec) ;
    int index;
    cout << "Which byte do you want to read?" << endl;
    cin >> index;

    file.seekg(index, ios::beg);

    unsigned char byte;
    if (file.read((char*)&byte,1)) {
        cout << "Byte at index " << index << " is " << (int)byte << endl;
    }
    else cout << "End of file reached." << endl;

    file.close();
    return 0;
}