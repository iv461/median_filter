#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <random>
#include <chrono>

using std::multiset;
using std::less;
using std::greater;
using std::vector;
using std::begin;
using std::end;
using std::cout;
using std::endl;
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
using std::generate_n;

using num_t = double;
using in_vector_t = vector<num_t>;

vector<double> some_vals = {
    3.67764, 3.94947, 4.12311,
    3.74845, 3.75832, 3.4121,
    3.43536, 3.16239, 3.16739,
    3.40006, 3.40331, 3.22619,
    3.26523, 3.31097, 3.26901,
    3.60902, 3.37824, 2.95,
    3.2196, 3.18416, 3.23608,
    3.35485, 2.93695, 2.9904,
    2.9412, 3.13396, 2.96902,
    3.00832
};

template<typename T>
void n_median_filter(vector<T> const &in_values,
                     vector<T> &out_values, size_t window_size) {
    if(in_values.empty() || window_size == 0) {
        return;
    }
    auto in_size = in_values.size();
    out_values.resize(in_size);
    multiset<T, greater<T>> upper;
    multiset<T, less<T>> lower;

    auto const half_window_size = std::min((window_size / 2 + 1), in_size);
    for(size_t iValue = 0; iValue < (in_size + half_window_size - 1); iValue++) {
        if(iValue >= window_size) {
            auto const elem_to_rem = in_values[iValue - window_size];
            auto const its = lower.equal_range(elem_to_rem);
            if(its.first != its.second) {
                lower.erase(its.first);
            } else {
                auto its = upper.equal_range(elem_to_rem);
                if(its.first != its.second) {
                    upper.erase(its.first);
                }
            }
        }
        if(iValue < in_size) {
            auto const val = in_values[iValue];
            if(upper.empty()) {
                upper.insert(val);
            } else {
                auto const min_upper = *upper.crbegin();
                if(val >= min_upper) {
                    upper.insert(val);
                } else {
                    lower.insert(val);
                }
            }
        }
        if(upper.size() > lower.size()) {
            while(upper.size() - lower.size() > 1) {
                auto const upper_lowest = *upper.crbegin();
                lower.insert(upper_lowest);
                upper.erase(upper.equal_range(upper_lowest).first);
            }
        } else if(lower.size() > upper.size()) {
            while(lower.size() - upper.size() > 1) {
                auto const lower_highest = *lower.crbegin();
                upper.insert(lower_highest);
                lower.erase(lower.equal_range(lower_highest).first);
            }
        }
        if(iValue + 1 >= half_window_size) {
            T median;
            auto const min_upper = *upper.crbegin();
            if(upper.size() == lower.size()) {
                auto const max_lower = *lower.crbegin();
                median = static_cast<T>(.5 * (min_upper + max_lower));
            } else if(upper.size() > lower.size()) {
                median = min_upper;
            } else {
                auto const max_lower = *lower.crbegin();
                median = max_lower;
            }
            out_values[iValue + 1 - half_window_size] = median;
        }
    }
}

int main() {
    in_vector_t filtered_vector;

    auto const window_size = 12;
    auto const number_of_elements = 1000;

    auto const &in_vector = some_vals;
    vector<double> filtered_samples;
    n_median_filter(in_vector, filtered_samples, window_size);

    cout << "filtered samples:\n";
    for(size_t iSample = 0; iSample < filtered_samples.size(); iSample++) {
        cout << filtered_samples[iSample];
        if(iSample + 1 != filtered_samples.size()) {
            cout << ", ";
        }
    }
    cout << "\n";

    random_device rd;
    mt19937 eng(rd());
    uniform_real_distribution<num_t> distr(-100, 100);

    in_vector_t rnd_vector;
    rnd_vector.resize(number_of_elements);

    generate_n(begin(rnd_vector), number_of_elements,
               [&]() { return distr(eng); });

    auto start_filter = std::chrono::steady_clock::now();

    n_median_filter(rnd_vector, filtered_vector, window_size);

    auto end_filter = std::chrono::steady_clock::now();
    auto time_filter = std::chrono::duration <double,
            std::milli>(end_filter - start_filter).count();

    cout << "filtering " << number_of_elements <<
            " random values with window size "
         << window_size << " took "
         << time_filter << " ms";

    getchar();
    return 0;
}
