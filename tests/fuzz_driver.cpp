#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <cstdint>

// 声明模糊测试目标函数
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

// 简单的随机数据生成器
std::vector<uint8_t> generate_random_data(size_t max_size) {
    static std::mt19937 rng(std::time(nullptr));
    std::uniform_int_distribution<int> dist_size(1, max_size);
    std::uniform_int_distribution<int> dist_byte(0, 255);

    size_t size = dist_size(rng);
    std::vector<uint8_t> data(size);
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<uint8_t>(dist_byte(rng));
    }
    return data;
}

int main(int argc, char** argv) {
    std::cout << "Running Standalone Fuzzer (MinGW Compatible)..." << std::endl;
    std::cout << "Press Ctrl+C to stop." << std::endl;

    size_t iterations = 0;
    while (true) {
        // 生成随机输入 (模拟模糊测试)
        // 这里的策略很简单，真实的 LibFuzzer 会有更复杂的变异策略
        std::vector<uint8_t> data = generate_random_data(1024);

        // 调用目标函数
        LLVMFuzzerTestOneInput(data.data(), data.size());

        iterations++;
        if (iterations % 1000 == 0) {
            std::cout << "Executed " << iterations << " iterations..." << "\r" << std::flush;
        }
    }

    return 0;
}
