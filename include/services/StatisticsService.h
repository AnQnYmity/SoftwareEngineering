#ifndef STATISTICSSERVICE_H
#define STATISTICSSERVICE_H

#include "../models/Transaction.h"
#include "../models/Category.h"
#include <map>
#include <vector>
#include <memory>

struct DateRange {
    time_t from;
    time_t to;
};

class TransactionRepository;

class StatisticsService {
private:
    std::shared_ptr<TransactionRepository> repository;

public:
    explicit StatisticsService(std::shared_ptr<TransactionRepository> repo);
    ~StatisticsService();

    std::map<std::string, double> calculateMonthlyTotals(const DateRange& range) const;
    std::map<std::string, double> categoryBreakdown(const DateRange& range) const;
    std::map<time_t, double> assetTrend(const DateRange& range) const;
    double getTotalIncome(const DateRange& range) const;
    double getTotalExpense(const DateRange& range) const;

private:
    bool isInDateRange(time_t date, const DateRange& range) const;
    std::string getMonthKey(time_t timestamp) const;
};

#endif // STATISTICSSERVICE_H
