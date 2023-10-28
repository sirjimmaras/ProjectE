#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

const int dimensions = 784;
const int maximages = 100;
const int max_capacity = 1000;
const int k = 3;
const int m = 100;

float v[k][dimensions];
int r[k];
float t = 3.0;
int w = 100;

using namespace std;

void vector_from_file(const char* filepath, int vector[], int offset) {
    ifstream file(filepath,ios::binary);
    unsigned char byte;

    for (int i=0; i<dimensions; i++) {
        file.seekg(offset, ios::beg);
        if (file.read((char*)&byte,1)) {
            // if ((int)byte != 0) cout << (int)byte << " ";
            vector[i] = (int)byte;            
        }
        else cout << "Error" << endl;
        offset++;
    }       

    cout << endl;
    file.close();
}

void create_rand() {
    mt19937 gen(random_device{}());
    normal_distribution<float> distribution(0.0, 1.0);
    for (int kh=0; kh<k; kh++) {
            for (int i=0; i<dimensions; i++) v[kh][i] = (distribution(gen)+2);
        if (kh!=0) r[kh] = gen() % 10 + 1;
        else r[kh] = 1;
    }
}

float v_distance (int vector1[], int vector2[], int dim) {
    int sum = 0 ;
    for (int i=0; i<dimensions; i++) sum += pow(vector1[i] - vector2[i], dim);
    float v_distance = pow(sum, 1.0/dim) ;
    return v_distance;
}

int exhaustive_search (int query[], int dataset[][dimensions]) {
    int min_vector = 0;
    float distance;
    float min_distance = v_distance(query,dataset[0],2);
    for (int i=0; i<maximages; i++) {
        distance = v_distance(query, dataset[i], 2);
        if (distance < min_distance) {
            min_distance = distance;
            min_vector = i;
        }
    }
    return min_vector;
}

int hashing(int query[]) {
    int h;
    int id = 0;
    float product = 0.0;

    for (int kh=0; kh<k; kh++) {
        for (int i=0; i<dimensions; i++) product += query[i]*v[kh][i];
        h = floor((product + t) / w);
        id += h*r[kh];
    }
    id = id % m;
    cout << id << " ";

    return id;
}

void dataset_hash(int images[][dimensions], int buckets[][max_capacity], int bucket_size[]) {
    create_rand();
    cout << "id(p) values:" << endl;
    int id,index;
    for (int i=0; i<maximages; i++) {
        id = hashing(images[i]);
        index = id % 10;
        if(bucket_size[index] < max_capacity){
            buckets[index][bucket_size[index]] = id;
            bucket_size[index]++;
        }
    }
    cout << endl << endl;
}


int main() {
    const char* file_path = "1.dat";

    int query[dimensions];
    int images[maximages][dimensions];

    for (int i=0; i<maximages; i++)  {
        vector_from_file(file_path, images[i], 16+i*dimensions);
        cout << endl;
    }

    int buckets [10][max_capacity] = {{0}};
    int bucket_size[10] = {0};


    dataset_hash(images, buckets, bucket_size);

    for (int i=0; i<10; i++) {
        cout << "Bucket " << i << ": ";
        for (int j=0; j<bucket_size[i];j++) {
            cout << buckets[i][j] << " ";
        }
        cout << endl << endl ;
    }

    for (int i= 0; i<=110; i++)
    {
        vector_from_file("1.dat", query, 16+i*dimensions);
        int minvec = exhaustive_search(query, images);
        cout << "The nearest neighbor to vector " << i << " from file is " ;
        cout << "vector number " << minvec << endl ;
    }

    return 0;
}
