#include <iostream>
#include "thread_pool_executor/thread_pool_executor.hpp"
#include "prime_generator/prime_generator.h"

int main() {
    PrimeGenerator x{10000000000, 1000000000, 20, "../primes.txt"};
    x.generate();
}
