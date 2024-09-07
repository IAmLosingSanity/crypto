#include <fstream>
#include <iostream>
#include <string>
#include <cmath>

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

void Round() {

}

void Encrypt() {

}

void Decrypt() {

}

void PadBlock() {

}

void UnpadBlock() {

}

int main() {
    return 0;
}
