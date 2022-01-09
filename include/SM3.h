#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include "Types.h"
using namespace Types;

class SM3 {

public:
    SM3() {}
    ~SM3() {}

    // API
    std::string Hash(const u8* arr, u32 length);
    std::string Hash(const std::string& path);

private:
    // 常量
    static u32 T(int j) { return (j < 16 ? 0x79CC4519U : 0x7A879D8AU); }

    void InitIV() {
        this->buf[0] = 0x7380166f;
        this->buf[1] = 0x4914b2b9;
        this->buf[2] = 0x172442d7;
        this->buf[3] = 0xda8a0600;
        this->buf[4] = 0xa96f30bc;
        this->buf[5] = 0x163138aa;
        this->buf[6] = 0xe38dee4d;
        this->buf[7] = 0xb0fb0e4e;
    }

    // 布尔函数
    static u32 FF(int j, u32 X, u32 Y, u32 Z) {
        if(j >= 0 && j < 16) 
            return X ^ Y ^ Z;
        else if (j < 64)
            return (X & Y) | (X & Z) | (Y & Z);
        return 0;
    }
    static u32 GG(int j, u32 X, u32 Y, u32 Z) {
        if(j >= 0 && j < 16) 
            return X ^ Y ^ Z;
        else if (j < 64)
            return (X & Y) | ((~X) & Z);
        return 0;
    }

    // 置换函数
    static u32 RoLU32(u32 num, u32 shiftBits){
        shiftBits %= 32;
        return (num << shiftBits) | (num >> (32 - shiftBits));
    }
    // 大小端转换
    static u32 EndianSwitch(u32 num) {
        u8* p = (u8*)&num;
        return p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
    }

    static u32 P_0(u32 X) {return X ^ RoLU32(X, 9) ^ RoLU32(X, 17); }
    static u32 P_1(u32 X) {return X ^ RoLU32(X, 15) ^ RoLU32(X, 23); }

    // 返回hash字符串
    std::string DumpHashStr() {
        std::stringstream ss;
        std::string strRet = "";
        for (int i = 0; i < 8; i++) {
            ss << std::hex << std::setw(8) << std::setfill('0') << this->buf[i];
            strRet += ss.str() + ' ';
            ss.str("");
        }
        strRet.pop_back();

        return strRet;
    }

    void CF(const u8* block);
    // 处理短块
    void Padding(const u8* block, int fileSize);

private:
    u32 buf[8];

};
