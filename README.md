# Bond Price Calculator with Yield Curve Modeling

## Overview

This C++ project implements a sophisticated Bond Price Calculator that incorporates yield curve modeling. It provides both static pricing and Monte Carlo simulation methods for accurate bond valuation across various market scenarios.

## Features

- **Static Pricing**: Calculates bond prices using interpolated rates from a yield curve.
- **Monte Carlo Simulation**: Estimates bond prices considering yield curve volatility.
- **Yield Curve Modeling**: Implements a flexible yield curve class with linear interpolation.
- **Support for Various Bond Types**: Handles both coupon-bearing and zero-coupon bonds.
- **Robust Error Handling**: Includes input validation and edge case management.


## Sample Output
Enter bond face value: 1000
Enter annual coupon rate (as a decimal): 0.05
Enter years to maturity: 10
Enter coupons per year: 2
Enter yield to maturity (as a decimal): 0.05
Enter number of Monte Carlo simulations: 10000

Results:

Static Bond Price: $1000.00

Monte Carlo Bond Price: $1003.28 plus or minus $77.60

Duration: 7.99 years

Convexity: 77.43

## Future Enhancements

- Implement more complex yield curve models (e.g., Nelson-Siegel)
- Add duration and convexity calculations for non-flat yield curves
- Optimize Monte Carlo simulations for improved performance
- Expand bond types (e.g., callable bonds, floating-rate bonds)
