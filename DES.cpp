#include "DES.h"
const int DES_ROUNDS = 16;//轮数

const int DES_ROUND_KEY_SIZE = 48;//轮密钥长度

//初始置换IP
const int I_PERMUTATION_SIZE = 64;
int I_PERMUTATION[] = { 58, 50, 42, 34, 26, 18, 10, 2,
					   60, 52, 44, 36, 28, 20, 12, 4,
					   62, 54, 46, 38, 30, 22, 14, 6,
					   64, 56, 48, 40, 32, 24, 16, 8,
					   57, 49, 41, 33, 25, 17,  9, 1,
					   59, 51, 43, 35, 27, 19, 11, 3,
					   61, 53, 45, 37, 29, 21, 13, 5,
					   63, 55, 47, 39, 31, 23, 15, 7 };

//密钥置换
const int PC1_PERMUTATION_SIZE = 28;
int PC1_C_PERMUTATION[] = { 57, 49, 41, 33, 25, 17,  9,
							1, 58, 50, 42, 34, 26, 18,
						   10,  2, 59, 51, 43, 35, 27,
						   19, 11,  3, 60, 52, 44, 36 };

int PC1_D_PERMUTATION[] = { 63, 55, 47, 39, 31, 23, 15,
							7, 62, 54, 46, 38, 30, 22,
						   14,  6, 61, 53, 45, 37, 29,
						   21, 13,  5, 28, 20, 12,  4 };

//循环移位
int KEY_SHIFTS[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

//压缩置换
const int PC2_PERMUTATION_SIZE = 48;
int PC2_PERMUTATION[] = { 14, 17, 11, 24,  1,  5,
						  3, 28, 15,  6, 21, 10,
						 23, 19, 12,  4, 26,  8,
						 16,  7, 27, 20, 13,  2,
						 41, 52, 31, 37, 47, 55,
						 30, 40, 51, 45, 33, 48,
						 44, 49, 39, 56, 34, 53,
						 46, 42, 50, 36, 29, 32 };


//E扩展置换
const int E_PERMUTATION_SIZE = 48;
int E_PERMUTATION[] = { 32,  1,  2,  3,  4,  5,
						4,  5,  6,  7,  8,  9,
						8,  9, 10, 11, 12, 13,
					   12, 13, 14, 15, 16, 17,
					   16, 17, 18, 19, 20, 21,
					   20, 21, 22, 23, 24, 25,
					   24, 25, 26, 27, 28, 29,
					   28, 29, 30, 31, 32,  1 };

//P盒置换
const int P_PERMUTATION_SIZE = 32;
int P_PERMUTATION[] = { 16,  7, 20, 21, 29, 12, 28, 17,
						1, 15, 23, 26,  5, 18, 31, 10,
						2,  8, 24, 14, 32, 27,  3,  9,
					   19, 13, 30,  6, 22, 11,  4, 25 };

//8个S盒替代
const int S_BOX_INPUT_SIZE = 48;
const int S_BOX_COLS = 16;
const int S_BOX_ROWS = 4;
int S_BOX[][64] = { {14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
					 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
					 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
					15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13},

				   {15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
					 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
					10, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
					13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9},

				   {10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
					13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
					13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
					 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12},

				   { 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
					13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
					10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
					 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14},

				   { 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
					14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
					 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
					11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3},

				   {12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
					10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
					 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
					 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13},

				   { 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
					13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
					 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
					 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12},

				   {13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
					 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
					 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
					 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11} };

//ECB模式加密
BinStr ECBencrypt(BinStr string, BlockCipher cipher)
{
	if (string != NULL && cipher != NULL && string->length % cipher->blockSize == 0)
	{
		BinStr msg = empty_BinStr(0);
		for (int i = 0; i < string->length; i += cipher->blockSize)
		{
			BinStr bin_str = subString(string, i, i + cipher->blockSize - 1);//对加密数据进行分组
			bin_str = set(bin_str, (*cipher->encrypt)(bin_str, cipher->roundKeys));//进行加密
			msg = set(msg, append(msg, bin_str));//获得密文
			destroy_BinStr(bin_str);
		}
		return msg;
	}
	else
		exit(-1);
}

//ECB模式加密
BinStr ECBdecrypt(BinStr string, BlockCipher cipher)
{
	assert(string != NULL && cipher != NULL && string->length % cipher->blockSize == 0);
	BinStr msg = empty_BinStr(0);
	for (int i = 0; i < string->length; i += cipher->blockSize)
	{
		BinStr bin_str = subString(string, i, i + cipher->blockSize - 1);//对密文进行分组
		bin_str = set(bin_str, (*cipher->decrypt)(bin_str, cipher->roundKeys));//进行解密
		msg = set(msg, append(msg, bin_str));//获得明文
		destroy_BinStr(bin_str);
	}
	return msg;
}

//密钥置换，去掉8个校验位
BinStr cPermutation(BinStr key)
{
	assert(key != NULL && key->length == DES_KEY_SIZE);
	return permutate(key, PC1_C_PERMUTATION, PC1_PERMUTATION_SIZE, 1);
}

BinStr dPermutation(BinStr key)
{
	assert(key != NULL && key->length == DES_KEY_SIZE);
	return permutate(key, PC1_D_PERMUTATION, PC1_PERMUTATION_SIZE, 1);
}

//压缩置换,选择48位子密钥
BinStr rPermutation(BinStr key)
{
	assert(key != NULL && key->length == PC1_PERMUTATION_SIZE * 2);
	return permutate(key, PC2_PERMUTATION, PC2_PERMUTATION_SIZE, 1);
}

//生成16个48位的轮密钥
BinStr* initializeRoundKeys(BinStr key)
{
	assert(key != NULL && key->length == DES_KEY_SIZE);
	BinStr key_block_C = cPermutation(key);
	BinStr key_block_D = dPermutation(key);//分成两块，分别进行密钥置换
	BinStr* round_keys = (BinStr*)malloc(sizeof(BinStr) * DES_ROUNDS);
	if (round_keys == NULL)
	{
		printf("分配空间失败！");
		exit(-1);
	}
	for (int i = 0; i < DES_ROUNDS; i++)
	{
		key_block_C = set(key_block_C, rotateL(key_block_C, KEY_SHIFTS[i]));
		key_block_D = set(key_block_D, rotateL(key_block_D, KEY_SHIFTS[i]));//根据轮数循环左移不同位数
		BinStr round_key = append(key_block_C, key_block_D);//拼成56位
		round_key = set(round_key, rPermutation(round_key));//压缩置换
		round_keys[i] = round_key;
	}

	destroy_BinStr(key_block_C);
	destroy_BinStr(key_block_D);
	return round_keys;
}

//释放轮密钥
void destroyRoundKeys(BlockCipher DES)
{
	for (int i = 0; i < DES_ROUNDS; i++)
	{
		free(DES->roundKeys[i]);
	}
	free(DES->roundKeys);
}

//初始IP置换
BinStr iPermutation(BinStr block)
{
	assert(block != NULL && block->length == DES_BLOCK_SIZE);
	return permutate(block, I_PERMUTATION, I_PERMUTATION_SIZE, 1);
}

//最终置换，即IP逆置换
BinStr fPermutation(BinStr block)
{
	assert(block != NULL && block->length == DES_BLOCK_SIZE);
	return reversePermutate(block, I_PERMUTATION, I_PERMUTATION_SIZE, 1);
}

//E扩展置换，目标是IP置换后获得的右半部分R，将32位输入扩展为48位(分为4位×8组)输出
BinStr ePermutation(BinStr R)
{
	assert(R != NULL && R->length == DES_BLOCK_SIZE / 2);
	return permutate(R, E_PERMUTATION, E_PERMUTATION_SIZE, 1);
}

//P盒置换,把32位输入映射到输出位
BinStr pPermutation(BinStr R)
{
	assert(R != NULL && R->length == DES_BLOCK_SIZE / 2);
	return permutate(R, P_PERMUTATION, P_PERMUTATION_SIZE, 1);
}

//S盒代替，每个S盒有6位输入4位输出,最后结果是32位
BinStr sBox(BinStr block)
{
	assert(block != NULL && block->length == S_BOX_INPUT_SIZE);
	BinStr newstr = empty_BinStr(0);
	for (int box = 0; box < 8; box++)
	{
		int start = box * 6;
		BinStr rowStr = subString(block, start, start);
		BinStr rowEnd = subString(block, start + 5, start + 5);
		rowStr = set(rowStr, append(rowStr, rowEnd));//首位和末位决定行号
		int row = toDecimal(rowStr);
		destroy_BinStr(rowStr);
		destroy_BinStr(rowEnd);

		BinStr colStr = subString(block, start + 1, start + 4);
		int col = toDecimal(colStr);//中间4位决定列号
		destroy_BinStr(colStr);

		int element = S_BOX[box][(S_BOX_COLS * row) + col];
		BinStr bin_element = int_to_BinStr(element);//将对应的S盒中的int转为BinStr
		bin_element = set(bin_element, cut(bin_element, 4));//截取后4位
		newstr = set(newstr, append(newstr, bin_element));
		destroy_BinStr(bin_element);
	}
	return newstr;
}

//轮函数
BinStr DESroundFunction(BinStr block, BinStr key)
{
	assert(block != NULL && block->length == DES_BLOCK_SIZE / 2 && key != NULL && key->length == DES_ROUND_KEY_SIZE);
	BinStr newstr = ePermutation(block); //对R进行E扩展置换
	newstr = set(newstr, XOR(newstr, key));//异或产生中间值
	newstr = set(newstr, sBox(newstr));//S盒代替
	newstr = set(newstr, pPermutation(newstr));//P盒置换
	return newstr;
}

//DES加密的具体过程
BinStr DESencrypt(BinStr block, BinStr* roundKeys)
{
	assert(block != NULL && block->length == DES_BLOCK_SIZE);

	BinStr newstr = iPermutation(block);//对明文进行初始IP置换
	BinStr L = subString(newstr, 0, (DES_BLOCK_SIZE / 2) - 1);//左半部分为L
	BinStr R = subString(newstr, DES_BLOCK_SIZE / 2, DES_BLOCK_SIZE - 1);//右半部分为R
	//进行16轮Feistel轮函数运算
	for (int i = 0; i < DES_ROUNDS; i++)
	{
		BinStr newstr_R = DESroundFunction(R, roundKeys[i]);//轮函数
		newstr_R = set(newstr_R, XOR(newstr_R, L));//结果与L异或
		destroy_BinStr(L);
		L = R;
		R = newstr_R;//将左右两组交换，进行下一轮
	}

	newstr = set(newstr, append(R, L));
	newstr = set(newstr, fPermutation(newstr));//最终置换
	destroy_BinStr(L);
	destroy_BinStr(R);
	return newstr;
}

//DES解密的具体过程，是加密的逆运算
BinStr DESdecrypt(BinStr block, BinStr* roundKeys)
{
	assert(block != NULL && block->length == DES_BLOCK_SIZE);

	BinStr newstr = iPermutation(block);//初始IP置换
	BinStr R = subString(newstr, 0, (DES_BLOCK_SIZE / 2) - 1);
	BinStr L = subString(newstr, DES_BLOCK_SIZE / 2, DES_BLOCK_SIZE - 1);

	for (int i = DES_ROUNDS - 1; i >= 0; i--)
	{
		BinStr newstr_L = DESroundFunction(L, roundKeys[i]);//轮函数
		newstr_L = set(newstr_L, XOR(newstr_L, R));
		destroy_BinStr(R);
		R = L;
		L = newstr_L;
	}

	newstr = set(newstr, append(L, R));
	newstr = set(newstr, fPermutation(newstr));
	destroy_BinStr(L);
	destroy_BinStr(R);
	return newstr;
}

//初始化DES密钥
BlockCipher DES_initialize(BinStr key, const char* mode)
{
	assert(key != NULL && mode != NULL && key->length == DES_KEY_SIZE);

	BlockCipher DES = (BlockCipher)malloc(sizeof(struct block_cipher));
	if (DES == NULL)
	{
		printf("分配空间失败！");
		exit(-1);
	}
	DES->key = copyStr(key);
	DES->roundKeys = initializeRoundKeys(key);//生成轮密钥
	DES->encryptionMode = mode;//模式默认为ECB
	DES->blockSize = DES_BLOCK_SIZE;//块大小固定为64
	DES->encrypt = DESencrypt;
	DES->decrypt = DESdecrypt;
	return DES;
}

//释放DES密钥
void DES_destroy(BlockCipher DES)
{
	destroyRoundKeys(DES);
	free(DES->key);
	free(DES);
}