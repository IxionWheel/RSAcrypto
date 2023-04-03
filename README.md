# 项目内容

## 项目简介
RSAcrypto——基于RSA公钥体制的加密解密程序

## 项目背景
现代密码学课程设计

# 程序功能
## RSA公钥体制流程介绍
Alice先使用功能1，生成公钥和私钥，并把公钥通过安全信道发送给Bob<br>
Bob使用功能2，把密文通过不安全信道发送给Alice<br>
Alice使用功能3，获得明文<br>

## 各部分功能介绍
### 功能1 生成密钥
会生成：<br>
RSAPrivateKey.txt-----------RSA私钥，用于解密<br>
RSAPublicKey.txt------------RSA公钥，用于加密

### 功能2 加密文件
需读取：<br>
m.txt--------------------------需要加密的文件<br>
RSAPublicKey.txt---------------RSA公钥<br>
会生成：<br>
ciphertext.txt-----------------密文

### 功能3 解密文件
需读取：<br>
RSAPrivateKey.txt--------------RSA私钥<br>
ciphertext.txt-----------------密文<br>
会生成：<br>
plaintext.txt------------------明文

# 程序设计
## 代码结构
RSAcrypto.cpp-------------------main 函数所在处，程序主体部分<br>
ansix917.cpp,ansix917.h---------ANSI X9.17CRPNG 的实现<br>
BinStr.cpp,BinStr.h-------------定义了一个 binstr 结构体和相关操作函数，用来表示一定长度的二进制串<br>
DES.cpp DES.h-------------------DES 加密解密过程的相关函数，用于 ANSI X9.17 算法<br>
AES.cpp AES.h-------------------AES 类，进行 AES 加密解密操作
