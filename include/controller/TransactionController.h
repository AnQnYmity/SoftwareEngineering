#ifndef TRANSACTIONCONTROLLER_H
#define TRANSACTIONCONTROLLER_H

#include "../models/Transaction.h"
#include "../storage/TransactionRepository.h"
#include "../services/StatisticsService.h"
#include "../services/NotificationService.h"
#include "../services/ImportExportService.h"
#include <memory>
#include <vector>

struct TransactionDTO {
    double amount;
    TransactionType type;
    time_t date;
    std::string categoryId;
    std::string note;
};

class TransactionController {
private:
    std::shared_ptr<TransactionRepository> repository;
    std::shared_ptr<StatisticsService> statisticsService;
    std::shared_ptr<NotificationService> notificationService;
    std::shared_ptr<ImportExportService> importExportService;

public:
    TransactionController(
        std::shared_ptr<TransactionRepository> repo,
        std::shared_ptr<StatisticsService> stats,
        std::shared_ptr<NotificationService> notif,
        std::shared_ptr<ImportExportService> importExport
    );
    ~TransactionController();

    Transaction create(const TransactionDTO& dto);
    Transaction edit(const std::string& id, const TransactionDTO& dto);
    void remove(const std::string& id);
    std::vector<Transaction> search(const TransactionFilter& filter);
    std::vector<Transaction> getAll();

    // Statistics
    std::map<std::string, double> getMonthlyTotals(const DateRange& range);
    std::map<std::string, double> getCategoryBreakdown(const DateRange& range);

    // Import/Export
    std::string exportJSON();
    ImportResult importJSON(const std::string& json);
    std::string exportCSV();

    // Notifications
    std::vector<Notification> getNotifications();
    void registerNotificationListener(NotificationListener listener);
};

#endif // TRANSACTIONCONTROLLER_H
