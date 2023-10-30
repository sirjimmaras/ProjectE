#include <iostream>
#include <fstream>
#include <random>
#include <cmath>

const int dimensions = 784;
const int maximages = 10;
const int max_capacity = 1000;
const int k = 3;
const int m = 100;
const int max_hc = 1000*k;

float v[k][dimensions];
int r[k];
float t = 3.0;
int w = 100;
int hcmap[max_hc];
int cluster[k][dimensions];

using namespace std;

void vector_from_file(const char* filepath, int vector[], int offset) {
    ifstream file(filepath,ios::binary);
    unsigned char byte;

    for (int i=0; i<dimensions; i++) {
        file.seekg(offset, ios::beg);
        if (file.read((char*)&byte,1))  vector[i] = (int)byte;            
        else cout << "Error" << endl;
        offset++;
    }       
    file.close();
}

float v_distance (int vector1[], int vector2[], int dim) {
    int sum = 0 ;
    for (int i=0; i<dimensions; i++) sum += pow(vector1[i] - vector2[i], dim);
    float v_distance = pow(sum, 1.0/dim) ;
    return v_distance;
}

void clustering(int images[][dimensions], int cluster[][dimensions], int centroid[][max_capacity], int centroid_size[]) {
    int index;
    float distance;
    float min_distance = 1000000.0;
    // static int step = 1;
    // if (step == 4) return;
    for (int i=0; i<maximages; i++) {
        for (int kh=0; kh<k; kh++) {
            distance = v_distance(images[i],cluster[kh],2);
            if (distance < min_distance) {
                min_distance = distance;
                index = kh;
            }
        }
        if(centroid_size[index] < max_capacity) {
            centroid[index][centroid_size[index]] = i;
            centroid_size[index]++;
        }
    }
    int new_centroid[k][dimensions];
    int sum=0;
    for (int kh=0; kh<k; kh++) {
        for (int i=0; i<dimensions; i++) {
            for (int j=0; j<centroid_size[kh]; j++) sum += images[centroid[kh][j]][i];
            if (centroid_size[kh]!=0) new_centroid[kh][i] = sum / centroid_size[kh];
            else new_centroid[kh][i] = 0;
            // step++;
        }
    }
    // for (int kh=0;kh<k; kh++) centroid [k][max_capacity] = {0};
    // centroid_size[k] = {0};
    // clustering(images,new_centroid,centroid,centroid_size);
}

int silhouette(int images[][dimensions],int cluster[][dimensions], int centroid[][max_capacity], int centroid_size[]) {
    int closest;
    float avg_dist,sum,avg_dist_b,sum_b,dist,min_dist,s;
    min_dist = 1000000.0;
    dist = min_dist - 1.0;
    
    for (int kh=0; kh<k; kh++) {
        for (int c=0; c<k; c++) {
            if (c!=kh) dist = v_distance(images[centroid[kh][0]],cluster[c],2);
            if(dist<min_dist) min_dist = dist;
            closest = c;
        }
        for (int i=0; i<centroid_size[kh]; i++) {
            sum += v_distance(images[centroid[kh][0]],images[centroid[kh][i]],2);
        }
        avg_dist = sum/centroid_size[kh];
        for (int i=0; i<centroid_size[kh]; i++) {
            sum_b += v_distance(images[centroid[closest][0]],images[centroid[closest][i]],2);
        }
        avg_dist_b = sum/centroid_size[kh];        
    }
    if (avg_dist>avg_dist_b) s = (avg_dist_b-avg_dist)/avg_dist;
    else s = (avg_dist_b-avg_dist)/avg_dist_b;
    return s;
}

void create_rand() {
    mt19937 gen(random_device{}());
    normal_distribution<float> distribution(0.0, 1.0);
    for (int kh=0; kh<k; kh++) {
            for (int i=0; i<dimensions; i++) {
                v[kh][i] = (distribution(gen)+2);
                cluster[kh][i] = gen()%256;
            }
        if (kh!=0) r[kh] = gen() % 10 + 1;
        else r[kh] = 1;
    }
    for (int i=0; i<max_hc; i++) hcmap[i] = gen() % 2;
}

int hamm_distance(int x, int y) {
    int distance = 0;
    for (int i = 0; i < sizeof(int) * 8; i++) {
        int bit1 = (x >> i) & 1;
        int bit2 = (y >> i) & 1;

        if (bit1 != bit2) distance++;
    }
    return distance;
} 

int exhaustive_search (int query[], int dataset[][dimensions]) {
    int min_vector = 0;
    float distance;
    float min_distance = 1000000.0;
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
    return id;
}

int hypercube(int query[]) {
    int h;
    int index = 0;
    float product = 0.0;
    for (int kh=0; kh<k; kh++) {
        for (int i=0; i<dimensions; i++) product += query[i]*v[kh][i];
        h = floor((product + t) / w);
        index += hcmap[h % max_hc] * pow(2,kh);
    }
    return index;
}

void dataset_hc_hash(int images[][dimensions], int hc_buckets[][max_capacity],int hc_bucket_size[]) {
    int index;
    for (int i=0; i<maximages; i++) {
        index = hypercube(images[i]);
        if(hc_bucket_size[index] < max_capacity){
            hc_buckets[index][hc_bucket_size[index]] = i;
            hc_bucket_size[index]++;
        }
    }
}

void dataset_hash(int images[][dimensions], int buckets[][max_capacity], int bucket_size[], int imginfo[][2]) {
    int id,index;
    for (int i=0; i<maximages; i++) {
        id = hashing(images[i]);
        imginfo[i][0] = id;
        index = id % 10;
        if(bucket_size[index] < max_capacity){
            buckets[index][bucket_size[index]] = i;
            bucket_size[index]++;
            imginfo[i][1] = index;
        }
    }
}

int lsh_search (int query[], int buckets[][max_capacity], int bucket_size[], int imginfo[][2], int dataset[][dimensions]) {
    int min_vector = -1;
    int position;
    float distance;
    float min_distance = 1000000.0;
    int id = hashing(query);
    int index = id % 10;
    for (int i=0; i<bucket_size[index]; i++) {
        position = buckets[index][i];
        if (imginfo[position][0] == id) {
            distance = v_distance(query, dataset[position],2);
            if (distance < min_distance) {
                min_distance = distance;
                min_vector = position;
            }
        }
    }
    return min_vector;
}

int main() {
    create_rand();
    const char* file_path = "input.dat";

    int query[dimensions];
    int images[maximages][dimensions];
    int s = 0;
    int es = 0;

    for (int i=0; i<maximages; i++)  {
        vector_from_file(file_path, images[i], 16+i*dimensions);
    }

    int buckets [10][max_capacity] = {{0}};
    int bucket_size[10] = {0};
    int imginfo[maximages][2];
    int hc_bnum = pow(2,k);
    int hc_buckets [hc_bnum][max_capacity] = {{0}};
    int hc_bucket_size[hc_bnum] = {0};
    int centroid [k][max_capacity] = {{0}};
    int centroid_size [k] = {0};


    dataset_hash(images, buckets, bucket_size, imginfo);
    dataset_hc_hash(images, hc_buckets, hc_bucket_size);
    // lsh_search(images[0],buckets,bucket_size,imginfo,images);
    // hypercube(images[0]);
    es = exhaustive_search(images[3], images);
    cout <<"NN to provided query is image no"<< es << endl;
    clustering (images,cluster,centroid,centroid_size);
    s = silhouette(images,cluster,centroid,centroid_size);

    return 0;
}
