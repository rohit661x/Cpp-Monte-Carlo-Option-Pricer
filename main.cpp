// src/main.cpp
#include <iostream>
#include <memory>    // For std::unique_ptr
#include <vector>
#include <iomanip>   // For std::fixed and std::setprecision
#include <fstream>   // For file output (for CSV)

#include "OptionPricing.h" // Includes MonteCarloResult, SimulateBlackScholesPath, MonteCarloEuropeanOption
#include "Payoffs.h"       // Includes Payoff base class, PayoffCall, PayoffPut

// Function for convergence analysis
void runConvergenceAnalysis(
    double initialSpot,
    double strike,
    double riskFreeRate,
    double volatility,
    double timeToMaturity,
    const Payoff& payoff,
    const std::string& optionType,
    const std::string& filenamePrefix // New parameter for filename
) {
    std::cout << "\n--- Convergence Analysis for " << optionType << " ---\n";
    std::cout << std::fixed << std::setprecision(6); // Set precision for console output

    // Define simulation counts for convergence analysis
    std::vector<long> simulationCounts = {
        1000, 5000, 10000, 50000, 100000, 200000, 500000, 1000000, 2000000, 5000000
    }; // Added more points for a smoother graph

    std::string filename = filenamePrefix + "_convergence.csv";
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
        // Optionally, you might want to throw an exception or exit here
        return;
    }

    // Write CSV header
    outputFile << "Simulations,Price,StandardError,CILower,CIUpper\n";
    std::cout << "Simulations | Price       | Std. Error  | 95% CI Lower | 95% CI Upper\n";
    std::cout << "------------|-------------|-------------|--------------|--------------\n";

    for (long numSims : simulationCounts) {
        MonteCarloResult result = MonteCarloEuropeanOption(
            initialSpot,
            riskFreeRate,
            volatility,
            timeToMaturity,
            numSims,
            payoff
        );
        // Output to console
        std::cout << std::setw(11) << numSims << " | "
                  << std::setw(11) << result.price << " | "
                  << std::setw(11) << result.standardError << " | "
                  << std::setw(12) << result.confidenceIntervalLower << " | "
                  << std::setw(12) << result.confidenceIntervalUpper << "\n";

        // Output to CSV
        outputFile << numSims << ","
                   << result.price << ","
                   << result.standardError << ","
                   << result.confidenceIntervalLower << ","
                   << result.confidenceIntervalUpper << "\n";
    }
    std::cout << "------------------------------------------------------------------\n";
    outputFile.close();
    std::cout << "Convergence data saved to " << filename << "\n";
}


int main() {
    // Option Parameters
    double initialSpot = 100.0;     // Current stock price
    double strike = 105.0;          // Option strike price
    double riskFreeRate = 0.05;     // Risk-free interest rate (5%)
    double volatility = 0.20;       // Volatility (20%)
    double timeToMaturity = 1.0;    // Time to maturity in years

    long numSimulations = 1000000;   // Increased for better accuracy in final estimate

    std::cout << "Monte Carlo European Option Pricer - Foundations\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "Initial Spot Price: " << initialSpot << "\n";
    std::cout << "Strike Price: " << strike << "\n";
    std::cout << "Risk-Free Rate: " << riskFreeRate << "\n";
    std::cout << "Volatility: " << volatility << "\n";
    std::cout << "Time to Maturity: " << timeToMaturity << " years\n";
    std::cout << "Base Number of Simulations: " << numSimulations << "\n\n";

    // Price a European Call Option
    // Using std::unique_ptr for automatic memory management of Payoff objects
    std::unique_ptr<Payoff> callPayoff = std::make_unique<PayoffCall>(strike);
    MonteCarloResult callResult = MonteCarloEuropeanOption(
        initialSpot,
        riskFreeRate,
        volatility,
        timeToMaturity,
        numSimulations,
        *callPayoff // Dereference to pass by const Payoff&
    );
    std::cout << "Estimated European Call Price:         " << callResult.price << "\n";
    std::cout << "Call Price Standard Error:             " << callResult.standardError << "\n";
    std::cout << "Call Price 95% Confidence Interval:    ["
              << callResult.confidenceIntervalLower << ", "
              << callResult.confidenceIntervalUpper << "]\n\n";

    // Price a European Put Option
    std::unique_ptr<Payoff> putPayoff = std::make_unique<PayoffPut>(strike);
    MonteCarloResult putResult = MonteCarloEuropeanOption(
        initialSpot,
        riskFreeRate,
        volatility,
        timeToMaturity,
        numSimulations,
        *putPayoff // Dereference to pass by const Payoff&
    );
    std::cout << "Estimated European Put Price:          " << putResult.price << "\n";
    std::cout << "Put Price Standard Error:              " << putResult.standardError << "\n";
    std::cout << "Put Price 95% Confidence Interval:     ["
              << putResult.confidenceIntervalLower << ", "
              << putResult.confidenceIntervalUpper << "]\n\n";

    // Run convergence analysis for Call Option
    runConvergenceAnalysis(initialSpot, strike, riskFreeRate, volatility, timeToMaturity, *callPayoff, "Call Option", "call_option");

    // Run convergence analysis for Put Option
    runConvergenceAnalysis(initialSpot, strike, riskFreeRate, volatility, timeToMaturity, *putPayoff, "Put Option", "put_option");

    return 0;
}