#include <iostream>
#include <memory>
#include <cstring>
#include "../include/models/Transaction.h"
#include "../include/storage/FileStorage.h"
#include "../include/storage/TransactionRepository.h"

// Fuzzing target for TransactionRepository
// This file is designed to be used with AFL++

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 16) {
        return 0; // Not enough data
    }

    try {
        // Create a temporary storage
        auto storage = std::make_shared<FileStorage>("fuzz_test.json");
        auto repository = std::make_shared<TransactionRepository>(storage);

        // Parse fuzzing input
        double amount = 0.0;
        if (size >= sizeof(double)) {
            memcpy(&amount, data, sizeof(double));
            data += sizeof(double);
            size -= sizeof(double);
        }

        // Get transaction type
        TransactionType type = (data[0] % 2 == 0) ? TransactionType::INCOME : TransactionType::EXPENSE;
        data++;
        size--;

        // Get category and note from remaining data
        std::string categoryId;
        std::string note;
        
        if (size > 0) {
            size_t splitPoint = size / 2;
            categoryId = std::string(reinterpret_cast<const char*>(data), splitPoint);
            note = std::string(reinterpret_cast<const char*>(data + splitPoint), size - splitPoint);
        }

        // Create and add transaction
        Transaction tx;
        tx.amount = amount;
        tx.type = type;
        tx.categoryId = categoryId;
        tx.note = note;
        tx.date = time(nullptr);

        repository->add(tx);

        // Try to retrieve it
        auto transactions = repository->getAll();
        if (!transactions.empty()) {
            auto retrieved = repository->getById(transactions[0].id);
            
            // Update it
            retrieved.amount *= 1.5;
            repository->update(retrieved);
            
            // Remove it
            repository->remove(retrieved.id);
        }

        // Test filtering
        TransactionFilter filter;
        filter.categoryId = categoryId;
        repository->find(filter);

    } catch (const std::exception& e) {
        // Catch exceptions but don't crash
        // We're looking for crashes, not exceptions
    }

    return 0;
}

// Main function for standalone testing
#ifndef __AFL_FUZZ_TESTCASE_LEN
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");
    if (!f) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    uint8_t* buffer = new uint8_t[size];
    fread(buffer, 1, size, f);
    fclose(f);

    LLVMFuzzerTestOneInput(buffer, size);

    delete[] buffer;
    return 0;
}
#endif
