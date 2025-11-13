#include "../include/controller/TransactionController.h"
#include <iostream>

TransactionController::TransactionController(
    std::shared_ptr<TransactionRepository> repo,
    std::shared_ptr<StatisticsService> stats,
    std::shared_ptr<NotificationService> notif,
    std::shared_ptr<ImportExportService> importExport)
    : repository(repo), statisticsService(stats), 
      notificationService(notif), importExportService(importExport) {}

TransactionController::~TransactionController() {}

Transaction TransactionController::create(const TransactionDTO& dto) {
    Transaction tx("", dto.amount, dto.type, dto.date, dto.categoryId, dto.note);
    Transaction created = repository->add(tx);

    // Check thresholds after adding
    auto notifications = notificationService->checkThresholds();
    for (const auto& notif : notifications) {
        notificationService->notifyListeners(notif);
    }

    return created;
}

Transaction TransactionController::edit(const std::string& id, const TransactionDTO& dto) {
    Transaction existing = repository->getById(id);
    existing.amount = dto.amount;
    existing.type = dto.type;
    existing.date = dto.date;
    existing.categoryId = dto.categoryId;
    existing.note = dto.note;

    Transaction updated = repository->update(existing);

    // Check thresholds after updating
    auto notifications = notificationService->checkThresholds();
    for (const auto& notif : notifications) {
        notificationService->notifyListeners(notif);
    }

    return updated;
}

void TransactionController::remove(const std::string& id) {
    repository->remove(id);
}

std::vector<Transaction> TransactionController::search(const TransactionFilter& filter) {
    return repository->find(filter);
}

std::vector<Transaction> TransactionController::getAll() {
    return repository->getAll();
}

std::map<std::string, double> TransactionController::getMonthlyTotals(const DateRange& range) {
    return statisticsService->calculateMonthlyTotals(range);
}

std::map<std::string, double> TransactionController::getCategoryBreakdown(const DateRange& range) {
    return statisticsService->categoryBreakdown(range);
}

std::string TransactionController::exportJSON() {
    return importExportService->exportToJSON();
}

ImportResult TransactionController::importJSON(const std::string& json) {
    return importExportService->importFromJSON(json);
}

std::string TransactionController::exportCSV() {
    return importExportService->exportToCSV();
}

std::vector<Notification> TransactionController::getNotifications() {
    return notificationService->getNotifications();
}

void TransactionController::registerNotificationListener(NotificationListener listener) {
    notificationService->registerListener(listener);
}
