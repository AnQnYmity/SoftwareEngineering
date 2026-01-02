#ifndef IMPORTEXPORTSERVICE_H
#define IMPORTEXPORTSERVICE_H

#include "../models/Transaction.h"
#include <string>
#include <vector>
#include <memory>

struct ImportResult {
    bool success;
    int importedCount;
    std::string errorMessage;
};

class TransactionRepository;

class ImportExportService {
private:
    std::shared_ptr<TransactionRepository> repository;

public:
    explicit ImportExportService(std::shared_ptr<TransactionRepository> repo);
    ~ImportExportService();

    std::string exportToJSON() const;
    ImportResult importFromJSON(const std::string& json);
    std::string exportToCSV() const;
    bool importFromCSV(const std::string& csv);

private:
    std::string transactionToJSON(const Transaction& tx) const;
    Transaction jsonToTransaction(const std::string& json) const;
};

#endif // IMPORTEXPORTSERVICE_H
