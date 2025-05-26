// include/OptionPricing.h
#ifndef OPTION_PRICING_H
#define OPTION_PRICING_H

#include "Payoffs.h" // For Payoff base class
#include <vector>    // For vector<double>
#include <numeric>   // Will still be useful

// Structure to hold Monte Carlo simulation results
struct MonteCarloResult {
    double price;
    double standardError;
    double confidenceIntervalLower;
    double confidenceIntervalUpper;
    // You could also add a vector of payoffs here if you want to export for detailed analysis
    // std::vector<double> discountedPayoffs;
};

// Function to simulate a single asset price path according to Black-Scholes dynamics
double SimulateBlackScholesPath(
    double initialSpot,
    double riskFreeRate,
    double volatility,
    double timeToMaturity
);

// Monte Carlo pricer for European options, returning detailed results
MonteCarloResult MonteCarloEuropeanOption(
    double initialSpot,
    double riskFreeRate,
    double volatility,
    double timeToMaturity,
    long numSimulations,
    const Payoff& payoff,
    double confidenceLevel = 0.95 // Default to 95% confidence
);

#endif // OPTION_PRICING_H