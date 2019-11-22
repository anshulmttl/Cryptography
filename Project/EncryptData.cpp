#include "stdafx.h"
#include "EncryptData.h"

#include "cryptlib.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "hex.h"
#include "des.h"
#include "aes.h"
using namespace CryptoPP;

/*############################## AES KEY and IV ################################*/
// Generate key and iv using KeyGen. 
// Replace following values key_aes and iv_aes after generating keygen
unsigned char key_aes[] = {0x77, 0x66, 0x26, 0x82, 0xa5, 0xaf, 0x9a, 0xa0, 0xc0, 0x72, 0x60, 0x6e, 0x2a, 0x82, 0x90, 0xcb};
unsigned char iv_aes[] = {0x70, 0x18, 0x2f, 0x4c, 0xe9, 0x48, 0x3c, 0x7d, 0x8c, 0x68, 0x47, 0x90, 0xe8, 0x6a, 0x89, 0x45};

/*############################# DES_EDE3 KEY and IV ############################*/
// Generate key and iv using keygen
// Replace following values key_DES_EDE3 and iv_DES_EDE3 after generating keygen
unsigned char key_DES_EDE3[] = {0x38, 0x7f, 0xfd, 0xd4, 0x56, 0x26, 0xc5, 0xbc, 0xef, 0x1e, 0xd5, 0xb2, 0x74, 0x71, 0xbe, 0xc7, 0x86, 0xc9, 0xbb, 0x2b, 0x9d, 0xcf, 0xca, 0x6b};
unsigned char iv_DES_EDE3[] = {0xa6, 0xbb, 0xba, 0x52, 0xf6, 0x11, 0xd7, 0xf3};

std::vector<unsigned char> EncryptTripleDES::Encrypt(char *inputBytes, int numberOfBytes)
{
	//unsigned char key[DES_EDE3::MAX_KEYLENGTH];
	//unsigned char iv[DES_EDE3::BLOCKSIZE];
	std::vector<unsigned char> plain(inputBytes, inputBytes + numberOfBytes), cipher;
	//memset (key, 0x00, sizeof(key));
	//memset (iv, 0x00, sizeof(iv));

	// Encryption
	CBC_Mode<DES_EDE3>::Encryption enc;
	enc.SetKeyWithIV(key_DES_EDE3, sizeof(key_DES_EDE3), iv_DES_EDE3);

	cipher.resize(plain.size() + DES_EDE3::BLOCKSIZE);
	ArraySink cs(&cipher[0], cipher.size());

	ArraySource(plain.data(), plain.size(), true, new StreamTransformationFilter(enc, new Redirector(cs)));
	cipher.resize(cs.TotalPutLength());

	return cipher;
}

std::vector<unsigned char> EncryptTripleDES::Decrypt(char *inputBytes, int numberOfBytes)
{
	//unsigned char key[DES_EDE3::MAX_KEYLENGTH];
	//unsigned char iv[DES_EDE3::BLOCKSIZE];
	std::vector<unsigned char> plain(inputBytes, inputBytes + numberOfBytes), decrpyted;
	//memset (key, 0x00, sizeof(key));
	//memset (iv, 0x00, sizeof(iv));

	// Encryption
	CBC_Mode<DES_EDE3>::Decryption enc;
	enc.SetKeyWithIV(key_DES_EDE3, sizeof(key_DES_EDE3), iv_DES_EDE3);

	decrpyted.resize(plain.size());
	ArraySink cs(&decrpyted[0], decrpyted.size());

	ArraySource(plain.data(), plain.size(), true, new StreamTransformationFilter(enc, new Redirector(cs)));
	decrpyted.resize(cs.TotalPutLength());

	return decrpyted;
}

std::vector<unsigned char> EncryptAES::Encrypt(char *inputBytes, int numberOfBytes)
{
	//unsigned char key[AES::MAX_KEYLENGTH];
	//unsigned char iv[AES::BLOCKSIZE];
	std::vector<unsigned char> plain(inputBytes, inputBytes + numberOfBytes), cipher;
	//memset (key, 0x00, sizeof(key));
	//memset (iv, 0x00, sizeof(iv));

	// Encryption
	CBC_Mode<AES>::Encryption enc;
	enc.SetKeyWithIV(key_aes, sizeof(key_aes), iv_aes);

	cipher.resize(plain.size() + AES::BLOCKSIZE);
	ArraySink cs(&cipher[0], cipher.size());

	ArraySource(plain.data(), plain.size(), true, new StreamTransformationFilter(enc, new Redirector(cs)));
	cipher.resize(cs.TotalPutLength());

	return cipher;
}

std::vector<unsigned char> EncryptAES::Decrypt(char *inputBytes, int numberOfBytes)
{
	//unsigned char key[AES::MAX_KEYLENGTH];
	//unsigned char iv[AES::BLOCKSIZE];
	std::vector<unsigned char> plain(inputBytes, inputBytes + numberOfBytes), decrpyted;
	//memset (key, 0x00, sizeof(key));
	//memset (iv, 0x00, sizeof(iv));

	// Decryption
	CBC_Mode<AES>::Decryption enc;
	enc.SetKeyWithIV(key_aes, sizeof(key_aes), iv_aes);

	decrpyted.resize(plain.size());
	ArraySink cs(&decrpyted[0], decrpyted.size());

	ArraySource(plain.data(), plain.size(), true, new StreamTransformationFilter(enc, new Redirector(cs)));
	decrpyted.resize(cs.TotalPutLength());

	return decrpyted;
}