#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <inttypes.h>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <exception>
#include <stdexcept>
#include <sstream>
#include <iterator>

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

std::vector<std::string> split_string(const std::string& str) {
    std::stringstream stream(str);

    std::string word;
    std::vector<std::string> res;

    while (stream >> word) {
        res.push_back(word);
    }

    return res;
}

class index_reader {
public:
    index_reader()
        : index("index")
    {
        post_constructor();
    }

public:
    std::vector<uint32_t> get_word_posting_list(const std::string& word) const {
        const auto it = word_to_offset_and_length.find(word);
        if (it == word_to_offset_and_length.end()) {
            return {};
        }

        const std::pair<uint32_t, uint32_t> offset_and_length = it->second;

        std::vector<char> res(offset_and_length.second / sizeof(char));

        index.clear();
        index.seekg(offset_and_length.first);
        index.read(reinterpret_cast<char*>(res.data()),
                   offset_and_length.second);

        auto decoded = decode<uint32_t>(res.data(), res.size());

        for (size_t i = 1; i < decoded.size(); ++i) {
            decoded[i] += decoded[i - 1];
        }

        return decoded;
    }

    std::vector<uint32_t> intersect_posting_list(const std::vector<std::string>& words) const {
        if (words.empty()) {
            return {};
        }

        std::vector<uint32_t> res = get_word_posting_list(words[0]);

        for (size_t i = 1; i < words.size(); ++i) {
            const std::vector<uint32_t> posting = get_word_posting_list(words[i]);

            std::vector<uint32_t> intersection;
            std::set_intersection(
                        res.begin(), res.end(),
                        posting.begin(), posting.end(),
                        std::back_inserter(intersection));
            res.swap(intersection);
        }

        return res;
    }

    std::vector<std::string> search(const std::string& query) const {
        const std::vector<uint32_t> docids =
                intersect_posting_list(split_string(query));

        std::vector<std::string> res;
        for (const uint32_t docid : docids) {
            res.push_back(docid_to_url.at(docid));
        }

        return res;
    }

private:
    void post_constructor() {
        {
            uint32_t docid = 0;
            std::string url;

            std::ifstream input("docid_to_url");
            while (input >> docid >> url) {
                docid_to_url[docid] = url;
            }
        }
        {
            std::string word;
            std::pair<uint32_t, uint32_t> offset_and_length;

            std::ifstream input("word_to_offset_and_length");
            while (input >> word >> offset_and_length.first >> offset_and_length.second) {
                word_to_offset_and_length[word] = offset_and_length;
            }
        }
    }

private:
    mutable std::ifstream index;
    std::unordered_map<uint32_t, std::string> docid_to_url;
    std::unordered_map<std::string, std::pair<uint32_t, uint32_t>> word_to_offset_and_length;
};

int main(void) {
    index_reader index;

    std::cout << "Enter your queries:" << std::endl;

    std::string query;
    while (std::getline(std::cin, query)) {
        const auto search_result = index.search(query);
        for (const std::string& url : search_result) {
            std::cout << " -- " << url << '\n';
        }
        std::cout << " " << search_result.size() << " total\n";
    }

    return 0;
}
