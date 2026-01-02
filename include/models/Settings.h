#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <optional>

struct Settings {
    std::string currency;
    std::optional<double> monthlyBudget;
    std::map<std::string, double> reminderThresholds;

    Settings() : currency("USD"), monthlyBudget(std::nullopt) {}

    Settings(const std::string& _currency)
        : currency(_currency), monthlyBudget(std::nullopt) {}

    Settings(const std::string& _currency, double _monthlyBudget)
        : currency(_currency), monthlyBudget(_monthlyBudget) {}
};

#endif // SETTINGS_H
