#include "stdafx.h"
#include "EncryptData.h"
#include "cryptlib.h"
#include "filters.h"
#include "files.h"
#include "modes.h"
#include "hex.h"
#include "aes.h"

using namespace CryptoPP;

/*############################## AES KEY and IV ################################*/
// Generate key and iv using KeyGen. 
// Replace following values key_aes and iv_aes after generating keygen
unsigned char key_aes[] = {0x77, 0x66, 0x26, 0x82, 0xa5, 0xaf, 0x9a, 0xa0, 0xc0, 0x72, 0x60, 0x6e, 0x2a, 0x82, 0x90, 0xcb};
unsigned char iv_aes[] = {0x70, 0x18, 0x2f, 0x4c, 0xe9, 0x48, 0x3c, 0x7d, 0x8c, 0x68, 0x47, 0x90, 0xe8, 0x6a, 0x89, 0x45};

string EncryptDataAES::Encrypt(string inputString)
{
	string cipher, encoded, recovered;

	try{
			CBC_Mode<AES>::Encryption enc;
			enc.SetKeyWithIV(key_aes, sizeof(key_aes), iv_aes);

			StringSource s(inputString, true, 
				new StreamTransformationFilter(enc,
				new StringSink(cipher)));

#if 0
			StreamTransformationFilter filter(enc);
			filter.Put((const byte*)inputString.data(), inputString.size());
			filter.MessageEnd();

			const size_t ret = filter.MaxRetrievable();
			cipher.resize(ret);
			filter.Get((byte*)cipher.data(), cipher.size());
#endif

	}
	catch(const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
		exit(1);
	}

	return cipher;
}

string EncryptDataAES::Decrypt(string encryptedString)
{
	string recovered;

	try{
			CBC_Mode<AES>::Decryption dec;
			dec.SetKeyWithIV(key_aes, sizeof(key_aes), iv_aes);

			StringSource s(encryptedString, true,
				new StreamTransformationFilter(dec,
				new StringSink(recovered)));

#if 0
			StreamTransformationFilter filter(dec);
			filter.Put((const byte*)encryptedString.data(),encryptedString.size());
			filter.MessageEnd();

			const size_t ret = filter.MaxRetrievable();
			recovered.resize(ret);
			filter.Get((byte*)recovered.data(), recovered.size());
#endif
	}
	catch(CryptoPP::Exception &e)
	{
		exit(1);
	}

	return recovered;
}