### 1D Median filter

Simple 1D median filter with a test application and small benchmark, implemented with ```std::multiset```.

Compile for example on Linux with:

```
g++ median_filter.cpp -std=c++11 -o median_filter
```

Output is verified against MATLAB ```smoothdata``` function, it works for window size > 0,
for window size >= 2 * input length it returns for every output value the median of the whole input vector.
