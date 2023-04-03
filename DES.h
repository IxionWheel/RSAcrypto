#pragma once
#pragma once
#ifndef DES_H
#define DES_H
#include "BinStr.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <cstdlib>
#include <cstring>
#define DES_KEY_SIZE 64
#define DES_BLOCK_SIZE 64

//DES工作密钥结构体
typedef struct block_cipher {
    BinStr key;//初始密钥
    BinStr* roundKeys;//16个轮密钥
    int blockSize; //块大小
    const char* encryptionMode;//工作模式
    BinStr(*encrypt)(BinStr, BinStr*);//加密函数
    BinStr(*decrypt)(BinStr, BinStr*);//解密函数
}*BlockCipher;

BlockCipher DES_initialize(BinStr key, const char* mode);

void DES_destroy(BlockCipher DES);

BinStr ECBencrypt(BinStr string, BlockCipher cipher);

BinStr ECBdecrypt(BinStr string, BlockCipher cipher);

#endif