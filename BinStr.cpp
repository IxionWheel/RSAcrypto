#include "BinStr.h"
#include<iostream>
#include<bitset>
using namespace std;

const int BITS_PER_WORD = 32;
const int BITS_PER_BYTE = 8;
const int MAX_BYTE = 256;

char HEX_CONVERT[][16] = { "0000", "0001", "0010", "0011", "0100", "0101",
                          "0110", "0111", "1000", "1001", "1010", "1011",
                          "1100", "1101", "1110", "1111" };

//char字符数组转为binstr
BinStr str_to_BinStr(char* bits, int length) {
    BinStr newstr = (BinStr)malloc(sizeof(struct binstr));
    if (newstr == NULL)
    {
        printf("分配空间失败！");
        exit(-1);
    }
    newstr->bits = (bool*)malloc(sizeof(bool) * length);
    if (newstr->bits == NULL)
    {
        printf("分配空间失败！");
        exit(-1);
    }
    for (int i = 0; i < length; i++) {
        if (bits[i] == '1') {
            newstr->bits[i] = 1;
        }
        else {//bits[i]为其他数字、大写字母、小写字母字符，所以初始密钥中0的个数会特别多
            newstr->bits[i] = 0;
        }
    }
    newstr->length = length;
    return newstr;
}

//int转为8位binstr
BinStr int_to_BinStr(unsigned int n) {
    assert(n >= 0);
    if (n == 0) {
        return empty_BinStr(8);
    }
    else {
        int length = 8;
        BinStr newstr = empty_BinStr(length);
        unsigned int powerOfTwo = 1;
        for (int i = 0; i < length - 1; i++) {
            powerOfTwo *= 2;
        }
        for (int i = 0; i < length; i++) {
            if (n >= powerOfTwo) {
                n -= powerOfTwo;
                newstr->bits[i] = 1;
            }
            powerOfTwo /= 2;
        }
        return newstr;
    }
}

//将16进制的字符数组转为binstr 1字符转为4位二进制串 
BinStr hex_to_BinStr(char* str) {
    if (str) {
        int length = (int)strlen(str);//length应该为16
        BinStr newstr = empty_BinStr(length * 4);
        for (int i = 0; i < length; i++) {
            char* hex = 0;
            if ('0' <= str[i] && str[i] <= '9') {
                hex = HEX_CONVERT[(str[i] - '0')];
            }
            else if ('A' <= str[i] && str[i] <= 'F') {
                hex = HEX_CONVERT[(str[i] - 'A') + 10];
            }
            else if ('a' <= str[i] && str[i] <= 'f') {
                hex = HEX_CONVERT[(str[i] - 'a') + 10];
            }
            else
                ;
            for (int j = 0; j < 4; j++) {
                if (hex[j] == '1') {
                    newstr->bits[i * 4 + j] = true;
                }
            }
        }
        return newstr;
    }
    else
        return NULL;
}

//ASCII字符串转为binstr
BinStr ASCII_to_BinStr(char* str, int length)
{
    assert(str);

    BinStr newstr = empty_BinStr(0);
    for (int i = 0; i < length; i++) {
        BinStr app = int_to_BinStr((unsigned int)str[i]);
        app = set(app, cut(app, BITS_PER_BYTE));
        newstr = set(newstr, append(newstr, app));
        destroy_BinStr(app);
    }
    return newstr;
}

/*返回一个长为length，值全为0的BinStr结构体*/
BinStr empty_BinStr(int length) {
    //assert(length >= 0);
    BinStr newstr = (BinStr)malloc(sizeof(struct binstr));
    newstr->bits = (bool*)malloc(sizeof(bool) * length);
    for (int i = 0; i < length; i++) {
        newstr->bits[i] = 0;
    }
    newstr->length = length;
    return newstr;
}

//释放str
void destroy_BinStr(BinStr str) {
    if (str != NULL)
    {
        free(str->bits);
        free(str);
    }
    return;
}

//复制str
BinStr copyStr(BinStr str) {
    assert(str != NULL);
    BinStr newstrc = empty_BinStr(str->length);
    for (int i = 0; i < str->length; i++) {
        newstrc->bits[i] = str->bits[i];
    }
    return newstrc;
}

//返回str2，删去str1
BinStr set(BinStr str1, BinStr str2) {
    if (str1 != NULL) {
        destroy_BinStr(str1);
    }
    return str2;
}

//去掉开头的0，使开头为1
BinStr flush(BinStr str) {
    assert(str != NULL);
    int leading = 0;
    while (str->bits[leading] == 0) {
        leading++;//第一个1的位置
    }
    if (leading == 0) {//1开头的串
        return str;
    }
    else if (leading == str->length) {//全0串
        return empty_BinStr(1);
    }
    else {
        char* newstr = (char*)malloc(sizeof(char) * (str->length - (long long)leading));
        for (int i = leading; i < str->length; i++) {
            if (str->bits[i]) {//如果i位是1
                newstr[i - leading] = '1';
            }
            else {
                newstr[i - leading] = '0';
            }
        }
        BinStr newstrStr = str_to_BinStr(newstr, str->length - leading);
        free(newstr);
        return newstrStr;
    }
}

//截取str的后n位
BinStr cut(BinStr str, int n) {
    assert(str != NULL && n > 0);
    BinStr newstr = empty_BinStr(n);
    for (int i = 1; i <= n && i <= str->length; i++) {
        newstr->bits[n - i] = str->bits[str->length - i];
    }
    return newstr;
}

//从begin到end截取Binstr
BinStr subString(BinStr str, int begin, int end) {
    if (str != NULL && begin <= end && end <= str->length) {
        BinStr newstr = empty_BinStr(end - begin + 1);
        for (int i = 0; i < newstr->length; i++) {
            newstr->bits[i] = str->bits[i + begin];
        }
        return newstr;
    }
    return empty_BinStr(0);
}

//返回字节数（长度/8）
int bytes(BinStr str) {
    assert(str != NULL);
    if (str->length % BITS_PER_BYTE == 0) {
        return str->length / BITS_PER_BYTE;
    }
    else {
        return 1 + (str->length / BITS_PER_BYTE);
    }
}

//返回第n个字节的内容
BinStr getByte(BinStr str, int n) {
    assert(str != NULL && n <= str->length / BITS_PER_BYTE && n >= 0);
    BinStr newstr = empty_BinStr(BITS_PER_BYTE);//8位
    int firstBit = n * BITS_PER_BYTE;
    for (int i = firstBit; i < firstBit + BITS_PER_BYTE; i++) {
        newstr->bits[i - firstBit] = str->bits[i];
    }
    //printStr(newstr);
    return newstr;
}

//非运算
BinStr NOT(BinStr str) {
    assert(str != NULL);
    char* bits = (char*)malloc(sizeof(char) * (str->length));
    for (int i = 0; i < str->length; i++) {
        bits[i] = !str->bits[i];
    }
    BinStr newstr = str_to_BinStr(bits, str->length);
    free(bits);
    return newstr;
}

//异或运算
BinStr XOR(BinStr str1, BinStr str2) {
    assert(str1 != NULL && str2 != NULL && str1->length == str2->length);
    char* bits = (char*)malloc(sizeof(char) * (str1->length));
    for (int i = 0; i < str1->length; i++) {
        if ((str1->bits[i] ^ str2->bits[i]) == 0) {
            bits[i] = '0';
        }
        else {
            bits[i] = '1';
        }
    }
    BinStr newstr = str_to_BinStr(bits, str1->length);
    free(bits);
    return newstr;
}

//或运算
BinStr OR(BinStr str1, BinStr str2) {
    assert(str1 != NULL && str2 != NULL && str1->length == str2->length);
    char* bits = (char*)malloc(sizeof(char) * (str1->length));
    for (int i = 0; i < str1->length; i++) {
        if ((str1->bits[i] | str2->bits[i]) == 0) {
            bits[i] = '0';
        }
        else {
            bits[i] = '1';
        }
    }
    BinStr newstr = str_to_BinStr(bits, str1->length);
    free(bits);
    return newstr;
}

//与运算
BinStr AND(BinStr str1, BinStr str2) {
    assert(str1 != NULL && str2 != NULL && str1->length == str2->length);
    char* bits = (char*)malloc(sizeof(char) * (str1->length));
    for (int i = 0; i < str1->length; i++) {
        if ((str1->bits[i] & str2->bits[i]) == 0) {
            bits[i] = '0';
        }
        else {
            bits[i] = '1';
        }
    }
    BinStr newstr = str_to_BinStr(bits, str1->length);
    free(bits);
    return newstr;
}

//循环左移n位
BinStr rotateL(BinStr str, int n) {
    assert(str != NULL && n >= 0);
    n %= str->length;
    BinStr newstr = subString(str, n, str->length - 1);
    BinStr back = subString(str, 0, n - 1);
    newstr = set(newstr, append(newstr, back));
    destroy_BinStr(back);
    return newstr;
}

//循环右移n位
BinStr rotateR(BinStr str, int n) {
    assert(str != NULL && n >= 0);
    n %= str->length;
    BinStr newstr = subString(str, n, str->length - 1);
    BinStr back = subString(str, 0, n - 1);
    newstr = set(newstr, append(back, newstr));
    destroy_BinStr(back);
    return newstr;
}

//转为01字符串
char* toString(BinStr str) {
    if (str != NULL) {
        char* newstr = (char*)malloc(sizeof(char) * (str->length));
        for (int i = 0; i < str->length; i++) {
            newstr[i] = str->bits[i];
        }
        newstr[str->length] = '\0';
        return newstr;
    }
    return NULL;
}

//8位二进制串转为十进制int
int toDecimal(BinStr str) {
    assert(str != NULL);
    if (str->length > 31) {
        return -1;
    }
    else {
        int sum = 0;
        int power2 = 1;
        for (int i = str->length - 1; i >= 0; i--) {
            if (str->bits[i] == 1) {
                sum += power2;
            }
            power2 *= 2;
        }

        return sum;
    }
}

//转为ASCII字符串
char* toASCII(BinStr str) {
    char* newstr = (char*)malloc(sizeof(char) * ((long long)(str->length / BITS_PER_BYTE) + 1));//（128/8+1）*8位
    for (int i = 0; i < (str->length / BITS_PER_BYTE); i++) {
        newstr[i] = (char)toDecimal(getByte(str, i));
    }
    newstr[(str->length / BITS_PER_BYTE)] = '\0';
    return newstr;
}

//返回str1,str2的拼接
BinStr append(BinStr str1, BinStr str2) {
    assert(str1 != NULL && str2 != NULL);
    BinStr newstr = empty_BinStr(str1->length + str2->length);
    for (int i = 0; i < str1->length; i++) {
        newstr->bits[i] = str1->bits[i];
    }
    for (int i = 0; i < str2->length; i++) {
        newstr->bits[i + str1->length] = str2->bits[i];
    }
    return newstr;
}

//打印str
void printStr(BinStr str) {
    if (str == NULL) {
        printf("NULL\n");
    }
    else {
        printf("length=%d\n", str->length);
        for (int i = 0; i < str->length; i++) {

            printf("%d", str->bits[i]);
        }
        printf("\n");
    }
}

//按order规则进行置换
BinStr permutate(BinStr str, int* order, int len, int offset) {
    assert(str != NULL && order != NULL && len >= 0 && offset >= 0);
    BinStr newstr = empty_BinStr(len);
    for (int i = 0; i < len; i++) {
        assert(order[i] - offset < str->length);
        newstr->bits[i] = str->bits[order[i] - offset];//考虑到数组从0开始，要减1
    }
    return newstr;
}

//逆置换，用于最终置换
BinStr reversePermutate(BinStr str, int* order, int len, int offset) {
    assert(str != NULL && order != NULL && len >= 0 && offset >= 0);
    BinStr newstr = empty_BinStr(len);
    for (int i = 0; i < len; i++) {
        assert(order[i] - offset < str->length);
        newstr->bits[order[i] - offset] = str->bits[i];//此处逆置换
    }
    return newstr;
}

//相加
BinStr add(BinStr str1, BinStr str2) {
    BinStr newstr = empty_BinStr(str1->length + str2->length);
    int offset = 0;
    for (int i = 1; i <= newstr->length; i++) {
        if (str1->length >= i && str1->bits[str1->length - i])
            offset++;
        if (str2->length >= i && str2->bits[str2->length - i])
            offset++;
        if ((offset % 2 != 0 || offset == 1) && offset != 0) {
            newstr->bits[newstr->length - i] = 1;
            offset--;
        }
        offset = offset / 2;
    }
    return set(newstr, flush(newstr));
}

//逆序
BinStr reverse(BinStr str) {
    assert(str != NULL);
    BinStr newstr = empty_BinStr(str->length);
    for (int i = 0; i < str->length; i++) {
        newstr->bits[i] = str->bits[str->length - i - 1];
    }
    return newstr;
}

//每隔split_len距离选1位
BinStr* split(BinStr str, int split_len) {
    assert(str != NULL && split_len > 0 && str->length % split_len == 0);
    BinStr* list = (BinStr*)malloc(sizeof(BinStr) * str->length / split_len);
    for (int i = 0; i < str->length; i += split_len) {
        list[i] = subString(str, i, i + split_len - 1);
    }
    return list;
}
