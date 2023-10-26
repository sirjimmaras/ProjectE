```#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

const int dimensions = 784;
const int maximages = 100;
const int max_capacity = 1000;
const int k = 1;

float v[dimensions];
float t = 3.0;
int w = 100;

using namespace std;

void createv() {
    mt19937 gen(random_device{}());
    normal_distribution<float> distribution(0.0, 1.0);
    for (int i=0; i<dimensions; i++) v[i] = distribution(gen)+2;
}

void hashing(int images[][dimensions], int buckets[][max_capacity], int bucket_size[]) {
    float product[maximages] = {0.0};
    int h[maximages];
    for (int i=0; i<maximages; i++) {
        for (int j=0; j<dimensions; j++) product[i] += images[i][j]*v[j];
        h[i] = floor((product[i] + t) / w);
    }

    cout << endl;
    cout << "h(p) values:" << endl;

    for (int i=0; i<maximages; i++) {
        cout << h[i] << " " ;
        int index = h[i] % 10;

        if(bucket_size[index] < max_capacity){
            buckets[index][bucket_size[index]] = h[i];
            bucket_size[index]++;
        }
    }
    cout << endl << endl;
}


int main() {

    const char* file_path = "1.dat";
    ifstream file(file_path,ios::binary);
    int numbers[dimensions];
    int images[maximages][dimensions];

    int byte_index = 16;
    unsigned char byte;

    for (int i=0; i<maximages*dimensions; i++) {
        file.seekg(byte_index, ios::beg);
        if (file.read((char*)&byte,1)) {
            if ((int)byte != 0) cout << (int)byte << " ";
            if (i % dimensions == 0) cout << endl << endl ;
            images[i/dimensions][(i%dimensions)-1] = (int)byte;
            byte_index++;
        }
        else cout << "End of file reached." << endl;
    }

    cout << endl;
    file.close();

int buckets [10][max_capacity] = {{0}};
int bucket_size[10] = {0};

createv();
hashing(images, buckets, bucket_size);

for (int i=0; i<10; i++) {
    cout << "Bucket " << i << ": ";
    for (int j=0; j<bucket_size[i];j++) {
        cout << buckets[i][j] << " ";
    }
    cout << endl << endl ;
}

return 0;
}
