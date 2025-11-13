#include "../include/services/StatisticsService.h"
#include "../include/storage/TransactionRepository.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

StatisticsService::StatisticsService(std::shared_ptr<TransactionRepository> repo)
    : repository(repo) {}

StatisticsService::~StatisticsService() {}

std::string StatisticsService::getMonthKey(time_t timestamp) const {
    struct tm* timeinfo = localtime(&timestamp);
    std::stringstream ss;
    ss << std::put_time(timeinfo, "%Y-%m");
    return ss.str();
}

bool StatisticsService::isInDateRange(time_t date, const DateRange& range) const {
    return (range.from == 0 || date >= range.from) &&
           (range.to == 0 || date <= range.to);
}

std::map<std::string, double> StatisticsService::calculateMonthlyTotals(const DateRange& range) const {
    std::map<std::string, double> result;
    auto transactions = repository->getAll();

    for (const auto& tx : transactions) {
        if (isInDateRange(tx.date, range) && !tx.isDeleted) {
            std::string month = getMonthKey(tx.date);
            if (result.find(month) == result.end()) {
                result[month] = 0;
            }

            if (tx.type == TransactionType::INCOME) {
                result[month] += tx.amount;
            } else {
                result[month] -= tx.amount;
            }
        }
    }

    return result;
}

std::map<std::string, double> StatisticsService::categoryBreakdown(const DateRange& range) const {
    std::map<std::string, double> result;
    auto transactions = repository->getAll();

    for (const auto& tx : transactions) {
        if (isInDateRange(tx.date, range) && !tx.isDeleted && tx.type == TransactionType::EXPENSE) {
            if (result.find(tx.categoryId) == result.end()) {
                result[tx.categoryId] = 0;
            }
            result[tx.categoryId] += tx.amount;
        }
    }

    return result;
}

std::map<time_t, double> StatisticsService::assetTrend(const DateRange& range) const {
    std::map<time_t, double> result;
    auto transactions = repository->getAll();

    double currentBalance = 0;
    for (const auto& tx : transactions) {
        if (isInDateRange(tx.date, range) && !tx.isDeleted) {
            if (tx.type == TransactionType::INCOME) {
                currentBalance += tx.amount;
            } else {
                currentBalance -= tx.amount;
            }
            result[tx.date] = currentBalance;
        }
    }

    return result;
}

double StatisticsService::getTotalIncome(const DateRange& range) const {
    double total = 0;
    auto transactions = repository->getAll();

    for (const auto& tx : transactions) {
        if (isInDateRange(tx.date, range) && !tx.isDeleted && tx.type == TransactionType::INCOME) {
            total += tx.amount;
        }
    }

    return total;
}

double StatisticsService::getTotalExpense(const DateRange& range) const {
    double total = 0;
    auto transactions = repository->getAll();

    for (const auto& tx : transactions) {
        if (isInDateRange(tx.date, range) && !tx.isDeleted && tx.type == TransactionType::EXPENSE) {
            total += tx.amount;
        }
    }

    return total;
}
