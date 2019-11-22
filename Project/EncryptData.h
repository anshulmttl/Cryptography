#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Base class for encryotion. Contain virtual functions only
class EncryptData
{
public :
	virtual std::vector<unsigned char> Encrypt(char *inputBytes, int numberOfBytes) = 0;
	virtual std::vector<unsigned char> Decrypt(char *inputBytes, int numberOfBytes) = 0;
};

// DES_EDE3 encryption
class EncryptTripleDES : public EncryptData
{
public:
    std::vector<unsigned char> Encrypt(char *inputBytes, int numberOfBytes);
	std::vector<unsigned char> Decrypt(char *inputBytes, int numberOfBytes);
};

// AES encryotion
class EncryptAES : public EncryptData
{
public:
	std::vector<unsigned char> Encrypt(char *inputBytes, int numberOfBytes);
	std::vector<unsigned char> Decrypt(char *inputBytes, int numberOfBytes);
};