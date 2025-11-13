#include "../include/services/NotificationService.h"
#include "../include/storage/TransactionRepository.h"
#include <algorithm>

NotificationService::NotificationService(std::shared_ptr<TransactionRepository> repo,
                                        std::shared_ptr<Settings> _settings)
    : repository(repo), settings(_settings) {}

NotificationService::~NotificationService() {}

std::vector<Notification> NotificationService::checkThresholds() const {
    std::vector<Notification> result;

    if (!settings->monthlyBudget) {
        return result;
    }

    // Get current month's expenses
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    timeinfo->tm_mday = 1;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;
    time_t monthStart = mktime(timeinfo);

    double totalExpense = 0;
    auto transactions = repository->getAll();

    for (const auto& tx : transactions) {
        if (tx.date >= monthStart && tx.date <= now && 
            !tx.isDeleted && tx.type == TransactionType::EXPENSE) {
            totalExpense += tx.amount;
        }
    }

    double budget = settings->monthlyBudget.value();

    if (totalExpense >= budget * 0.8) {
        Notification notif("notif_budget_warning", 
                          "You've spent 80% of your monthly budget!",
                          "warning");
        result.push_back(notif);
    }

    if (totalExpense >= budget) {
        Notification notif("notif_budget_exceeded",
                          "You've exceeded your monthly budget!",
                          "danger");
        result.push_back(notif);
    }

    return result;
}

void NotificationService::registerListener(NotificationListener listener) {
    listeners.push_back(listener);
}

void NotificationService::notifyListeners(const Notification& notif) {
    for (auto& listener : listeners) {
        listener(notif);
    }
}

std::vector<Notification> NotificationService::getNotifications() const {
    return notifications;
}

void NotificationService::markAsRead(const std::string& notificationId) {
    auto it = std::find_if(notifications.begin(), notifications.end(),
                          [&notificationId](const Notification& n) {
                              return n.id == notificationId;
                          });

    if (it != notifications.end()) {
        it->isRead = true;
    }
}

Notification NotificationService::createNotification(const std::string& message,
                                                     const std::string& type) {
    std::string id = "notif_" + std::to_string(time(nullptr));
    Notification notif(id, message, type);
    notifications.push_back(notif);
    notifyListeners(notif);
    return notif;
}
