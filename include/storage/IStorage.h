#ifndef ISTORAGE_H
#define ISTORAGE_H

#include <string>

class IStorage {
public:
    virtual ~IStorage() = default;

    virtual void save(const std::string& key, const std::string& value) = 0;
    virtual std::string load(const std::string& key) = 0;
    virtual std::string backup() = 0;
    virtual bool exists(const std::string& key) = 0;
    virtual void remove(const std::string& key) = 0;
};

#endif // ISTORAGE_H
