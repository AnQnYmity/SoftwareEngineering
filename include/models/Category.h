#ifndef CATEGORY_H
#define CATEGORY_H

#include <string>
#include <optional>

struct Category {
    std::string id;
    std::string name;
    std::string color;
    std::optional<std::string> parentId;

    Category() = default;

    Category(const std::string& _id, const std::string& _name, const std::string& _color)
        : id(_id), name(_name), color(_color), parentId(std::nullopt) {}

    Category(const std::string& _id, const std::string& _name, const std::string& _color,
             const std::string& _parentId)
        : id(_id), name(_name), color(_color), parentId(_parentId) {}
};

#endif // CATEGORY_H
