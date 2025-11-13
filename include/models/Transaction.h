#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <ctime>

enum class TransactionType {
    INCOME,
    EXPENSE
};

struct Transaction {
    std::string id;
    double amount;
    TransactionType type;
    time_t date;
    std::string categoryId;
    std::string note;
    time_t createdAt;
    time_t updatedAt;
    bool isDeleted;

    Transaction() : amount(0), type(TransactionType::EXPENSE), date(0), 
                    createdAt(0), updatedAt(0), isDeleted(false) {}

    Transaction(const std::string& _id, double _amount, TransactionType _type,
                time_t _date, const std::string& _categoryId, const std::string& _note)
        : id(_id), amount(_amount), type(_type), date(_date), 
          categoryId(_categoryId), note(_note), isDeleted(false) {
        createdAt = time(nullptr);
        updatedAt = createdAt;
    }
};

#endif // TRANSACTION_H
