// src/OptionPricing.cpp
#include "OptionPricing.h"
#include "Random.h" // For Random::GetStandardNormal()
#include <cmath>    // For std::exp, std::sqrt
#include <vector>   // For std::vector
#include <numeric>  // For std::accumulate
#include <iostream> // For debugging, can remove later

// Helper function to calculate standard deviation
double calculate_std_dev(const std::vector<double>& data, double mean) {
    if (data.size() <= 1) return 0.0; // Avoid division by zero for single or no data points
    double sum_sq_diff = 0.0;
    for (double val : data) {
        sum_sq_diff += (val - mean) * (val - mean);
    }
    // Using (data.size() - 1) for sample standard deviation (unbiased estimator)
    return std::sqrt(sum_sq_diff / (data.size() - 1));
}

// Function to simulate a single asset price path according to Black-Scholes dynamics
double SimulateBlackScholesPath(
    double initialSpot,
    double riskFreeRate,
    double volatility,
    double timeToMaturity
) {
    // Only interested in the final price for European options
    // S_T = S_0 * exp((r - 0.5 * sigma^2) * T + sigma * sqrt(T) * Z)
    double exponent = (riskFreeRate - 0.5 * volatility * volatility) * timeToMaturity +
                      volatility * std::sqrt(timeToMaturity) * Random::GetStandardNormal();
    return initialSpot * std::exp(exponent);
}

// Monte Carlo pricer for European options
MonteCarloResult MonteCarloEuropeanOption(
    double initialSpot,
    double riskFreeRate,
    double volatility,
    double timeToMaturity,
    long numSimulations,
    const Payoff& payoff,
    double confidenceLevel // Added confidenceLevel parameter
) {
    std::vector<double> discountedPayoffs;
    discountedPayoffs.reserve(numSimulations); // Pre-allocate memory for efficiency

    for (long i = 0; i < numSimulations; ++i) {
        double finalSpotPrice = SimulateBlackScholesPath(
            initialSpot,
            riskFreeRate,
            volatility,
            timeToMaturity
        );
        double currentPayoff = payoff(finalSpotPrice);
        discountedPayoffs.push_back(currentPayoff * std::exp(-riskFreeRate * timeToMaturity));
    }

    // Calculate statistics
    double sumOfPayoffs = std::accumulate(discountedPayoffs.begin(), discountedPayoffs.end(), 0.0);
    double averagePayoff = sumOfPayoffs / numSimulations;

    double stdDev = calculate_std_dev(discountedPayoffs, averagePayoff);
    double standardError = stdDev / std::sqrt(numSimulations);

    // Calculate confidence interval (assuming large N for Z-score approximation)
    // For specific confidence levels, we use approximate Z-scores.
    // In a production environment, you might use a more robust way to get Z-scores
    // from a standard normal inverse CDF or a t-distribution for smaller N.
    double z_score;
    if (confidenceLevel == 0.90) {
        z_score = 1.645;
    } else if (confidenceLevel == 0.99) {
        z_score = 2.576;
    } else { // Default or 0.95
        z_score = 1.96;
    }

    double marginOfError = z_score * standardError;

    MonteCarloResult result;
    result.price = averagePayoff; // This is the discounted price already
    result.standardError = standardError;
    result.confidenceIntervalLower = averagePayoff - marginOfError;
    result.confidenceIntervalUpper = averagePayoff + marginOfError;
    // result.discountedPayoffs = discountedPayoffs; // Uncomment if you need to export for plotting direct access

    return result;
}