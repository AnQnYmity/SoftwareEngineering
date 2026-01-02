#include <iostream>
#include <memory>
#include <ctime>
#include "../include/controller/TransactionController.h"
#include "../include/storage/FileStorage.h"
#include "../include/services/StatisticsService.h"
#include "../include/services/NotificationService.h"
#include "../include/services/ImportExportService.h"

void testDefects() {
    // 1. Memory Leak
    int* leak = new int[10];
    leak[0] = 1;
    // No delete[] leak;

    // 2. Double Free
    int* ptr = new int(5);
    delete ptr;
    delete ptr; // Double free

    // 3. Null Pointer Dereference
    int* nullPtr = nullptr;
    *nullPtr = 10; // Dereference
}

void printMenu() {
    std::cout << "\n====== 记账本系统 ======\n";
    std::cout << "1. 添加交易\n";
    std::cout << "2. 查看所有交易\n";
    std::cout << "3. 编辑交易\n";
    std::cout << "4. 删除交易\n";
    std::cout << "5. 搜索交易\n";
    std::cout << "6. 查看月度统计\n";
    std::cout << "7. 查看分类统计\n";
    std::cout << "8. 导出为JSON\n";
    std::cout << "9. 导出为CSV\n";
    std::cout << "10. 查看提醒\n";
    std::cout << "0. 退出\n";
    std::cout << "请选择: ";
}

void addTransaction(TransactionController& controller) {
    double amount;
    int type;
    std::string categoryId, note;

    std::cout << "请输入金额: ";
    std::cin >> amount;

    std::cout << "交易类型 (0=支出, 1=收入): ";
    std::cin >> type;

    std::cin.ignore();
    std::cout << "请输入分类ID: ";
    std::getline(std::cin, categoryId);

    std::cout << "请输入备注: ";
    std::getline(std::cin, note);

    TransactionDTO dto;
    dto.amount = amount;
    dto.type = (type == 1) ? TransactionType::INCOME : TransactionType::EXPENSE;
    dto.date = time(nullptr);
    dto.categoryId = categoryId;
    dto.note = note;

    try {
        Transaction tx = controller.create(dto);
        std::cout << "\n✓ 交易添加成功! ID: " << tx.id << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void viewAllTransactions(TransactionController& controller) {
    try {
        auto transactions = controller.getAll();
        if (transactions.empty()) {
            std::cout << "\n当前没有交易记录\n";
            return;
        }

        std::cout << "\n====== 所有交易 ======\n";
        std::cout << "ID | 金额 | 类型 | 分类 | 备注\n";
        std::cout << "----------------------------------------\n";

        for (const auto& tx : transactions) {
            std::cout << tx.id << " | "
                      << tx.amount << " | "
                      << (tx.type == TransactionType::INCOME ? "收入" : "支出") << " | "
                      << tx.categoryId << " | "
                      << tx.note << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void viewMonthlyStats(TransactionController& controller) {
    try {
        time_t now = time(nullptr);
        DateRange range;
        range.from = 0;
        range.to = now;

        auto stats = controller.getMonthlyTotals(range);
        std::cout << "\n====== 月度统计 ======\n";

        for (const auto& [month, total] : stats) {
            std::cout << month << ": " << total << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void viewCategoryStats(TransactionController& controller) {
    try {
        time_t now = time(nullptr);
        DateRange range;
        range.from = 0;
        range.to = now;

        auto stats = controller.getCategoryBreakdown(range);
        std::cout << "\n====== 分类统计 ======\n";

        for (const auto& [category, amount] : stats) {
            std::cout << category << ": " << amount << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void exportJSON(TransactionController& controller) {
    try {
        std::string json = controller.exportJSON();
        std::cout << "\n====== JSON 导出 ======\n";
        std::cout << json << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void exportCSV(TransactionController& controller) {
    try {
        std::string csv = controller.exportCSV();
        std::cout << "\n====== CSV 导出 ======\n";
        std::cout << csv << std::endl;
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

void viewNotifications(TransactionController& controller) {
    try {
        auto notifications = controller.getNotifications();
        std::cout << "\n====== 提醒 ======\n";

        if (notifications.empty()) {
            std::cout << "没有提醒\n";
            return;
        }

        for (const auto& notif : notifications) {
            std::cout << "[" << notif.type << "] " << notif.message << "\n";
        }
    } catch (const std::exception& e) {
        std::cout << "\n✗ 错误: " << e.what() << std::endl;
    }
}

int main() {
    testDefects();
    try {
        // Initialize storage and services
        auto storage = std::make_shared<FileStorage>("data");
        auto repository = std::make_shared<TransactionRepository>(storage);
        auto settings = std::make_shared<Settings>("CNY", 5000.0);
        auto statisticsService = std::make_shared<StatisticsService>(repository);
        auto notificationService = std::make_shared<NotificationService>(repository, settings);
        auto importExportService = std::make_shared<ImportExportService>(repository);

        TransactionController controller(repository, statisticsService, 
                                        notificationService, importExportService);

        // Register notification listener
        controller.registerNotificationListener([](const Notification& notif) {
            std::cout << "\n[提醒] " << notif.message << std::endl;
        });

        int choice;
        while (true) {
            printMenu();
            std::cin >> choice;

            switch (choice) {
                case 1:
                    addTransaction(controller);
                    break;
                case 2:
                    viewAllTransactions(controller);
                    break;
                case 3:
                    std::cout << "编辑功能 - 待实现\n";
                    break;
                case 4:
                    std::cout << "删除功能 - 待实现\n";
                    break;
                case 5:
                    std::cout << "搜索功能 - 待实现\n";
                    break;
                case 6:
                    viewMonthlyStats(controller);
                    break;
                case 7:
                    viewCategoryStats(controller);
                    break;
                case 8:
                    exportJSON(controller);
                    break;
                case 9:
                    exportCSV(controller);
                    break;
                case 10:
                    viewNotifications(controller);
                    break;
                case 0:
                    std::cout << "退出程序\n";
                    return 0;
                default:
                    std::cout << "无效的选择\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "致命错误: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
