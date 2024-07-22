#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <numeric>


// YieldCurve class
class YieldCurve {
private:
    std::vector<double> maturities;
    std::vector<double> rates;

public:
    YieldCurve(const std::vector<double>& maturities, const std::vector<double>& rates)
        : maturities(maturities), rates(rates) {
        if (maturities.size() != rates.size()) {
            throw std::invalid_argument("Maturities and rates must have the same size");
        }
    }
    // Interpolate the yield for a given maturity
    double interpolate(double t) const {
        auto it = std::lower_bound(maturities.begin(), maturities.end(), t);
        if (it == maturities.begin()) return rates.front();
        if (it == maturities.end()) return rates.back();

        int index = std::distance(maturities.begin(), it);
        double t0 = maturities[index - 1];
        double t1 = maturities[index];
        double r0 = rates[index - 1];
        double r1 = rates[index];
        // Linear Interpolation
        return r0 + (r1 - r0) * (t - t0) / (t1 - t0);
    }
};


class BondPriceCalculator {
private:
    double face_value;
    double coupon_rate;
    int years_to_maturity;
    int coupons_per_year;
    YieldCurve yield_curve;
    int num_simulations;
    bool is_zero_coupon;

    std::mt19937 gen;
    std::normal_distribution<> dist;

public:
    BondPriceCalculator(double fv, double cr, int ytm, int cpy, 
                        const YieldCurve& yc, int sims)
        : face_value(fv), coupon_rate(cr), years_to_maturity(ytm), coupons_per_year(cpy),
          yield_curve(yc), num_simulations(sims),
          gen(std::random_device{}()), dist(0.0, 0.005) {
        validateInputs();
        is_zero_coupon = (coupon_rate == 0.0 && coupons_per_year == 1);
    }


    void validateInputs() {
        if (face_value <= 0) throw std::invalid_argument("Face value must be positive");
        if (coupon_rate < 0) throw std::invalid_argument("Coupon rate cannot be negative");
        if (years_to_maturity <= 0) throw std::invalid_argument("Years to maturity must be positive");
        if (coupons_per_year <= 0) throw std::invalid_argument("Coupons per year must be positive");
        if (num_simulations <= 0) throw std::invalid_argument("Number of simulations must be positive");
    }

    // Static price
    double calculateStaticPrice() {
        if (is_zero_coupon) {
            double ytm = yield_curve.interpolate(years_to_maturity);
            return face_value / std::pow(1 + ytm, years_to_maturity);
        }

        double price = 0.0;
        double coupon_payment = face_value * coupon_rate / coupons_per_year;
        
        for (int i = 1; i <= years_to_maturity * coupons_per_year; ++i) {
            double t = i / static_cast<double>(coupons_per_year);
            double ytm = yield_curve.interpolate(t);
            price += coupon_payment / std::pow(1 + ytm / coupons_per_year, i);
        }
        
        double final_ytm = yield_curve.interpolate(years_to_maturity);
        price += face_value / std::pow(1 + final_ytm / coupons_per_year, years_to_maturity * coupons_per_year);
        
        return price;
    }

    // Monte Carlo simulation
    std::pair<double, double> calculateMonteCarlo() {
        std::vector<double> prices;
        prices.reserve(num_simulations);

        for (int i = 0; i < num_simulations; ++i) {
            YieldCurve shifted_curve = shiftYieldCurve();
            double price = calculatePrice(shifted_curve);
            prices.push_back(price);
        }

        double mean = std::accumulate(prices.begin(), prices.end(), 0.0) / num_simulations;
        double variance = std::accumulate(prices.begin(), prices.end(), 0.0,
            [mean](double acc, double price) {
                return acc + std::pow(price - mean, 2);
            }) / (num_simulations - 1);

        return {mean, std::sqrt(variance)};
    }

    // Shifted yield curve for MC simulation
    YieldCurve shiftYieldCurve() {
        std::vector<double> maturities, rates;
        for (int i = 1; i <= years_to_maturity; ++i) {
            maturities.push_back(i);
            double base_rate = yield_curve.interpolate(i);
            double shifted_rate = base_rate + dist(gen);
            rates.push_back(std::max(0.0, shifted_rate));  // Non-negative rates
        }
        return YieldCurve(maturities, rates);
    }

    // Calculate bond price with a given yield
    double calculatePrice(const YieldCurve& curve) {
        if (is_zero_coupon) {
            double ytm = curve.interpolate(years_to_maturity);
            return face_value / std::pow(1 + ytm, years_to_maturity);
        }

        double price = 0.0;
        double coupon_payment = face_value * coupon_rate / coupons_per_year;
        
        for (int i = 1; i <= years_to_maturity * coupons_per_year; ++i) {
            double t = i / static_cast<double>(coupons_per_year);
            double ytm = curve.interpolate(t);
            price += coupon_payment / std::pow(1 + ytm / coupons_per_year, i);
        }
        
        double final_ytm = curve.interpolate(years_to_maturity);
        price += face_value / std::pow(1 + final_ytm / coupons_per_year, years_to_maturity * coupons_per_year);
        
        return price;
    }

    // Duration and concavity need to be changes for yield curves that are not flat
    // This is for further improvement
};

int main() {
    try {
        double face_value, coupon_rate;
        int years_to_maturity, coupons_per_year, num_simulations;

        std::cout << "Enter bond face value: ";
        std::cin >> face_value;
        std::cout << "Enter annual coupon rate (as a decimal): ";
        std::cin >> coupon_rate;
        std::cout << "Enter years to maturity: ";
        std::cin >> years_to_maturity;
        std::cout << "Enter coupons per year: ";
        std::cin >> coupons_per_year;
        std::cout << "Enter number of Monte Carlo simulations: ";
        std::cin >> num_simulations;

        // Simple yield curve for demonstration
        std::vector<double> maturities = {1, 2, 3, 5, 10, 30};
        std::vector<double> rates = {0.01, 0.015, 0.02, 0.025, 0.03, 0.035};
        YieldCurve yield_curve(maturities, rates);

        BondPriceCalculator calculator(face_value, coupon_rate, years_to_maturity, 
                                       coupons_per_year, yield_curve, num_simulations);

        double static_price = calculator.calculateStaticPrice();
        auto [mc_price, mc_std_dev] = calculator.calculateMonteCarlo();

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nResults:\n";
        std::cout << "Static Bond Price: $" << static_price << std::endl;
        std::cout << "Monte Carlo Bond Price: $" << mc_price << " ± $" << mc_std_dev << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
