#include <iostream>
#include <fstream>
#include <NTL/ZZ.h>
#include"ansix917.h"
#include"AES.h"

using namespace NTL;
using namespace std;

#define AES_SIZE 128

struct RsaPubKey {
	ZZ n;
	ZZ b;//加密指数，设为65537
};

struct RsaPriKey {
	ZZ p;
	ZZ q;
	ZZ a;//解密指数
};

int getSequenceSize();// 选择要生成的随机数的位数
bool Miller_Rabin(ZZ n, int t);// Miller-Rabin随机算法检测n是否为素数
ZZ binstr_to_ZZ(BinStr str);// binstr_to_ZZ
BinStr ZZ_to_binstr(ZZ t, int outlen);// ZZ_to_binstr
unsigned int GetPaddingLength(unsigned int len);// 返回AES加密补全后的密文长度
int menu();// 绘制菜单
void makeRSAkeys();// 1.生成密钥
void EncryptFile();// 2.加密文件
void DecryptFile();// 3.解密文件

int main()
{
	srand((unsigned)time(0));//设立随机数种子

	while (1)
	{
		int n = menu();
		switch (n) {
			case 1:
				makeRSAkeys();
				break;
			case 2:
				EncryptFile();
				break;
			case 3:
				DecryptFile();
				break;
			case 0:
				break;
		}
		if (n == 0)
		{
			break;
		}
		system("pause");
	}

	return 0;
}

int menu()
{
	cout << "\n**********菜单**********\n"
		<< "*     1.生成密钥       *\n"
		<< "*     2.加密文件       *\n"
		<< "*     3.解密文件       *\n"
		<< "*     0.退出程序       *\n"
		<< "************************\n"
		<< "请输入功能编号:";
	int n;
	while (1)
	{
		cin >> n;
		if (!cin || n < 0 || n>3)
		{
			cin.clear();
			while (cin.get() != '\n')
				;
			cout << "输入非法，请重新输入\n";
		}
		else
			break;
	}
	return n;
}

//binstr_to_ZZ
ZZ binstr_to_ZZ(BinStr str)
{
	ZZ res;
	for (int i = 0; i < str->length; i++)
		res += power((ZZ)2, str->length - i - 1) * (str->bits[i]);
	return res;
}

//ZZ_to_binstr
BinStr ZZ_to_binstr(ZZ t, int outlen)
{
	BinStr newstr = empty_BinStr(outlen);
	int i;
	for (i = 0; i < outlen; i++)
	{
		newstr->bits[outlen - 1 - i] = (t % 2);
		if (t < 2)
		{
			break;
		}
		t = t >> 1;
	}
	for (int j = i + 1; j < outlen - 1; j++)
		newstr->bits[outlen - 1 - j] = 0;
	return newstr;
}

// Miller-Rabin随机算法检测n是否为素数
bool Miller_Rabin(ZZ n, int t)
{
	if (n == 2 || n == 3)
		return true;
	if (n < 2 || !IsOdd(n))
		return false;
	ZZ m = n - 1;
	int k = 0;
	while ((m & (ZZ)1) == 0)
	{//将x分解成(2^k)*m的样子
		k++;
		m >>= 1;
	}
	for (int i = 1; i <= t; i++)  // t为MiZZer-Rabin测试的迭代次数
	{
		ZZ a = (ZZ)rand() % (n - 1) + 1;
		ZZ x = PowerMod(a, m, n);//a^m(mod n)
		ZZ y;
		for (int j = 1; j <= k; j++)
		{
			y = MulMod(x, x, n);//x^2(mod n)
			if (y == 1 && x != 1 && x != n - 1)//二次探测
				return false;
			x = y;
		}
		if (y != 1)
			return false;//费马小定理判断
	}
	return true;
}

//选择要生成的随机数的位数
int getSequenceSize()
{
	int n = 0;
	while (true)
	{
		cout << "请输入要生成的随机素数的比特长(512或1024):\n";
		if ((cin >> n) && (n == 512 || n == 1024)) 
		{
			return n;
		}
		else
		{
			cin.clear();
			while (cin.get() != '\n')
				;
			cout << "输入非法，请重新输入\n";
		}
	}
}

//返回AES加密补全后的密文长度
unsigned int GetPaddingLength(unsigned int len)
{
	unsigned int lengthWithPadding = (len / 16);
	if (len % 16) {//若不是16的整数倍，就需要补全一个
		lengthWithPadding++;
	}

	lengthWithPadding *= 16;

	return lengthWithPadding;
}

//1.生成密钥
void makeRSAkeys()
{
	int size = getSequenceSize();
	cout << "\n1.生成RSA密钥\n请稍等";
	RsaPubKey pub;
	RsaPriKey pri;
	//1.1 生成第1个512/1024位的大整数p,并转为ZZ类型
	BinStr TestStr1 = ANSIX917_CSPRNG(size);
	ZZ teststr1 = binstr_to_ZZ(TestStr1);
	// cout << "teststr1=" << teststr1 << endl;
	while (1) {
		//1.2 进行Miller-Rabin素性检测，若通过，生成第2个；否则重新生成。
		if (Miller_Rabin(teststr1, size / 2))
		{//一次迭代的概率是1/4
			pri.p = teststr1;
			cout << "已生成p\n";//p="<<pri.p<<endl;
			break;
		}
		else
		{
			teststr1 += (IsOdd(teststr1) ? 2 : 1);//奇数加2，查找效率更高
			cout << ".";//打印'.'可以起到进度条的作用，防止用户等待时因程序无响应产生负面情绪
		}
	}

	//1.3 生成第二个512/1024位的大整数p,并转为ZZ类型
	BinStr TestStr2 = ANSIX917_CSPRNG(size);
	ZZ teststr2 = binstr_to_ZZ(TestStr2);

	while (1) {
		//1.4 进行Miller-Rabin素性检测
		if (Miller_Rabin(teststr2, size / 2))
		{
			pri.q = teststr2;
			cout << "已生成q\n";// q = " << pri.q << endl;
			//1.5 检测生成的两素数是否满足安全性
			//-------------(可以省略)--------------
			break;
		}
		else
		{
			teststr2 += (IsOdd(teststr2) ? 2 : 1);
			cout << ".";
		}
	}

	//1.6 Alice把公钥传给bob(赋值并写进txt文件中)
	pub.n = pri.p * pri.q;
	pub.b = 65537;
	ZZ r;
	r = (pri.p - 1) * (pri.q - 1);
	pri.a = InvMod(pub.b, r);
	ofstream PubKey("RSAPublicKey.txt", ios::out);
	ofstream PriKey("RSAPrivateKey.txt", ios::out);
	if (!PubKey.is_open() || !PriKey.is_open()) {
		cout << "Creat file error!" << endl;
		return;
	}
	PubKey << pub.n << "\n" << pub.b << "\n\n顺序为n,b";
	PriKey << pri.p << "\n" << pri.q << "\n" << pri.a << "\n\n顺序为p,q,a";
	PubKey.close();
	PriKey.close();
	cout << "密钥生成完毕！\n已将公钥写入RSAPublicKey.txt，将私钥写入RSAPrivateKey.txt\n" << endl;
}

//2.加密文件
void EncryptFile()
{
	cout << "\n2.加密文件" << endl;
	RsaPubKey pub;
	AES aes(128);
	//2.1 输入文件m,和公钥n,b
	ifstream file_m("m.txt", ios::in);
	ifstream PubKey("RSAPublicKey.txt", ios::in);
	if (!file_m.is_open() || !PubKey.is_open()) {
		cout << "Open file error!" << endl;
		return;
	}
	file_m.seekg(0, ifstream::end);
	unsigned int m_len = (unsigned int)file_m.tellg();
	file_m.seekg(0);
	unsigned char* m = new unsigned char[m_len];//记得delete   
	memset(m, 0, m_len);//初始化为\0
	file_m.read((char*)m, m_len);
	file_m.close();
	cout << "已读取m.txt，长度为" << m_len << endl;
	PubKey >> pub.n >> pub.b;
	PubKey.close();

	//2.2 生成128比特的临时会话密钥k，并将它加密得到ZZ类型c1=k^b mod n
	unsigned char* k_char = new unsigned char[AES_SIZE / 8 + 1];
	gen_random((char*)k_char, AES_SIZE / 8);
	BinStr k_binstr = ASCII_to_BinStr((char*)k_char, AES_SIZE / 8);
	ZZ k = binstr_to_ZZ(k_binstr);
	ZZ c1 = PowerMod(k, pub.b, pub.n);
	cout << "已生成会话密钥k，并加密为c1\n";
	//aes.printHexArray(k_char, 16);
	//cout << "\nc1=" << c1 << endl;

	BinStr IV = ANSIX917_CSPRNG(128);
	unsigned char* iv = new unsigned char[IV->length / 8];
	iv = (unsigned char*)toASCII(IV);//初始向量

	//2.3 用k加密m得到c2,AES算法，CBC模式 
	unsigned int c2_len;
	c2_len = GetPaddingLength(m_len);//计算输出文件的字节数
	unsigned char* c2 = new unsigned char[c2_len];
	c2 = aes.EncryptCBC(m, m_len, k_char, iv, c2_len);//AES加密
	cout << "已得到密文c2\n";

	//2.4 将(c1,c2_len,iv,c2)发送给Alice(即存入文件)
	ofstream ciphertext("ciphertext.txt", ios::out);
	if (!ciphertext.is_open()) {
		cout << "Creat <ciphertext.txt> error!" << endl;
		return;
	}
	ciphertext << c1 << endl;//大整数
	ciphertext << c2_len << endl;//密文个数
	for (unsigned int i = 0; i < 16; i++)
		ciphertext << (unsigned int)iv[i] << " ";//iv
	for (unsigned int i = 0; i < c2_len; i++)
		ciphertext << (unsigned int)c2[i] << " ";//字符数组的int表示
	ciphertext.close();
	cout << "加密完成！\n已写入ciphertext.txt\n" << endl;

	delete[]k_char;
	delete[]c2;
	delete[]iv;
}

//3.解密文件
void DecryptFile()
{
	cout << "\n3.解密文件\n";
	//3.1 读取私钥和密文文件，用私钥解密c1得到k
	AES aes(128);
	RsaPriKey pri;
	ifstream PriKey("RSAPrivateKey.txt", ios::in);
	ifstream ciphertext("ciphertext.txt", ios::in);
	if (!PriKey.is_open() || !ciphertext.is_open()) {
		cout << "Open file error!" << endl;
		return;
	}
	PriKey >> pri.p >> pri.q >> pri.a;
	ZZ c1, k_dec;
	ciphertext >> c1;

	k_dec = PowerMod(c1, pri.a, pri.p * pri.q);
	BinStr k_dec_binstr = ZZ_to_binstr(k_dec, AES_SIZE);
	unsigned char* new_k_char = new unsigned char[AES_SIZE / 8];
	new_k_char = (unsigned char*)toASCII(k_dec_binstr);

	cout << "已将c1解密为k\n";
	//aes.printHexArray(new_k_char, 16);

	//3.2 读取iv和c2,用k解密c2得到plain_m
	unsigned int c2_len;
	ciphertext >> c2_len;
	unsigned char* c2 = new unsigned char[c2_len];
	unsigned char* iv = new unsigned char[16];
	unsigned int ch;
	for (unsigned int i = 0; i < 16; i++)
	{
		ciphertext >> ch;
		iv[i] = (unsigned char)ch;
	}
	for (unsigned int i = 0; i < c2_len; i++)
	{
		ciphertext >> ch;
		c2[i] = (unsigned char)ch;
	}
	ciphertext.close();
	unsigned char* plain_m = new unsigned char[c2_len];
	plain_m = aes.DecryptCBC(c2, c2_len, new_k_char, iv);
	cout << "已将c2解密为明文\n";
	//3.3 输出明文m
	//cout << "plain_m= ";
	//aes.printHexArray(plain_m, m_len);

	ofstream plaintext("plaintext.txt", ios::out);
	if (!plaintext.is_open()) {
		cout << "Creat plaintext error!" << endl;
		return;
	}
	plaintext << plain_m;
	plaintext.close();
	cout << "解密完成！\n已写入plaintext.txt\n" << endl;
	delete[]new_k_char;
	delete[]plain_m;
	delete[]c2;
	delete[]iv;
}