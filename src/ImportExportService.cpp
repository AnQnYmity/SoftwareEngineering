#include "../include/services/ImportExportService.h"
#include "../include/storage/TransactionRepository.h"
#include <sstream>
#include <iomanip>

ImportExportService::ImportExportService(std::shared_ptr<TransactionRepository> repo)
    : repository(repo) {}

ImportExportService::~ImportExportService() {}

std::string ImportExportService::transactionToJSON(const Transaction& tx) const {
    std::stringstream ss;
    ss << "{ \"id\": \"" << tx.id << "\", \"amount\": " << tx.amount
       << ", \"type\": \"" << (tx.type == TransactionType::INCOME ? "INCOME" : "EXPENSE")
       << "\", \"date\": " << tx.date << ", \"categoryId\": \"" << tx.categoryId
       << "\", \"note\": \"" << tx.note << "\" }";
    return ss.str();
}

Transaction ImportExportService::jsonToTransaction(const std::string& jsonStr) const {
    Transaction tx;
    // Simplified parsing - in production use a proper JSON library
    return tx;
}

std::string ImportExportService::exportToJSON() const {
    std::stringstream ss;
    ss << "[\n";
    auto transactions = repository->getAll();

    for (size_t i = 0; i < transactions.size(); ++i) {
        const auto& tx = transactions[i];
        ss << "  { \"id\": \"" << tx.id << "\", \"amount\": " << tx.amount
           << ", \"type\": \"" << (tx.type == TransactionType::INCOME ? "INCOME" : "EXPENSE")
           << "\", \"date\": " << tx.date << ", \"categoryId\": \"" << tx.categoryId
           << "\", \"note\": \"" << tx.note << "\" }";
        if (i < transactions.size() - 1) ss << ",";
        ss << "\n";
    }

    ss << "]";
    return ss.str();
}

ImportResult ImportExportService::importFromJSON(const std::string& jsonStr) {
    ImportResult result;
    result.success = false;
    result.importedCount = 0;

    try {
        // Simplified parsing - in production use a proper JSON library
        result.success = true;
        return result;
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Parse error: ") + e.what();
        return result;
    }
}

std::string ImportExportService::exportToCSV() const {
    std::stringstream ss;
    ss << "ID,Amount,Type,Date,CategoryId,Note,CreatedAt,UpdatedAt,IsDeleted\n";

    auto transactions = repository->getAll();
    for (const auto& tx : transactions) {
        ss << tx.id << ","
           << tx.amount << ","
           << (tx.type == TransactionType::INCOME ? "INCOME" : "EXPENSE") << ","
           << tx.date << ","
           << tx.categoryId << ","
           << tx.note << ","
           << tx.createdAt << ","
           << tx.updatedAt << ","
           << (tx.isDeleted ? "true" : "false") << "\n";
    }

    return ss.str();
}

bool ImportExportService::importFromCSV(const std::string& csv) {
    try {
        std::istringstream stream(csv);
        std::string line;

        // Skip header
        std::getline(stream, line);

        while (std::getline(stream, line)) {
            if (line.empty()) continue;

            std::istringstream lineStream(line);
            std::string id, amountStr, typeStr, dateStr, categoryId, note, createdStr, updatedStr, deletedStr;

            std::getline(lineStream, id, ',');
            std::getline(lineStream, amountStr, ',');
            std::getline(lineStream, typeStr, ',');
            std::getline(lineStream, dateStr, ',');
            std::getline(lineStream, categoryId, ',');
            std::getline(lineStream, note, ',');
            std::getline(lineStream, createdStr, ',');
            std::getline(lineStream, updatedStr, ',');
            std::getline(lineStream, deletedStr, ',');

            Transaction tx;
            tx.id = id;
            tx.amount = std::stod(amountStr);
            tx.type = (typeStr == "INCOME") ? TransactionType::INCOME : TransactionType::EXPENSE;
            tx.date = std::stol(dateStr);
            tx.categoryId = categoryId;
            tx.note = note;
            tx.createdAt = std::stol(createdStr);
            tx.updatedAt = std::stol(updatedStr);
            tx.isDeleted = (deletedStr == "true");

            repository->add(tx);
        }

        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
