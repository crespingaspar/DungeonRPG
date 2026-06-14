#pragma once
#include <string>
#include <fstream>
#include <iostream>

struct GameSaveData {
    int floor = 1;
    int playerHP = 100;
    int playerMaxHP = 100;
    int playerLevel = 1;
    int playerExp = 0;
    int playerExpToNext = 50;
    float playerSpeed = 180.f;
    int attackPower = 20;
    float playerX = 0.f;
    float playerY = 0.f;
};

class SaveSystem {
public:
    static std::string slotFilename(int slot) {
        return "save_" + std::to_string(slot) + ".dat";
    }

    static bool save(const GameSaveData& data, const std::string& filename = "save_0.dat") {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        file.write(reinterpret_cast<const char*>(&data), sizeof(GameSaveData));
        return true;
    }

    static bool load(GameSaveData& data, const std::string& filename = "save_0.dat") {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        file.read(reinterpret_cast<char*>(&data), sizeof(GameSaveData));
        return true;
    }

    static bool hasSave(const std::string& filename = "save_0.dat") {
        std::ifstream file(filename);
        return file.good();
    }

    static void deleteSave(const std::string& filename = "save_0.dat") {
        std::remove(filename.c_str());
    }
};