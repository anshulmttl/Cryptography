// KeyGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

/*#####################################################
  ##### GENERATE KEY AND IV FOR AES / TRIPLE DES #####
  ####################################################*/

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <string>

#include "cryptlib.h"
using CryptoPP::Exception;

#include "aes.h"
#include "des.h"
using namespace CryptoPP;

#include <sstream>
#include <iomanip>

std::string hexStr(byte *data, int len)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');

	for(int i(0); i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << "0x" << static_cast<int>(data[i]) << ", ";

	return ss.str();
}

void GenerateKeyAndivAES()
{
	AutoSeededRandomPool prng;
	byte key_aes[AES::DEFAULT_KEYLENGTH];
	prng.GenerateBlock(key_aes, sizeof(key_aes));

	byte iv_aes[AES::BLOCKSIZE];
	prng.GenerateBlock(iv_aes, sizeof(iv_aes));

	std::string strKey;
	strKey = hexStr(key_aes, AES::DEFAULT_KEYLENGTH);
	cout << "byte key[] = {" << strKey << " }" << endl;
	
	std::string striv;
	striv = hexStr(iv_aes, AES::BLOCKSIZE);
	cout << "byte iv[] = {" << striv << " }" << endl;
}

// Use if required to generate DES_EDE3 key and iv
void GenerateKeyAndivDES_EDE3()
{
	AutoSeededRandomPool prng;
	byte key_DES_EDE3[DES_EDE3::DEFAULT_KEYLENGTH];
	prng.GenerateBlock(key_DES_EDE3, sizeof(key_DES_EDE3));

	byte iv_DES_EDE3[DES_EDE3::BLOCKSIZE];
	prng.GenerateBlock(iv_DES_EDE3, sizeof(iv_DES_EDE3));

	std::string strKey;
	strKey = hexStr(key_DES_EDE3, DES_EDE3::DEFAULT_KEYLENGTH);
	cout << "byte key[] = {" << strKey << " }" << endl;

	std::string striv;
	striv = hexStr(iv_DES_EDE3, DES_EDE3::BLOCKSIZE);
	cout << "byte iv[] = {" << striv << " }" << endl;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//GenerateKeyAndivAES(); //Use for key and iv of AES algorithm
	GenerateKeyAndivDES_EDE3(); //Use for key and iv of DES_EDE3 algorithm (TripleDES)
	return 0;
}

