#define _CRT_SECURE_NO_WARNINGS
#include "AES.h"

//AES类
AES::AES(int keyLen)
{
    this->Nb = 4;
    switch (keyLen)
    {
        case 128:
            this->Nk = 4;
            this->Nr = 10;
            break;
        case 192:
            this->Nk = 6;
            this->Nr = 12;
            break;
        case 256:
            this->Nk = 8;
            this->Nr = 14;
            break;
        default:
            throw "Incorrect key length";
    }

    blockBytesLen = 4 * this->Nb * sizeof(unsigned char);//字节数，128位为16
}

//CBC模式加密
unsigned char* AES::EncryptCBC(unsigned char in[], unsigned int inLen, unsigned  char key[], unsigned char* iv, unsigned int outLen)
{
    unsigned char* alignIn = PaddingNulls(in, inLen, outLen);//填充0之后的明文
    unsigned char* out = new unsigned char[outLen];//密文字符数组
    unsigned char* block = new unsigned char[blockBytesLen];//工作块，16字节
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];//轮密钥，16字节*11
    KeyExpansion(key, roundKeys);//密钥编排
    memcpy(block, iv, blockBytesLen);//初始向量
    for (unsigned int i = 0; i < outLen; i += blockBytesLen)//根据明文长度，执行相应次数
    {
        XorBlocks(block, alignIn + i, block, blockBytesLen);//block与明文对应块异或（CBC模式特点）
        EncryptBlock(block, out + i, roundKeys);//进行AES加密
        memcpy(block, out + i, blockBytesLen);//再将结果复制到block
    }

    delete[] block;
    delete[] alignIn;
    delete[] roundKeys;

    return out;
}

//CBC模式解密
unsigned char* AES::DecryptCBC(unsigned char in[], unsigned int inLen, unsigned  char key[], unsigned char* iv)
{
    unsigned char* out = new unsigned char[inLen];//明文字符数组
    unsigned char* block = new unsigned char[blockBytesLen];//工作块
    unsigned char* roundKeys = new unsigned char[4 * Nb * (Nr + 1)];//轮密钥，16字节*11
    KeyExpansion(key, roundKeys);//密钥编排
    memcpy(block, iv, blockBytesLen);//初始向量
    for (unsigned int i = 0; i < inLen; i += blockBytesLen)
    {
        DecryptBlock(in + i, out + i, roundKeys);//先解密
        XorBlocks(block, out + i, out + i, blockBytesLen);//再异或
        memcpy(block, in + i, blockBytesLen);//复制迭代
    }

    delete[] block;
    delete[] roundKeys;

    return out;
}

//填充0
unsigned char* AES::PaddingNulls(unsigned char in[], unsigned int inLen, unsigned int alignLen)
{
    unsigned char* alignIn = new unsigned char[alignLen];
    memcpy(alignIn, in, inLen);
    memset(alignIn + inLen, 0x00, alignLen - inLen);//将指针变量s所指向的前 n 字节的内存单元用0替换
    return alignIn;
}

//AES块加密
void AES::EncryptBlock(unsigned char in[], unsigned char out[], unsigned  char* roundKeys)
{
    unsigned char** state = new unsigned char* [4];
    state[0] = new unsigned char[4 * Nb];//一个状态是16个字节
    int i, j, round;
    for (i = 0; i < 4; i++)
    {
        state[i] = state[0] + Nb * i;
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            state[i][j] = in[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys);//初始轮密钥加

    for (round = 1; round <= Nr - 1; round++)//9轮
    {
        SubBytes(state);//字节替代
        ShiftRows(state);//行移位
        MixColumns(state);//列混淆
        AddRoundKey(state, roundKeys + round * 4 * Nb);//轮密钥加
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, roundKeys + Nr * 4 * Nb);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            out[i + 4 * j] = state[i][j];
        }
    }

    delete[] state[0];
    delete[] state;
}

//AES块解密
void AES::DecryptBlock(unsigned char in[], unsigned char out[], unsigned  char* roundKeys)
{
    unsigned char** state = new unsigned char* [4];
    state[0] = new unsigned  char[4 * Nb];
    int i, j, round;
    for (i = 0; i < 4; i++)
    {
        state[i] = state[0] + Nb * i;
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++) {
            state[i][j] = in[i + 4 * j];
        }
    }

    AddRoundKey(state, roundKeys + Nr * 4 * Nb);

    for (round = Nr - 1; round >= 1; round--)
    {
        InvSubBytes(state);
        InvShiftRows(state);
        AddRoundKey(state, roundKeys + round * 4 * Nb);
        InvMixColumns(state);
    }

    InvSubBytes(state);
    InvShiftRows(state);
    AddRoundKey(state, roundKeys);

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++) {
            out[i + 4 * j] = state[i][j];
        }
    }

    delete[] state[0];
    delete[] state;
}

//S盒替代
void AES::SubBytes(unsigned char** state)
{
    int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            t = state[i][j];
            state[i][j] = sbox[t / 16][t % 16];
        }
    }

}

//单行移位,第i行循环左移n字节
void AES::ShiftRow(unsigned char** state, int i, int n)    // shift row i on n positions
{
    unsigned char* tmp = new unsigned char[Nb];
    for (int j = 0; j < Nb; j++)
    {
        tmp[j] = state[i][(j + n) % Nb];
    }
    memcpy(state[i], tmp, Nb * sizeof(unsigned char));

    delete[] tmp;
}

//行移位
void AES::ShiftRows(unsigned char** state)
{
    ShiftRow(state, 1, 1);
    ShiftRow(state, 2, 2);
    ShiftRow(state, 3, 3);
}



//快速软件实现的外部算法
unsigned char AES::FieldMult(unsigned char b)
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}
//单列混和
void AES::MixColumn(unsigned char* s)
{
    unsigned char x[4];//输入列的复制
    for (unsigned int c = 0; c < 4; c++)
    {
        x[c] = s[c];
    }
    s[0] = x[1] ^ x[2] ^ x[3];
    s[1] = x[0] ^ x[2] ^ x[3];
    s[2] = x[1] ^ x[0] ^ x[3];
    s[3] = x[1] ^ x[2] ^ x[0];
    for (unsigned int c = 0; c < 4; c++)
    {
        x[c] = FieldMult(x[c]);
    }
    s[0] = s[0] ^ x[0] ^ x[1];
    s[1] = s[1] ^ x[1] ^ x[2];
    s[2] = s[2] ^ x[2] ^ x[3];
    s[3] = s[3] ^ x[3] ^ x[0];
}

//列混和
void AES::MixColumns(unsigned char** state)
{
    unsigned char* temp = new unsigned char[4];

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            temp[j] = state[j][i]; //temp表示目前的列
        }
        MixColumn(temp); //单列混淆
        for (int j = 0; j < 4; ++j)
        {
            state[j][i] = temp[j]; //放回状态中
        }
    }
    delete[] temp;
}

//轮密钥加（异或）
void AES::AddRoundKey(unsigned char** state, unsigned char* key)
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            state[i][j] = state[i][j] ^ key[i + 4 * j];
        }
    }
}

//S盒替换
void AES::SubWord(unsigned char* a)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        a[i] = sbox[a[i] / 16][a[i] % 16];
    }
}

//循环左移
void AES::RotWord(unsigned char* a)
{
    unsigned char c = a[0];
    a[0] = a[1];
    a[1] = a[2];
    a[2] = a[3];
    a[3] = c;
}

void AES::XorWords(unsigned char* a, unsigned char* b, unsigned char* c)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        c[i] = a[i] ^ b[i];
    }
}

void AES::Rcon(unsigned char* a, int n)
{
    int i;
    unsigned char c = 1;
    for (i = 0; i < n - 1; i++)
    {
        c = xtime(c);
    }

    a[0] = c;
    a[1] = a[2] = a[3] = 0;
}

//密钥编排 11条
void AES::KeyExpansion(unsigned char key[], unsigned char w[])
{
    unsigned char* temp = new unsigned char[4];
    unsigned char* rcon = new unsigned char[4];

    int i = 0;
    while (i < 4 * Nk)
    {
        w[i] = key[i];
        i++;
    }

    i = 4 * Nk;
    while (i < 4 * Nb * (Nr + 1))
    {
        temp[0] = w[i - 4 + 0];
        temp[1] = w[i - 4 + 1];
        temp[2] = w[i - 4 + 2];
        temp[3] = w[i - 4 + 3];

        if (i / 4 % Nk == 0)
        {
            RotWord(temp);//循环左移
            SubWord(temp);//S盒替换
            Rcon(rcon, i / (Nk * 4));
            XorWords(temp, rcon, temp);//异或
        }
        else if (Nk > 6 && i / 4 % Nk == 4)
        {
            SubWord(temp);
        }
        //第i轮密钥
        w[i + 0] = w[i - 4 * Nk] ^ temp[0];
        w[i + 1] = w[i + 1 - 4 * Nk] ^ temp[1];
        w[i + 2] = w[i + 2 - 4 * Nk] ^ temp[2];
        w[i + 3] = w[i + 3 - 4 * Nk] ^ temp[3];
        i += 4;
    }

    delete[]rcon;
    delete[]temp;

}


//S盒逆运算
void AES::InvSubBytes(unsigned char** state)
{
    int i, j;
    unsigned char t;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            t = state[i][j];
            state[i][j] = inv_sbox[t / 16][t % 16];
        }
    }
}

unsigned char AES::mul_bytes(unsigned char a, unsigned char b) // multiplication a and b in galois field
{
    unsigned char p = 0;
    unsigned char high_bit_mask = 0x80;
    unsigned char high_bit = 0;
    unsigned char modulo = 0x1B; /* x^8 + x^4 + x^3 + x + 1 */

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }

        high_bit = a & high_bit_mask;
        a <<= 1;
        if (high_bit) {
            a ^= modulo;
        }
        b >>= 1;
    }

    return p;
}

unsigned char AES::xtime(unsigned char b)
{
    return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}
//列混合的逆操作
void AES::InvMixColumns(unsigned char** state)
{
    unsigned char s[4], s1[4];
    int i, j;

    for (j = 0; j < Nb; j++)
    {
        for (i = 0; i < 4; i++)
        {
            s[i] = state[i][j];
        }
        s1[0] = mul_bytes(0x0e, s[0]) ^ mul_bytes(0x0b, s[1]) ^ mul_bytes(0x0d, s[2]) ^ mul_bytes(0x09, s[3]);
        s1[1] = mul_bytes(0x09, s[0]) ^ mul_bytes(0x0e, s[1]) ^ mul_bytes(0x0b, s[2]) ^ mul_bytes(0x0d, s[3]);
        s1[2] = mul_bytes(0x0d, s[0]) ^ mul_bytes(0x09, s[1]) ^ mul_bytes(0x0e, s[2]) ^ mul_bytes(0x0b, s[3]);
        s1[3] = mul_bytes(0x0b, s[0]) ^ mul_bytes(0x0d, s[1]) ^ mul_bytes(0x09, s[2]) ^ mul_bytes(0x0e, s[3]);

        for (i = 0; i < 4; i++)
        {
            state[i][j] = s1[i];
        }
    }
}

//行移位的逆操作
void AES::InvShiftRows(unsigned char** state)
{
    ShiftRow(state, 1, Nb - 1);
    ShiftRow(state, 2, Nb - 2);
    ShiftRow(state, 3, Nb - 3);
}

//c=a异或b
void AES::XorBlocks(unsigned char* a, unsigned char* b, unsigned char* c, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        c[i] = a[i] ^ b[i];
    }
}

//打印
void AES::printHexArray(unsigned char a[], unsigned int n)
{
    for (unsigned int i = 0; i < n; i++) {
        printf("%02x ", a[i]);
    }
    cout << endl;
}
