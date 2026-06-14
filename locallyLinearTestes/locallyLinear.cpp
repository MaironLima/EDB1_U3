#include "locallyLinear.h"
#include <cstdlib>

LocallyLinearHashTable::LocallyLinearHashTable(int n, int beta) {
    size = n;
    blockSize = beta;
    numBlocks = (size + blockSize - 1) / blockSize;
    table.assign(size, 0);
    blockLoads.assign(numBlocks, 0);
    elementCount = 0; // iniciamente a tabela tem 0 elementos
}

InsertResult LocallyLinearHashTable::insert(uint64_t key) {
    //cout << key << endl;
    int probes_count = 0;
    uint64_t h1 = hash1(key) % size;
    uint64_t h2 = hash2(key) % size;
    //cout << h1 << endl;
    //cout << h2 << endl;

    int block1 = h1 / blockSize;
    int block2 = h2 / blockSize;

    int chosenBlock;

    if (blockLoads[block1] < blockLoads[block2])
        chosenBlock = block1;
    else if (blockLoads[block2] < blockLoads[block1])
        chosenBlock = block2;
    else
        chosenBlock = (rand() & 1) ? block1 : block2;

    int startOffset = (chosenBlock == block1) ? (h1 % blockSize) : (h2 % blockSize);

    for (int jump = 0; jump < numBlocks; jump++) {
        int currentBlock = (chosenBlock + jump) % numBlocks;
        int blockBegin = currentBlock * blockSize;

        if (blockBegin >= size)
            continue;

        int currentBlockSize = std::min(blockSize, size - blockBegin);

        if (blockLoads[currentBlock] >= currentBlockSize) {
            probes_count += currentBlockSize;
            continue;
        }

        int localStart = (jump == 0) ? startOffset % currentBlockSize : 0;

        for (int i = 0; i < currentBlockSize; i++) {
            probes_count++;
            int pos = blockBegin + ((localStart + i) % currentBlockSize);

            if (table[pos] == 0) {
                table[pos] = key;
                blockLoads[currentBlock]++;
                elementCount++; // toda vez q insere numa entrada vazia, aumenta o numero de elementos da tabela
                return {true, probes_count};
            }
        }
    }
    return {false, probes_count};
}

SearchResult LocallyLinearHashTable::search(uint64_t key) {
    int probes_count = 0;
    uint64_t h1 = hash1(key) % size;
    uint64_t h2 = hash2(key) % size;

    int block1 = h1 / blockSize;
    int block2 = h2 / blockSize;

    int offset1 = h1 % blockSize;
    int offset2 = h2 % blockSize;

    for (int jump = 0; jump < numBlocks; jump++) {
        int current1 = (block1 + jump) % numBlocks;
        int current2 = (block2 + jump) % numBlocks;

        bool reachedEmpty1 = false;
        bool reachedEmpty2 = false;

        int begin1 = current1 * blockSize;
        if (begin1 < size) {
            int currentBlockSize1 = std::min(blockSize, size - begin1);
            int start1 = (jump == 0) ? offset1 % currentBlockSize1 : 0;

            for (int i = 0; i < currentBlockSize1; i++) {
                probes_count++;
                int pos = begin1 + ((start1 + i) % currentBlockSize1);

                if (table[pos] == key)
                    return {true, probes_count};

                if (table[pos] == 0) {
                    reachedEmpty1 = true;
                    break;
                }
            }
        } else {
            reachedEmpty1 = true;
        }

        int begin2 = current2 * blockSize;
        if (begin2 < size) {
            int currentBlockSize2 = std::min(blockSize, size - begin2);
            int start2 = (jump == 0) ? offset2 % currentBlockSize2 : 0;

            for (int i = 0; i < currentBlockSize2; i++) {
                probes_count++;
                int pos = begin2 + ((start2 + i) % currentBlockSize2);

                if (table[pos] == key)
                    return {true, probes_count};

                if (table[pos] == 0) {
                    reachedEmpty2 = true;
                    break;
                }
            }
        } else {
            reachedEmpty2 = true;
        }

        if (reachedEmpty1 && reachedEmpty2) {
            return {false, probes_count};
        }
    }
    return {false, probes_count};
}

// Função para  retornar fator de carga
double LocallyLinearHashTable::loadFactor() const {
    return static_cast<double>(elementCount) / size;
}