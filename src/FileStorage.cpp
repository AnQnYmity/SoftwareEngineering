#include "../include/storage/FileStorage.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

FileStorage::FileStorage(const std::string& dir) : storageDir(dir) {
    ensureDirectoryExists();
}

FileStorage::~FileStorage() {
    // Implement backup on destruction if needed
}

void FileStorage::ensureDirectoryExists() {
    try {
        std::string cmd;
        #ifdef _WIN32
            cmd = "if not exist \"" + storageDir + "\" mkdir \"" + storageDir + "\"";
        #else
            cmd = "mkdir -p \"" + storageDir + "\"";
        #endif
        std::system(cmd.c_str());
    } catch (const std::exception& e) {
        std::cerr << "Error creating storage directory: " << e.what() << std::endl;
    }
}

std::string FileStorage::getFilePath(const std::string& key) const {
    return storageDir + "/" + key + ".json";
}

void FileStorage::save(const std::string& key, const std::string& value) {
    try {
        std::string filePath = getFilePath(key);
        std::ofstream file(filePath);
        if (file.is_open()) {
            file << value;
            file.close();
            data[key] = value;
        } else {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
    }
}

std::string FileStorage::load(const std::string& key) {
    try {
        if (data.find(key) != data.end()) {
            return data[key];
        }

        std::string filePath = getFilePath(key);
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::string content((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            file.close();
            data[key] = content;
            return content;
        }
        return "";
    } catch (const std::exception& e) {
        std::cerr << "Error loading from file: " << e.what() << std::endl;
        return "";
    }
}

std::string FileStorage::backup() {
    try {
        std::string backupDir = storageDir + "/backup";
        std::string cmd;
        #ifdef _WIN32
            cmd = "if not exist \"" + backupDir + "\" mkdir \"" + backupDir + "\"";
        #else
            cmd = "mkdir -p \"" + backupDir + "\"";
        #endif
        std::system(cmd.c_str());
        return backupDir;
    } catch (const std::exception& e) {
        std::cerr << "Error creating backup: " << e.what() << std::endl;
        return "";
    }
}

bool FileStorage::exists(const std::string& key) {
    try {
        std::string filePath = getFilePath(key);
        std::ifstream file(filePath);
        bool fileExists = file.good();
        file.close();
        return fileExists;
    } catch (const std::exception& e) {
        std::cerr << "Error checking file existence: " << e.what() << std::endl;
        return false;
    }
}

void FileStorage::remove(const std::string& key) {
    try {
        std::string filePath = getFilePath(key);
        std::string cmd = "del \"" + filePath + "\"";
        std::system(cmd.c_str());
        data.erase(key);
    } catch (const std::exception& e) {
        std::cerr << "Error removing file: " << e.what() << std::endl;
    }
}
