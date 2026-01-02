#include <stdint.h>
#include <stddef.h>
#include <string>
#include <memory>
#include <vector>
#include "../include/services/ImportExportService.h"
#include "../include/storage/TransactionRepository.h"
#include "../include/storage/IStorage.h"

// 定义一个不进行任何实际IO操作的 MockStorage
// 模糊测试需要极高的速度，不能有文件IO
class FuzzMockStorage : public IStorage {
public:
    void save(const std::string& key, const std::string& value) override {
        // Do nothing
    }
    std::string load(const std::string& key) override {
        return "[]";
    }
    std::string backup() override {
        return "";
    }
    bool exists(const std::string& key) override {
        return false;
    }
    void remove(const std::string& key) override {
        // Do nothing
    }
};

// LibFuzzer 的入口点
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    // 1. 将随机字节流转换为字符串
    std::string jsonInput(reinterpret_cast<const char*>(Data), Size);

    // 2. 初始化服务 (使用 Mock 存储)
    auto mockStorage = std::make_shared<FuzzMockStorage>();
    auto repository = std::make_shared<TransactionRepository>(mockStorage);
    ImportExportService service(repository);

    // 3. 调用目标函数进行测试
    // 我们测试 importFromJSON，因为它包含复杂的解析逻辑，最容易出问题
    try {
        service.importFromJSON(jsonInput);
    } catch (...) {
        // 捕获所有异常，防止非崩溃性错误终止测试
        // 我们只关心真正的 Crash (如内存越界、空指针解引用)
    }

    return 0;
}
