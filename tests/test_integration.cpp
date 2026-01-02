#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/storage/TransactionRepository.h"
#include "../include/services/StatisticsService.h"
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

// Integration Test Suite 1: Repository and Statistics Integration
class RepositoryStatisticsIntegrationTest : public ::testing::Test {
protected:
    std::shared_ptr<MockStorage> mockStorage;
    std::shared_ptr<TransactionRepository> repository;
    std::shared_ptr<StatisticsService> statisticsService;

    void SetUp() override {
        mockStorage = std::make_shared<MockStorage>();
        EXPECT_CALL(*mockStorage, load(testing::_))
            .WillRepeatedly(testing::Return("[]"));
        EXPECT_CALL(*mockStorage, save(testing::_, testing::_))
            .WillRepeatedly(testing::Return());
        
        repository = std::make_shared<TransactionRepository>(mockStorage);
        statisticsService = std::make_shared<StatisticsService>(repository);
    }

    void TearDown() override {
        statisticsService.reset();
        repository.reset();
        mockStorage.reset();
    }

    time_t getDate(int year, int month, int day) {
        struct tm timeinfo = {};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = 12;
        return mktime(&timeinfo);
    }
};

// Integration Test 1: Complete workflow - Add transactions and calculate statistics
TEST_F(RepositoryStatisticsIntegrationTest, CompleteWorkflowAddAndCalculate) {
    // Step 1: Add income transactions
    Transaction income1;
    income1.amount = 5000.0;
    income1.type = TransactionType::INCOME;
    income1.categoryId = "salary";
    income1.note = "Monthly salary";
    income1.date = getDate(2024, 1, 1);
    repository->add(income1);

    Transaction income2;
    income2.amount = 1000.0;
    income2.type = TransactionType::INCOME;
    income2.categoryId = "bonus";
    income2.note = "Year-end bonus";
    income2.date = getDate(2024, 1, 15);
    repository->add(income2);

    // Step 2: Add expense transactions
    Transaction expense1;
    expense1.amount = 500.0;
    expense1.type = TransactionType::EXPENSE;
    expense1.categoryId = "food";
    expense1.note = "Groceries";
    expense1.date = getDate(2024, 1, 5);
    repository->add(expense1);

    Transaction expense2;
    expense2.amount = 300.0;
    expense2.type = TransactionType::EXPENSE;
    expense2.categoryId = "transport";
    expense2.note = "Gas";
    expense2.date = getDate(2024, 1, 10);
    repository->add(expense2);

    Transaction expense3;
    expense3.amount = 200.0;
    expense3.type = TransactionType::EXPENSE;
    expense3.categoryId = "food";
    expense3.note = "Restaurant";
    expense3.date = getDate(2024, 1, 20);
    repository->add(expense3);

    // Step 3: Calculate statistics
    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 1, 31);

    // Verify total income
    double totalIncome = statisticsService->getTotalIncome(range);
    EXPECT_EQ(totalIncome, 6000.0);

    // Verify total expense
    double totalExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(totalExpense, 1000.0);

    // Verify category breakdown
    auto breakdown = statisticsService->categoryBreakdown(range);
    EXPECT_EQ(breakdown.size(), 2);
    EXPECT_EQ(breakdown["food"], 700.0);
    EXPECT_EQ(breakdown["transport"], 300.0);

    // Verify monthly totals
    auto monthlyTotals = statisticsService->calculateMonthlyTotals(range);
    EXPECT_EQ(monthlyTotals["2024-01"], 5000.0); // 6000 - 1000
}

// Integration Test 2: Update transactions and verify statistics change
TEST_F(RepositoryStatisticsIntegrationTest, UpdateTransactionsAndVerifyStatistics) {
    // Step 1: Add initial transactions
    Transaction tx1;
    tx1.amount = 1000.0;
    tx1.type = TransactionType::INCOME;
    tx1.categoryId = "salary";
    tx1.note = "Initial salary";
    tx1.date = getDate(2024, 1, 1);
    Transaction addedTx1 = repository->add(tx1);

    Transaction tx2;
    tx2.amount = 200.0;
    tx2.type = TransactionType::EXPENSE;
    tx2.categoryId = "food";
    tx2.note = "Groceries";
    tx2.date = getDate(2024, 1, 5);
    Transaction addedTx2 = repository->add(tx2);

    // Step 2: Calculate initial statistics
    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 1, 31);

    double initialIncome = statisticsService->getTotalIncome(range);
    double initialExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(initialIncome, 1000.0);
    EXPECT_EQ(initialExpense, 200.0);

    // Step 3: Update transactions
    addedTx1.amount = 1500.0; // Increase income
    repository->update(addedTx1);

    addedTx2.amount = 300.0; // Increase expense
    repository->update(addedTx2);

    // Step 4: Verify statistics changed
    double updatedIncome = statisticsService->getTotalIncome(range);
    double updatedExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(updatedIncome, 1500.0);
    EXPECT_EQ(updatedExpense, 300.0);

    // Verify monthly totals updated
    auto monthlyTotals = statisticsService->calculateMonthlyTotals(range);
    EXPECT_EQ(monthlyTotals["2024-01"], 1200.0); // 1500 - 300
}

// Integration Test 3: Delete transactions and verify statistics
TEST_F(RepositoryStatisticsIntegrationTest, DeleteTransactionsAndVerifyStatistics) {
    // Step 1: Add transactions
    Transaction tx1;
    tx1.amount = 1000.0;
    tx1.type = TransactionType::INCOME;
    tx1.categoryId = "salary";
    tx1.date = getDate(2024, 1, 1);
    Transaction added1 = repository->add(tx1);

    Transaction tx2;
    tx2.amount = 500.0;
    tx2.type = TransactionType::EXPENSE;
    tx2.categoryId = "food";
    tx2.date = getDate(2024, 1, 5);
    Transaction added2 = repository->add(tx2);

    Transaction tx3;
    tx3.amount = 300.0;
    tx3.type = TransactionType::EXPENSE;
    tx3.categoryId = "transport";
    tx3.date = getDate(2024, 1, 10);
    Transaction added3 = repository->add(tx3);

    // Step 2: Calculate initial statistics
    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 1, 31);

    double initialExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(initialExpense, 800.0);

    // Step 3: Delete a transaction
    repository->remove(added2.id);

    // Step 4: Verify statistics updated
    double updatedExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(updatedExpense, 300.0); // Only transport left

    // Verify category breakdown updated
    auto breakdown = statisticsService->categoryBreakdown(range);
    EXPECT_EQ(breakdown.size(), 1);
    EXPECT_EQ(breakdown.count("food"), 0); // Food category should be gone
    EXPECT_EQ(breakdown["transport"], 300.0);
}

// Integration Test 4: Filter transactions and calculate statistics
TEST_F(RepositoryStatisticsIntegrationTest, FilterTransactionsAndCalculateStatistics) {
    // Add transactions across multiple months
    Transaction jan1;
    jan1.amount = 1000.0;
    jan1.type = TransactionType::INCOME;
    jan1.categoryId = "salary";
    jan1.date = getDate(2024, 1, 15);
    repository->add(jan1);

    Transaction jan2;
    jan2.amount = 200.0;
    jan2.type = TransactionType::EXPENSE;
    jan2.categoryId = "food";
    jan2.date = getDate(2024, 1, 20);
    repository->add(jan2);

    Transaction feb1;
    feb1.amount = 1000.0;
    feb1.type = TransactionType::INCOME;
    feb1.categoryId = "salary";
    feb1.date = getDate(2024, 2, 15);
    repository->add(feb1);

    Transaction feb2;
    feb2.amount = 300.0;
    feb2.type = TransactionType::EXPENSE;
    feb2.categoryId = "food";
    feb2.date = getDate(2024, 2, 20);
    repository->add(feb2);

    // Filter for January only
    TransactionFilter filter;
    filter.dateFrom = getDate(2024, 1, 1);
    filter.dateTo = getDate(2024, 1, 31);
    
    auto januaryTransactions = repository->find(filter);
    EXPECT_EQ(januaryTransactions.size(), 2);

    // Calculate statistics for January
    DateRange janRange;
    janRange.from = getDate(2024, 1, 1);
    janRange.to = getDate(2024, 1, 31);

    double janIncome = statisticsService->getTotalIncome(janRange);
    double janExpense = statisticsService->getTotalExpense(janRange);
    EXPECT_EQ(janIncome, 1000.0);
    EXPECT_EQ(janExpense, 200.0);

    // Calculate statistics for February
    DateRange febRange;
    febRange.from = getDate(2024, 2, 1);
    febRange.to = getDate(2024, 2, 29);

    double febIncome = statisticsService->getTotalIncome(febRange);
    double febExpense = statisticsService->getTotalExpense(febRange);
    EXPECT_EQ(febIncome, 1000.0);
    EXPECT_EQ(febExpense, 300.0);
}

// Integration Test Suite 2: End-to-End Business Scenario Testing
class EndToEndBusinessScenarioTest : public ::testing::Test {
protected:
    std::shared_ptr<MockStorage> mockStorage;
    std::shared_ptr<TransactionRepository> repository;
    std::shared_ptr<StatisticsService> statisticsService;

    void SetUp() override {
        mockStorage = std::make_shared<MockStorage>();
        EXPECT_CALL(*mockStorage, load(testing::_))
            .WillRepeatedly(testing::Return("[]"));
        EXPECT_CALL(*mockStorage, save(testing::_, testing::_))
            .WillRepeatedly(testing::Return());
        
        repository = std::make_shared<TransactionRepository>(mockStorage);
        statisticsService = std::make_shared<StatisticsService>(repository);
    }

    time_t getDate(int year, int month, int day) {
        struct tm timeinfo = {};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = 12;
        return mktime(&timeinfo);
    }
};

// Business Scenario 1: Monthly budget tracking
TEST_F(EndToEndBusinessScenarioTest, MonthlyBudgetTracking) {
    // Scenario: User receives salary and tracks monthly expenses
    
    // Receive monthly salary
    Transaction salary;
    salary.amount = 5000.0;
    salary.type = TransactionType::INCOME;
    salary.categoryId = "salary";
    salary.note = "January salary";
    salary.date = getDate(2024, 1, 1);
    repository->add(salary);

    // Track various expenses throughout the month
    struct ExpenseRecord {
        double amount;
        std::string category;
        std::string note;
        int day;
    };

    std::vector<ExpenseRecord> expenses = {
        {500.0, "food", "Groceries week 1", 3},
        {200.0, "transport", "Gas", 5},
        {100.0, "entertainment", "Movie tickets", 7},
        {500.0, "food", "Groceries week 2", 10},
        {150.0, "utilities", "Electricity bill", 12},
        {300.0, "shopping", "Clothes", 15},
        {500.0, "food", "Groceries week 3", 17},
        {200.0, "transport", "Car maintenance", 20},
        {500.0, "food", "Groceries week 4", 24},
        {100.0, "entertainment", "Concert", 28}
    };

    for (const auto& expense : expenses) {
        Transaction tx;
        tx.amount = expense.amount;
        tx.type = TransactionType::EXPENSE;
        tx.categoryId = expense.category;
        tx.note = expense.note;
        tx.date = getDate(2024, 1, expense.day);
        repository->add(tx);
    }

    // Calculate monthly statistics
    DateRange range;
    range.from = getDate(2024, 1, 1);
    range.to = getDate(2024, 1, 31);

    // Total expense should be sum of all expenses
    double totalExpense = statisticsService->getTotalExpense(range);
    EXPECT_EQ(totalExpense, 3050.0);

    // Remaining budget
    double totalIncome = statisticsService->getTotalIncome(range);
    double remainingBudget = totalIncome - totalExpense;
    EXPECT_EQ(remainingBudget, 1950.0);

    // Category breakdown
    auto breakdown = statisticsService->categoryBreakdown(range);
    EXPECT_EQ(breakdown["food"], 2000.0);
    EXPECT_EQ(breakdown["transport"], 400.0);
    EXPECT_EQ(breakdown["entertainment"], 200.0);
    EXPECT_EQ(breakdown["utilities"], 150.0);
    EXPECT_EQ(breakdown["shopping"], 300.0);
}

// Business Scenario 2: Quarterly financial review
TEST_F(EndToEndBusinessScenarioTest, QuarterlyFinancialReview) {
    // Scenario: Business owner reviews Q1 performance
    
    // January
    Transaction janSales;
    janSales.amount = 10000.0;
    janSales.type = TransactionType::INCOME;
    janSales.categoryId = "sales";
    janSales.date = getDate(2024, 1, 15);
    repository->add(janSales);

    Transaction janExpenses;
    janExpenses.amount = 3000.0;
    janExpenses.type = TransactionType::EXPENSE;
    janExpenses.categoryId = "operating";
    janExpenses.date = getDate(2024, 1, 20);
    repository->add(janExpenses);

    // February
    Transaction febSales;
    febSales.amount = 12000.0;
    febSales.type = TransactionType::INCOME;
    febSales.categoryId = "sales";
    febSales.date = getDate(2024, 2, 15);
    repository->add(febSales);

    Transaction febExpenses;
    febExpenses.amount = 3500.0;
    febExpenses.type = TransactionType::EXPENSE;
    febExpenses.categoryId = "operating";
    febExpenses.date = getDate(2024, 2, 20);
    repository->add(febExpenses);

    // March
    Transaction marSales;
    marSales.amount = 15000.0;
    marSales.type = TransactionType::INCOME;
    marSales.categoryId = "sales";
    marSales.date = getDate(2024, 3, 15);
    repository->add(marSales);

    Transaction marExpenses;
    marExpenses.amount = 4000.0;
    marExpenses.type = TransactionType::EXPENSE;
    marExpenses.categoryId = "operating";
    marExpenses.date = getDate(2024, 3, 20);
    repository->add(marExpenses);

    // Calculate Q1 statistics
    DateRange q1Range;
    q1Range.from = getDate(2024, 1, 1);
    q1Range.to = getDate(2024, 3, 31);

    double q1Income = statisticsService->getTotalIncome(q1Range);
    double q1Expense = statisticsService->getTotalExpense(q1Range);
    double q1Profit = q1Income - q1Expense;

    EXPECT_EQ(q1Income, 37000.0);
    EXPECT_EQ(q1Expense, 10500.0);
    EXPECT_EQ(q1Profit, 26500.0);

    // Monthly breakdown
    auto monthlyTotals = statisticsService->calculateMonthlyTotals(q1Range);
    EXPECT_EQ(monthlyTotals["2024-01"], 7000.0);   // 10000 - 3000
    EXPECT_EQ(monthlyTotals["2024-02"], 8500.0);   // 12000 - 3500
    EXPECT_EQ(monthlyTotals["2024-03"], 11000.0);  // 15000 - 4000
}

// Run all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
