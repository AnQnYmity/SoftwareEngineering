#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include "../models/Transaction.h"
#include "../models/Settings.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

struct Notification {
    std::string id;
    std::string message;
    std::string type;
    time_t timestamp;
    bool isRead;

    Notification() : timestamp(0), isRead(false) {}
    Notification(const std::string& _id, const std::string& _msg, const std::string& _type)
        : id(_id), message(_msg), type(_type), timestamp(time(nullptr)), isRead(false) {}
};

using NotificationListener = std::function<void(const Notification&)>;

class TransactionRepository;

class NotificationService {
private:
    std::shared_ptr<TransactionRepository> repository;
    std::shared_ptr<Settings> settings;
    std::vector<Notification> notifications;
    std::vector<NotificationListener> listeners;

public:
    NotificationService(std::shared_ptr<TransactionRepository> repo,
                       std::shared_ptr<Settings> _settings);
    ~NotificationService();

    std::vector<Notification> checkThresholds() const;
    void registerListener(NotificationListener listener);
    void notifyListeners(const Notification& notif);
    std::vector<Notification> getNotifications() const;
    void markAsRead(const std::string& notificationId);

private:
    Notification createNotification(const std::string& message, const std::string& type);
};

#endif // NOTIFICATIONSERVICE_H
