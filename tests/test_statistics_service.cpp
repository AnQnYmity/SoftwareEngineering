#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/services/StatisticsService.h"
#include "../include/storage/TransactionRepository.h"
#include "../include/storage/IStorage.h"
#include <memory>

// Mock Storage class
class MockStorage : public IStorage {
public:
    MOCK_METHOD(void, save, (const std::string& key, const std::string& data), (override));
    MOCK_METHOD(std::string, load, (const std::string& key), (override));
    MOCK_METHOD(std::string, backup, (), (override));
    MOCK_METHOD(bool, exists, (const std::string& key), (override));
    MOCK_METHOD(void, remove, (const std::string& key), (override));
};

// Test fixture for StatisticsService
class StatisticsServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockStorage> mockStorage;
    std::shared_ptr<TransactionRepository> repository;
    std::shared_ptr<StatisticsService> service;

    void SetUp() override {
        mockStorage = std::make_shared<MockStorage>();
        EXPECT_CALL(*mockStorage, load(testing::_))
            .WillRepeatedly(testing::Return("[]"));
        EXPECT_CALL(*mockStorage, save(testing::_, testing::_))
            .WillRepeatedly(testing::Return());
        
        repository = std::make_shared<TransactionRepository>(mockStorage);
        service = std::make_shared<StatisticsService>(repository);
    }

    void TearDown() override {
        service.reset();
        repository.reset();
        mockStorage.reset();
    }

    // Helper function to create a transaction
    void addTransaction(double amount, TransactionType type, const std::string& category, time_t date) {
        Transaction tx;
        tx.amount = amount;
        tx.type = type;
        tx.categoryId = category;
        tx.note = "Test transaction";
        tx.date = date;
        repository->add(tx);
    }

    // Helper to get time for specific date
    time_t getDate(int year, int month, int day) {
        struct tm timeinfo = {};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = 12;
        return mktime(&timeinfo);
    }
};

// Test Case 1: Calculate total income with no transactions
TEST_F(StatisticsServiceTest, GetTotalIncomeEmpty) {
    DateRange range;
    range.from = 0;
    range.to = time(nullptr);

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 0.0);
}

// Test Case 2: Calculate total income with single income transaction
TEST_F(StatisticsServiceTest, GetTotalIncomeSingle) {
    time_t now = time(nullptr);
    addTransaction(1000.0, TransactionType::INCOME, "salary", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 1000.0);
}

// Test Case 3: Calculate total income with multiple income transactions
TEST_F(StatisticsServiceTest, GetTotalIncomeMultiple) {
    time_t now = time(nullptr);
    addTransaction(1000.0, TransactionType::INCOME, "salary", now);
    addTransaction(500.0, TransactionType::INCOME, "bonus", now);
    addTransaction(200.0, TransactionType::INCOME, "freelance", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 1700.0);
}

// Test Case 4: Calculate total income ignoring expenses
TEST_F(StatisticsServiceTest, GetTotalIncomeIgnoreExpenses) {
    time_t now = time(nullptr);
    addTransaction(1000.0, TransactionType::INCOME, "salary", now);
    addTransaction(200.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 1000.0);
}

// Test Case 5: Calculate total expense with no transactions
TEST_F(StatisticsServiceTest, GetTotalExpenseEmpty) {
    DateRange range;
    range.from = 0;
    range.to = time(nullptr);

    double total = service->getTotalExpense(range);

    EXPECT_EQ(total, 0.0);
}

// Test Case 6: Calculate total expense with single expense transaction
TEST_F(StatisticsServiceTest, GetTotalExpenseSingle) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalExpense(range);

    EXPECT_EQ(total, 100.0);
}

// Test Case 7: Calculate total expense with multiple expense transactions
TEST_F(StatisticsServiceTest, GetTotalExpenseMultiple) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);
    addTransaction(50.0, TransactionType::EXPENSE, "transport", now);
    addTransaction(200.0, TransactionType::EXPENSE, "shopping", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalExpense(range);

    EXPECT_EQ(total, 350.0);
}

// Test Case 8: Calculate total expense ignoring income
TEST_F(StatisticsServiceTest, GetTotalExpenseIgnoreIncome) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);
    addTransaction(1000.0, TransactionType::INCOME, "salary", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double total = service->getTotalExpense(range);

    EXPECT_EQ(total, 100.0);
}

// Test Case 9: Date range filtering - transactions within range
TEST_F(StatisticsServiceTest, GetTotalIncomeWithinDateRange) {
    time_t day1 = getDate(2024, 1, 1);
    time_t day5 = getDate(2024, 1, 5);
    time_t day10 = getDate(2024, 1, 10);
    time_t day15 = getDate(2024, 1, 15);

    addTransaction(100.0, TransactionType::INCOME, "salary", day1);
    addTransaction(200.0, TransactionType::INCOME, "bonus", day5);
    addTransaction(300.0, TransactionType::INCOME, "freelance", day10);
    addTransaction(400.0, TransactionType::INCOME, "investment", day15);

    DateRange range;
    range.from = day5;
    range.to = day10;

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 500.0); // Only day5 and day10
}

// Test Case 10: Date range filtering - transactions outside range
TEST_F(StatisticsServiceTest, GetTotalIncomeOutsideDateRange) {
    time_t day1 = getDate(2024, 1, 1);
    time_t day15 = getDate(2024, 1, 15);
    time_t day30 = getDate(2024, 1, 30);

    addTransaction(100.0, TransactionType::INCOME, "salary", day1);
    addTransaction(200.0, TransactionType::INCOME, "bonus", day30);

    DateRange range;
    range.from = getDate(2024, 1, 10);
    range.to = getDate(2024, 1, 20);

    double total = service->getTotalIncome(range);

    EXPECT_EQ(total, 0.0); // No transactions in range
}

// Test Case 11: Category breakdown with empty transactions
TEST_F(StatisticsServiceTest, CategoryBreakdownEmpty) {
    DateRange range;
    range.from = 0;
    range.to = time(nullptr);

    auto breakdown = service->categoryBreakdown(range);

    EXPECT_TRUE(breakdown.empty());
}

// Test Case 12: Category breakdown with single category
TEST_F(StatisticsServiceTest, CategoryBreakdownSingleCategory) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);
    addTransaction(50.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    auto breakdown = service->categoryBreakdown(range);

    EXPECT_EQ(breakdown.size(), 1);
    EXPECT_EQ(breakdown["food"], 150.0);
}

// Test Case 13: Category breakdown with multiple categories
TEST_F(StatisticsServiceTest, CategoryBreakdownMultipleCategories) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);
    addTransaction(50.0, TransactionType::EXPENSE, "transport", now);
    addTransaction(200.0, TransactionType::EXPENSE, "shopping", now);
    addTransaction(75.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    auto breakdown = service->categoryBreakdown(range);

    EXPECT_EQ(breakdown.size(), 3);
    EXPECT_EQ(breakdown["food"], 175.0);
    EXPECT_EQ(breakdown["transport"], 50.0);
    EXPECT_EQ(breakdown["shopping"], 200.0);
}

// Test Case 14: Category breakdown ignores income
TEST_F(StatisticsServiceTest, CategoryBreakdownIgnoreIncome) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "food", now);
    addTransaction(1000.0, TransactionType::INCOME, "salary", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    auto breakdown = service->categoryBreakdown(range);

    EXPECT_EQ(breakdown.size(), 1);
    EXPECT_EQ(breakdown.count("salary"), 0);
}

// Test Case 15: Monthly totals calculation
TEST_F(StatisticsServiceTest, CalculateMonthlyTotals) {
    time_t jan1 = getDate(2024, 1, 15);
    time_t jan2 = getDate(2024, 1, 20);
    time_t feb1 = getDate(2024, 2, 10);

    addTransaction(1000.0, TransactionType::INCOME, "salary", jan1);
    addTransaction(200.0, TransactionType::EXPENSE, "food", jan2);
    addTransaction(500.0, TransactionType::INCOME, "bonus", feb1);

    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 2, 28);

    auto monthlyTotals = service->calculateMonthlyTotals(range);

    EXPECT_EQ(monthlyTotals.size(), 2);
    EXPECT_EQ(monthlyTotals["2024-01"], 800.0); // 1000 - 200
    EXPECT_EQ(monthlyTotals["2024-02"], 500.0); // 500
}

// Test Case 16: Asset trend calculation
TEST_F(StatisticsServiceTest, AssetTrendCalculation) {
    time_t day1 = getDate(2024, 1, 1);
    time_t day2 = getDate(2024, 1, 2);
    time_t day3 = getDate(2024, 1, 3);

    addTransaction(1000.0, TransactionType::INCOME, "salary", day1);
    addTransaction(200.0, TransactionType::EXPENSE, "food", day2);
    addTransaction(500.0, TransactionType::INCOME, "bonus", day3);

    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 1, 31);

    auto trend = service->assetTrend(range);

    EXPECT_EQ(trend.size(), 3);
    EXPECT_EQ(trend[day1], 1000.0);
    EXPECT_EQ(trend[day2], 800.0);
    EXPECT_EQ(trend[day3], 1300.0);
}

// Test Case 17: Zero amount transactions
TEST_F(StatisticsServiceTest, HandleZeroAmountTransactions) {
    time_t now = time(nullptr);
    addTransaction(0.0, TransactionType::INCOME, "salary", now);
    addTransaction(0.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double totalIncome = service->getTotalIncome(range);
    double totalExpense = service->getTotalExpense(range);

    EXPECT_EQ(totalIncome, 0.0);
    EXPECT_EQ(totalExpense, 0.0);
}

// Test Case 18: Large amount handling
TEST_F(StatisticsServiceTest, HandleLargeAmounts) {
    time_t now = time(nullptr);
    addTransaction(999999999.99, TransactionType::INCOME, "investment", now);
    addTransaction(888888888.88, TransactionType::EXPENSE, "purchase", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    double totalIncome = service->getTotalIncome(range);
    double totalExpense = service->getTotalExpense(range);

    EXPECT_DOUBLE_EQ(totalIncome, 999999999.99);
    EXPECT_DOUBLE_EQ(totalExpense, 888888888.88);
}

// Test Case 19: Boundary date range (from = to)
TEST_F(StatisticsServiceTest, BoundaryDateRangeSameDay) {
    time_t specificDate = getDate(2024, 1, 15);
    
    addTransaction(100.0, TransactionType::INCOME, "salary", specificDate);
    addTransaction(50.0, TransactionType::EXPENSE, "food", getDate(2024, 1, 14));
    addTransaction(75.0, TransactionType::EXPENSE, "transport", getDate(2024, 1, 16));

    DateRange range;
    range.from = specificDate;
    range.to = specificDate;

    double totalIncome = service->getTotalIncome(range);

    EXPECT_EQ(totalIncome, 100.0);
}

// Test Case 20: Empty category ID handling
TEST_F(StatisticsServiceTest, HandleEmptyCategoryId) {
    time_t now = time(nullptr);
    addTransaction(100.0, TransactionType::EXPENSE, "", now);
    addTransaction(200.0, TransactionType::EXPENSE, "food", now);

    DateRange range;
    range.from = 0;
    range.to = time(nullptr) + 1000;

    auto breakdown = service->categoryBreakdown(range);

    EXPECT_EQ(breakdown.size(), 2);
    EXPECT_EQ(breakdown[""], 100.0);
    EXPECT_EQ(breakdown["food"], 200.0);
}
