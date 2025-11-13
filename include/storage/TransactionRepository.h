#ifndef TRANSACTIONREPOSITORY_H
#define TRANSACTIONREPOSITORY_H

#include "../models/Transaction.h"
#include "IStorage.h"
#include <vector>
#include <memory>

struct TransactionFilter {
    std::string categoryId;
    TransactionType* type = nullptr;
    time_t dateFrom = 0;
    time_t dateTo = 0;
    std::string keyword;
};

class TransactionRepository {
private:
    std::shared_ptr<IStorage> storage;
    std::vector<Transaction> transactions;

public:
    explicit TransactionRepository(std::shared_ptr<IStorage> _storage);
    ~TransactionRepository();

    Transaction add(const Transaction& tx);
    Transaction update(const Transaction& tx);
    void remove(const std::string& txId);
    std::vector<Transaction> find(const TransactionFilter& filter) const;
    Transaction getById(const std::string& id) const;
    std::vector<Transaction> getAll() const;

private:
    void loadFromStorage();
    void saveToStorage();
    std::string generateId();
};

#endif // TRANSACTIONREPOSITORY_H
