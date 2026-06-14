#ifndef LOCALLY_LINEAR_H
#define LOCALLY_LINEAR_H

#include <vector>
#include <cstdint>
#include <algorithm>
//#include <iostream>
//using namespace std;

struct InsertResult {
    bool success;
    int probes;
};

struct SearchResult {
    bool found;
    int probes;
};

class LocallyLinearHashTable {
private:
    int size;
    int blockSize;
    int numBlocks;

    std::vector<uint64_t> table;
    std::vector<int> blockLoads;

    int elementCount; // número de elementos na tabela

    //SplitMix64
    static uint64_t hash1(uint64_t x) {
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebULL;
        x ^= x >> 31;
        return x;
    }
    
    //SplitMix64
    static uint64_t hash2(uint64_t x) {
        x += 0x9e3779b97f4a7c15ULL;
        x ^= x >> 30;
        x *= 0xbf58476d1ce4e5b9ULL;
        x ^= x >> 27;
        x *= 0x94d049bb133111ebULL;
        x ^= x >> 31;
        return x;
    }

public:
    LocallyLinearHashTable(int n, int beta);
    InsertResult insert(uint64_t key);
    SearchResult search(uint64_t key);
    double loadFactor() const;          // Função para retornar o alfa
};

#endif // LOCALLY_LINEAR_H