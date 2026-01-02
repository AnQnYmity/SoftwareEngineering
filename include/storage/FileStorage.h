#ifndef FILESTORAGE_H
#define FILESTORAGE_H

#include "IStorage.h"
#include <map>

class FileStorage : public IStorage {
private:
    std::map<std::string, std::string> data;
    std::string storageDir;

public:
    FileStorage(const std::string& dir = "data");
    ~FileStorage();

    void save(const std::string& key, const std::string& value) override;
    std::string load(const std::string& key) override;
    std::string backup() override;
    bool exists(const std::string& key) override;
    void remove(const std::string& key) override;

private:
    std::string getFilePath(const std::string& key) const;
    void ensureDirectoryExists();
};

#endif // FILESTORAGE_H
