// include/Payoffs.h
#ifndef PAYOFFS_H
#define PAYOFFS_H

#include <algorithm> // For std::max

// Base class/interface for Payoffs (using polymorphism for flexibility)
class Payoff {
public:
    // The pure virtual operator() makes this an abstract base class.
    // Derived classes must implement this to calculate the option's payoff.
    virtual double operator()(double spotPrice) const = 0;

    // Virtual destructor is crucial for proper memory management when
    // deleting objects of derived classes through a base class pointer.
    virtual ~Payoff() = default;
};

// European Call Payoff: max(0, S_T - K)
class PayoffCall : public Payoff {
public:
    // Constructor takes the strike price
    PayoffCall(double strike) : strike_(strike) {}

    // Calculates the payoff for a call option
    double operator()(double spotPrice) const override {
        return std::max(0.0, spotPrice - strike_);
    }
private:
    double strike_; // Strike price of the option
};

// European Put Payoff: max(0, K - S_T)
class PayoffPut : public Payoff {
public:
    // Constructor takes the strike price
    PayoffPut(double strike) : strike_(strike) {}

    // Calculates the payoff for a put option
    double operator()(double spotPrice) const override {
        return std::max(0.0, strike_ - spotPrice);
    }
private:
    double strike_; // Strike price of the option
};

#endif // PAYOFFS_H