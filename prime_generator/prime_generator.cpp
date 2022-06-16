//
// Created by John Thomson on 2022-06-16.
//

#include "prime_generator.h"

#include <utility>
#include <cmath>
#include <iostream>

#include "../thread_pool_executor/thread_pool_executor.hpp"

using namespace std;

PrimeGenerator::PrimeGenerator(long long upto, long long block_size, int num_threads, string out_file) {
    this->upto = upto;
    this->block_size = block_size;
    this->num_threads = num_threads;
    this->out_file = std::move(out_file);
    f.open(this->out_file);
}

void PrimeGenerator::generate() {
    // first, generate primes up to sqrt(upto)
    // upto must be less than INT_MAX^2
    // this is a relatively small number, and is single threaded because I'm lazy
    int init_sz = (int) (sqrt(upto) + 1);
    bool *x = new bool[init_sz];
    fill_n(x, init_sz, true);
    for (int i = 2; i < sqrt(init_sz); i++) {
        for (int mult = 2; i * mult < init_sz; mult++) {
            x[i * mult] = false;
        }
    }
    x[0] = false;
    x[1] = false;
    dump_to_file(x, 0, init_sz);
    vector<int> nums;
    nums.reserve(init_sz);
    for (int i = 0; i < init_sz; i++) {
        if (x[i]) {
            nums.push_back(i);
        }
    }

    delete[] x;

    nums.shrink_to_fit();

    // now that we have the list of prime numbers under sqrt(upto), we make sequential blocks starting from that value

    x = new bool[this->block_size];
    for (long long start_block = init_sz + 1; start_block < this->upto; start_block += this->block_size) {
        fill_n(x, this->block_size, true);
        ThreadPoolExecutor pool(this->num_threads, this->num_threads, 5s, INT_MAX);
        int sz = min(this->block_size, upto - start_block);
        for (int &num: nums) {
            pool.submit([&]() {
                iterate(x, sz, start_block, num);
            });
        }
        pool.shutdown();
        pool.wait();
        dump_to_file(x, start_block, sz);
        cout << start_block << endl;
    }

    write_mutex.lock();

    f.close();
}

void PrimeGenerator::iterate(bool *arr, int sz, long long start, long long val) {
    long long mult;
    if (val % start == 0) {
        mult = start / val;
    } else {
        mult = start / val + 1;
    }

    for (; val * mult < start + sz; mult++) {
        arr[val * mult - start] = false;
    }
}

PrimeGenerator::PrimeGenerator() : PrimeGenerator(1000, 100, 1, "primes.txt") {

}

void PrimeGenerator::dump_to_file(const bool *arr, long long start, long long size) {
    write_mutex.lock();
    thread t{[=] {
        for (int i = 0; i < size; i++) {
            if (arr[i]) {
                f << start + i << " ";
            }
        }
        write_mutex.unlock();
    }};
    t.detach();
}
