#define _CRT_SECURE_NO_WARNINGS
#include "ansix917.h"
#include <cassert>
#include <clocale>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstdint>
#include<Windows.h>

//2key的TDES算法
BinStr TDES(BinStr str, BlockCipher DES1, BlockCipher DES2)
{
    BinStr newstr = ECBencrypt(str, DES1);
    newstr = set(newstr, ECBdecrypt(str, DES2));//解密
    newstr = set(newstr, ECBencrypt(str, DES1));
    return newstr;
}

//ANSI X9.17算法主体框架
BinStr ANSIX917(BinStr key, BinStr seed, int n)
{
    assert(key != NULL && seed != NULL && n >= 0 &&
        key->length == ANSIX917_KEY_SIZE && seed->length == ANSIX917_SEED_SIZE);

    BinStr cur_seed = copyStr(seed);
    BinStr key1 = subString(key, 0, DES_KEY_SIZE - 1);//取前64位为DES密钥key1
    BinStr key2 = subString(key, DES_KEY_SIZE, (DES_KEY_SIZE * 2) - 1);//取后64位为DES密钥key2
    BlockCipher DES1 = DES_initialize(key1, "ECB");//以ECB模式初始化DES密钥
    BlockCipher DES2 = DES_initialize(key2, "ECB");

    BinStr newstr = empty_BinStr(0);
    BinStr cur_time = int_to_BinStr((unsigned)time(NULL));//生成D(当前时间)

    cur_time = set(cur_time, cut(cur_time, DES_BLOCK_SIZE));
    cur_time = set(cur_time, TDES(cur_time, DES1, DES2));
    BinStr start_time = copyStr(cur_time);//中间值l

    //n次循环生成n*64比特的CSPRN
    while (newstr->length < n * DES_BLOCK_SIZE)
    {
        cur_time = copyStr(start_time);
        BinStr old_time = copyStr(cur_time);//中间值l
        cur_time = set(cur_time, XOR(cur_time, cur_seed));//异或
        cur_time = set(cur_time, TDES(cur_time, DES1, DES2));//TDES生成x
        newstr = set(newstr, append(newstr, cur_time));//最终结果

        cur_time = set(cur_time, XOR(cur_time, old_time));
        cur_seed = set(cur_seed, TDES(cur_time, DES1, DES2));//TDES生成s
        destroy_BinStr(cur_time);
        destroy_BinStr(old_time);
    }
    destroy_BinStr(start_time);
    destroy_BinStr(cur_seed);
    destroy_BinStr(key1);
    destroy_BinStr(key2);
    DES_destroy(DES1);
    DES_destroy(DES2);

    return newstr;
}

//伪随机数产生器，用于产生初始密钥,生成len个ascii字符，有8*len位
void gen_random(char* s, const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i)
    {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    //s[len] = 0;//len是字节数
}

//产生size位的密码学安全的伪随机数
BinStr ANSIX917_CSPRNG(int size)
{
    //产生初始密钥和种子
    char* key = (char*)malloc(sizeof(char) * ANSIX917_KEY_SIZE / 8);//128位
    char* seed = (char*)malloc(sizeof(char) * ANSIX917_SEED_SIZE / 8);
    gen_random(key, ANSIX917_KEY_SIZE / 8);//128位
    gen_random(seed, ANSIX917_SEED_SIZE / 8);
    //执行ANSI X9.17算法
    BinStr rez = ANSIX917(ASCII_to_BinStr(key, ANSIX917_KEY_SIZE / 8), ASCII_to_BinStr(seed, ANSIX917_SEED_SIZE / 8), size / 64);//512/64=8轮
    return rez;
}

