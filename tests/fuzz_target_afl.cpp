#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "../include/services/ImportExportService.h"
#include "../include/storage/TransactionRepository.h"
#include "../include/storage/IStorage.h"

// Mock Storage
class FuzzMockStorage : public IStorage {
public:
    void save(const std::string&, const std::string&) override {}
    std::string load(const std::string&) override { return "[]"; }
    std::string backup() override { return ""; }
    bool exists(const std::string&) override { return false; }
    void remove(const std::string&) override {}
};

// 简单的 main 函数，用于 AFL (非 Persistent Mode) 或手动测试
// 如果使用 AFL++ Persistent Mode，需要包含特定的宏，这里为了通用性使用标准 stdin 读取
int main() {
    // 初始化服务
    auto mockStorage = std::make_shared<FuzzMockStorage>();
    auto repository = std::make_shared<TransactionRepository>(mockStorage);
    ImportExportService service(repository);

    // 从 stdin 读取所有数据
    std::string input;
    std::string line;
    while (std::getline(std::cin, line)) {
        input += line + "\n";
    }

    try {
        service.importFromJSON(input);
    } catch (...) {
        // 忽略异常，只关心 Crash
    }

    return 0;
}
