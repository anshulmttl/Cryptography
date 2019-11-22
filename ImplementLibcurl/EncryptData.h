#pragma once
#include <iostream>
#include <string>
using namespace std;

class EncryptData
{
public:
	virtual string Encrypt(string inputString) = 0;
	virtual string Decrypt(string encryptedString) = 0;
};

class EncryptDataAES : public EncryptData
{
public:
	string Encrypt(string inputString);
	string Decrypt(string encryptedString);
};