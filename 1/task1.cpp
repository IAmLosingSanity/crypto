#include <fstream>
#include <ios>
#include <iostream>
#include <cmath>
#include <cstring>

const uint64_t KEY = 0x0123456789ABCDEF;

uint16_t squared_plus(uint16_t m, uint16_t n) {
    return pow(m, 2) - m * n + pow(n, 2) + m - n;
}

uint16_t F1(uint16_t m2, uint16_t m3) {
    return squared_plus((m2 << 5),m3);
}

uint16_t F2(uint16_t m0, uint16_t m1) {
    return (~m0)^(m1>>12);
}

uint16_t GenerateRoundKey(uint64_t key, int round) {
    uint32_t k_left = (key >> 32) & 0xFFFFFFFF;
    uint32_t k_right = key & 0xFFFFFFFF;
    uint16_t round_key = ((k_left >> (2 * round + 1)) ^ (k_right >> (2 * round + 1))) & 0xFFFF;
    return round_key;
}

void Round(uint16_t& m0, uint16_t& m1, uint16_t& m2, uint16_t& m3, uint16_t ki) {
    m0 = m0 ^ ki;
    m1 = m1 ^ F1(m2, m3);
    m2 = m2 ^ F2(m0, m1);
    m3 = m3 ^ (ki >> 3);

    uint16_t temp_m0 = m0;
    uint16_t temp_m1 = m1;
    uint16_t temp_m2 = m2;
    uint16_t temp_m3 = m3;

    m0 = temp_m2;
    m1 = temp_m0;
    m2 = temp_m3;
    m3 = temp_m1;
}

void Encrypt(uint64_t& block, uint64_t key, int numRounds) {
    uint16_t m0 = (block >> 48) & 0xFFFF;
    uint16_t m1 = (block >> 32) & 0xFFFF;
    uint16_t m2 = (block >> 16) & 0xFFFF;
    uint16_t m3 = block & 0xFFFF;

    for (int round = 1; round <= numRounds; ++round) {
        uint16_t roundKey = GenerateRoundKey(key, round);
        Round(m0, m1, m2, m3, roundKey);
    }

    block = (static_cast<uint64_t>(m0) << 48) |
            (static_cast<uint64_t>(m1) << 32) |
            (static_cast<uint64_t>(m2) << 16) |
            static_cast<uint64_t>(m3);
}

void Decrypt(uint64_t& block, uint64_t key, int numRounds) {
    uint16_t m0 = (block >> 48) & 0xFFFF;
    uint16_t m1 = (block >> 32) & 0xFFFF;
    uint16_t m2 = (block >> 16) & 0xFFFF;
    uint16_t m3 = block & 0xFFFF;

    for (int round = numRounds; round >= 1; --round) {
        uint16_t roundKey = GenerateRoundKey(key, round);
        Round(m2, m0, m3, m1, roundKey);
    }

    block = (static_cast<uint64_t>(m0) << 48) |
            (static_cast<uint64_t>(m1) << 32) |
            (static_cast<uint64_t>(m2) << 16) |
            static_cast<uint64_t>(m3);
}

void PadBlock(uint64_t& block, std::streamsize bytesRead) {
    if (bytesRead < sizeof(block)) {
        uint64_t padding = 0;
        memcpy(reinterpret_cast<char*>(&block) + bytesRead, &padding, sizeof(block) - bytesRead);
    }
}

void UnpadBlock(uint64_t& block, std::streamsize& bytesRead) {
    while (bytesRead > 0 && reinterpret_cast<char*>(&block)[bytesRead - 1] == '\0') {
        --bytesRead;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <action>" << std::endl;
        return 1;
    }

    std::string action = argv[1];

    if (action == "e") {

        std::ifstream inputFile("input.txt", std::ios::binary);
        std::ofstream outputFile("output.txt", std::ios::binary);

        if (!inputFile.is_open() || !outputFile.is_open()) {
            std::cerr << "Failed to open files." << std::endl;
            return 1;
        }

        uint64_t block;
        std::streamsize bytesRead;
        while ((bytesRead = inputFile.read(reinterpret_cast<char*>(&block), sizeof(block)).gcount()) > 0) {
            PadBlock(block, bytesRead);
            Encrypt(block, KEY, 8);
            outputFile.write(reinterpret_cast<const char*>(&block), sizeof(block));
        }

        inputFile.close();
        outputFile.close();

        std::cout << "Encryption completed successfully." << std::endl;

        return 0;
    }

    else if (action == "d")
    {
        std::ifstream inputFile("output.txt", std::ios::binary);
        std::ofstream outputFile("decoded.txt", std::ios::binary);

        if (!inputFile.is_open() || !outputFile.is_open()) {
            std::cerr << "Failed to open files." << std::endl;
            return 1;
        }

        uint64_t block;
        std::streamsize bytesRead;
        while ((bytesRead = inputFile.read(reinterpret_cast<char*>(&block), sizeof(block)).gcount()) > 0) {
            Decrypt(block, KEY, 8);
            UnpadBlock(block, bytesRead);
            outputFile.write(reinterpret_cast<const char*>(&block), bytesRead);
        }

        inputFile.close();
        outputFile.close();

        std::cout << "Decryption completed successfully." << std::endl;

        return 0;
    }

    else {
        std::cerr << "Invalid action. Available actions: e (encode), d (decode)" << std::endl;
        return 1;
    }
}
