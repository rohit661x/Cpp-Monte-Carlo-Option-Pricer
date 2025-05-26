// include/Random.h
#ifndef RANDOM_H
#define RANDOM_H

#include <random> // For std::mt19937, std::normal_distribution
#include <chrono> // For std::chrono::system_clock

namespace Random {

    // Global random engine (seeded once for efficiency)
    // Using a static engine within a function to ensure it's initialized once.
    // We could also pass it around, but for simplicity here, we'll keep it static.
    static std::mt19937& GetGenerator() {
        // Seed with current time
        static std::mt19937 generator(static_cast<unsigned int>(
            std::chrono::system_clock::now().time_since_epoch().count()
        ));
        return generator;
    }

    // Function to generate a standard normal random variable (mean 0, std dev 1)
    inline double GetStandardNormal() {
        static std::normal_distribution<double> distribution(0.0, 1.0);
        return distribution(GetGenerator());
    }

} // namespace Random

#endif // RANDOM_H