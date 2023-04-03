#pragma once
#pragma once
#ifndef ANSIX917_H
#define ANSIX917_H

#define ANSIX917_KEY_SIZE 128
#define ANSIX917_SEED_SIZE 64

#include "DES.h"

void gen_random(char* s, const int len);

BinStr TDES(BinStr str, BlockCipher DES1, BlockCipher DES2);

BinStr ANSIX917_CSPRNG(int size);

#endif
