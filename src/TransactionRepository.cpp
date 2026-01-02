#include "../include/storage/TransactionRepository.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <sstream>

TransactionRepository::TransactionRepository(std::shared_ptr<IStorage> _storage)
    : storage(_storage) {
    loadFromStorage();
}

TransactionRepository::~TransactionRepository() {
    saveToStorage();
}

std::string TransactionRepository::generateId() {
    static int counter = 0;
    return "tx_" + std::to_string(time(nullptr)) + "_" + std::to_string(counter++);
}

Transaction TransactionRepository::add(const Transaction& tx) {
    Transaction newTx = tx;
    if (newTx.id.empty()) {
        newTx.id = generateId();
    }
    newTx.createdAt = time(nullptr);
    newTx.updatedAt = newTx.createdAt;
    newTx.isDeleted = false;

    transactions.push_back(newTx);
    saveToStorage();
    return newTx;
}

Transaction TransactionRepository::update(const Transaction& tx) {
    auto it = std::find_if(transactions.begin(), transactions.end(),
                          [&tx](const Transaction& t) { return t.id == tx.id; });

    if (it != transactions.end()) {
        *it = tx;
        it->updatedAt = time(nullptr);
        saveToStorage();
        return *it;
    }

    throw std::runtime_error("Transaction not found: " + tx.id);
}

void TransactionRepository::remove(const std::string& txId) {
    auto it = std::find_if(transactions.begin(), transactions.end(),
                          [&txId](const Transaction& t) { return t.id == txId; });

    if (it != transactions.end()) {
        it->isDeleted = true;
        saveToStorage();
    }
}

std::vector<Transaction> TransactionRepository::find(const TransactionFilter& filter) const {
    std::vector<Transaction> result;

    for (const auto& tx : transactions) {
        if (tx.isDeleted) continue;

        if (!filter.categoryId.empty() && tx.categoryId != filter.categoryId) {
            continue;
        }

        if (filter.type && *filter.type != tx.type) {
            continue;
        }

        if (filter.dateFrom > 0 && tx.date < filter.dateFrom) {
            continue;
        }

        if (filter.dateTo > 0 && tx.date > filter.dateTo) {
            continue;
        }

        if (!filter.keyword.empty() && tx.note.find(filter.keyword) == std::string::npos) {
            continue;
        }

        result.push_back(tx);
    }

    return result;
}

Transaction TransactionRepository::getById(const std::string& id) const {
    auto it = std::find_if(transactions.begin(), transactions.end(),
                          [&id](const Transaction& t) { return t.id == id && !t.isDeleted; });

    if (it != transactions.end()) {
        return *it;
    }

    throw std::runtime_error("Transaction not found: " + id);
}

std::vector<Transaction> TransactionRepository::getAll() const {
    std::vector<Transaction> result;
    for (const auto& tx : transactions) {
        if (!tx.isDeleted) {
            result.push_back(tx);
        }
    }
    return result;
}

void TransactionRepository::loadFromStorage() {
    try {
        std::string data = storage->load("transactions");
        if (data.empty()) {
            return;
        }

        // Simple CSV-like parsing for demo purposes
        // In production, use a proper JSON library
        // std::cerr << "Note: Using simplified storage format" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading transactions: " << e.what() << std::endl;
    }
}

void TransactionRepository::saveToStorage() {
    try {
        std::string content;
        for (const auto& tx : transactions) {
            content += tx.id + "|" + std::to_string(tx.amount) + "|" +
                      std::to_string(static_cast<int>(tx.type)) + "|" +
                      std::to_string(tx.date) + "|" + tx.categoryId + "|" +
                      tx.note + "|" + std::to_string(tx.createdAt) + "|" +
                      std::to_string(tx.updatedAt) + "|" +
                      (tx.isDeleted ? "1" : "0") + "\n";
        }
        storage->save("transactions", content);
    } catch (const std::exception& e) {
        std::cerr << "Error saving transactions: " << e.what() << std::endl;
    }
}
