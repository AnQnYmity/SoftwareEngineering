#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

struct Account {
    std::string id;
    std::string name;
    double balance;
    std::string currency;

    Account() : balance(0.0), currency("USD") {}

    Account(const std::string& _id, const std::string& _name, 
            double _balance, const std::string& _currency)
        : id(_id), name(_name), balance(_balance), currency(_currency) {}
};

#endif // ACCOUNT_H
