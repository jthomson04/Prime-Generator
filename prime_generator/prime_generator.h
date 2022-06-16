//
// Created by John Thomson on 2022-06-16.
//

#ifndef PRIMES_PRIME_GENERATOR_H
#define PRIMES_PRIME_GENERATOR_H

#include <string>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;


class PrimeGenerator {
public:
    PrimeGenerator(long long upto, long long block_size, int num_threads, string out_file);

    PrimeGenerator();

    void generate();

private:
    long long upto;
    long long block_size;
    int num_threads;
    string out_file;
    ofstream f;
    mutex write_mutex;

    static void iterate(bool *arr, int sz, long long start, long long val);

    void dump_to_file(const bool *arr, long long start, long long size);
};


#endif //PRIMES_PRIME_GENERATOR_H
