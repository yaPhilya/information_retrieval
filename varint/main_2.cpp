#include <iostream>
#include <vector>
#include <fstream>
#include <random>


template<typename T>
std::vector<char> encode(const T* data, size_t count) {
    std::vector<char> result = std::vector<char>();

    for (auto i = 0; i < count; ++i) {
        T number = data[i];
        while (number) {
            char c = 0;
            if (number >> 7) {
                c |= 1 << 7;
            }
            c |= number & ((1 << 7) - 1);
            result.push_back(c);

            number = number >> 7;
        }
    }
    return result;
}

template<typename T>
std::vector<T> decode(const char* data, size_t length){
    std::vector<T> result = std::vector<T>();

    size_t position = 0;
    while (position < length) {
        T number = 0;
        int pow = 0;
        do {
            number |= (T)(data[position] & ((1 << 7) - 1)) << pow;
            pow += 7;
            position++;
        } while (data[position - 1] >> 7);

        result.push_back(number);
    }

    return result;
}

int main() {
    std::ifstream inp("input.txt");
    unsigned long long number = 0;
    std::vector<unsigned long long> vec;
    while (inp >> number) {
        vec.push_back(number);
    }
    auto encoded = encode<unsigned long long>(vec.data(), vec.size());
    
    std::cout << (double)(sizeof(unsigned long long) * vec.size()) / (double)(encoded.size() * sizeof(char));
    return 0;
}