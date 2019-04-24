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


template<typename T>
bool test(size_t count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> dis;

    std::vector<T> data(count);
    for (size_t i = 0; i < count; ++i) {
        data[i] = dis(gen);
    }

    auto encoded = encode<T>(data.data(), count);

    auto decoded = decode<T>(encoded.data(), encoded.size());

    if (decoded.size() != count) {
        return false;
    }

    for (auto k = 0; k < count; ++k) {
        if (decoded[k] != data[k]) {
            return false;
        }
    }
    return true;
}

int main() {
    if (test<unsigned int>(1000000)) {
       printf("OK\n");
    } else {
       printf("WRONG\n");
    }

    if (test<unsigned long long>(1000000)) {
       printf("OK\n");
    } else {
       printf("WRONG\n");
    }
    return 0;
}