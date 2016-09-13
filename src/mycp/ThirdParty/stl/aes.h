#ifndef __AES_H__
#define __AES_H__

#include <openssl/evp.h>  
#include <openssl/aes.h>  
#include <openssl/rand.h>  
#ifdef WIN32
#pragma comment(lib, "libeay32.lib")  
#pragma comment(lib, "ssleay32.lib")  
#endif

// ****ECB模式
inline int aes_ecb_encrypt_full(const unsigned char *userKey,int klen,const unsigned char* in,int ilen,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_encrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	for (int i=0;i<ilen; i+=16)
		AES_ecb_encrypt(in+i,out+i,&key,AES_ENCRYPT);
	return 0;
}
inline int aes_ecb_encrypt(const unsigned char *userKey,int klen,const unsigned char* in,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_encrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	AES_ecb_encrypt(in,out,&key,AES_ENCRYPT);
	return 0;
}
inline int aes_ecb_decrypt_full(const unsigned char *userKey,int klen,const unsigned char* in,int ilen,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_decrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	for (int i=0;i<ilen; i+=16)
		AES_ecb_encrypt(in+i,out+i,&key,AES_DECRYPT);
	return 0;
}
inline int aes_ecb_decrypt(const unsigned char *userKey,int klen,const unsigned char* in,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_decrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	AES_ecb_encrypt(in,out,&key,AES_DECRYPT);
	return 0;
}
// ****CBC模式简介
inline int aes_cbc_encrypt_full(const unsigned char *userKey,int klen,const unsigned char* in,int length,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_encrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	//Init vector.  
	unsigned char   iv[AES_BLOCK_SIZE * 4];
	memset(iv,0,sizeof(iv));
	for (int i=0;i<length; i+=16)
		AES_cbc_encrypt(in+i,out+i,((length-i)>=16)?16:(length%16),&key,iv,AES_ENCRYPT);
	return 0;
}
inline int aes_cbc_encrypt(const unsigned char *userKey,int klen,const unsigned char* in,int length,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_encrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	//Init vector.  
	unsigned char   iv[AES_BLOCK_SIZE * 4];
	memset(iv,0,sizeof(iv));
	AES_cbc_encrypt(in,out,length,&key,iv,AES_ENCRYPT);
	return 0;
}
inline int aes_cbc_decrypt_full(const unsigned char *userKey,int klen,const unsigned char* in,int length,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_decrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	//Init vector.  
	unsigned char   iv[AES_BLOCK_SIZE * 4];
	memset(iv,0,sizeof(iv));
	for (int i=0;i<length; i+=16)
		AES_cbc_encrypt(in+i,out+i,16,&key,iv,AES_DECRYPT);
	return 0;
}
inline int aes_cbc_decrypt(const unsigned char *userKey,int klen,const unsigned char* in,int length,unsigned char* out)
{
	// 如果密钥数字是16个字节，16*8=128；则此参数值应为128；
	// 返回值： 0 成功， -1 userkey，key为空， -2： 密钥长度不是128(16*8)，192(24*8)，256(32*8)；
	int ret = 0;
	AES_KEY         key;
	ret = AES_set_decrypt_key(userKey, klen*8, &key);
	if (ret!=0)
		return ret;
	//Init vector.  
	unsigned char   iv[AES_BLOCK_SIZE * 4];
	memset(iv,0,sizeof(iv));
	AES_cbc_encrypt(in,out,length,&key,iv,AES_DECRYPT);
	return 0;
}


//void aes_cbc_encrypt(const unsigned char *userKey,int klen,const unsigned char* in,int length,unsigned char* out)
//{
//	int ret = 0;
//	AES_KEY         key;
//	ret = AES_set_encrypt_key(userKey, klen*8, &key);
//	if (ret!=0)
//		return ret;
//	AES_cbc_encrypt(in,out,length,&key,AES_ENCRYPT);
//	return 0;
//}
//
//int aestest(void)
//{
//
//	//128bits key.  
//	unsigned char   rkey[16];  
//	//Internal key.  
//	AES_KEY         key;  
//
//	//Testdata.  
//	// [yasi] Make static content instead of random text  
//	unsigned char   plaintext[AES_BLOCK_SIZE * 4] =  
//	{  
//		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i',  
//		'0', '1', '2', '3', '4', '5', '6', '7', '0', '1', '2', '3', '4', '5', '6', '7',  
//		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'i',  
//		'0', '1', '2', '3', '4', '5', '6', '7', '0', '1', '2', '3', '4', '5', '6', '7'  
//	};  
//	unsigned char   ciphertext[AES_BLOCK_SIZE * 4];  
//	unsigned char   checktext[AES_BLOCK_SIZE * 4];  
//
//	//Init vector.  
//	unsigned char   iv[AES_BLOCK_SIZE * 4];  
//	//Save vector.  
//	unsigned char   saved_iv[AES_BLOCK_SIZE * 4];  
//
//	int nr_of_bits = 0;  
//	int nr_of_bytes = 0;  
//
//	//Zeror buffer.  
//	memset(ciphertext, 0, sizeof ciphertext);  
//	memset(checktext, 0, sizeof checktext);  
//
//	//Generate random  
//	RAND_pseudo_bytes(rkey, sizeof rkey);  
//	RAND_pseudo_bytes(saved_iv, sizeof saved_iv);  
//
//	//hexdump(stdout, "== rkey ==",  rkey,  sizeof(rkey));  
//	//hexdump(stdout, "== iv ==",  saved_iv,  sizeof(saved_iv));  
//	//printf("\n");  
//
//	//hexdump(stdout, "== plaintext ==",  plaintext,  sizeof(plaintext));  
//	//printf("\n");  
//
//	//Entrypt  
//	memcpy(iv, saved_iv, sizeof(iv));  
//	nr_of_bits = 8 * sizeof(rkey);  
//	AES_set_encrypt_key(rkey, nr_of_bits, &key);  
//	nr_of_bytes = sizeof(plaintext);  
//	AES_cbc_encrypt(plaintext,  
//		ciphertext,  
//		nr_of_bytes,  
//		&key,  
//		iv,  
//		AES_ENCRYPT);  
//
//	//hexdump(stdout, "== ciphertext ==",  ciphertext,  sizeof(ciphertext));  
//	//printf("\n");  
//	//// [yasi] iv is changed in encryption  
//	//hexdump(stdout, "== iv changed ==",  iv,  sizeof(iv));  
//	//printf("\n");  
//
//	//Decrypt  
//	memcpy(iv, saved_iv, sizeof(iv));       // [yasi] without this line, decrypt will fail because iv is changed in encryption  
//	nr_of_bits = 8 * sizeof(rkey);  
//	AES_set_decrypt_key(rkey, nr_of_bits, &key);  
//	nr_of_bytes = sizeof(ciphertext);  
//
//	AES_cbc_encrypt(ciphertext,  
//		checktext,  
//		nr_of_bytes,  
//		&key, iv,  
//		AES_DECRYPT);  
//	//hexdump(stdout, "== checktext ==",  checktext,  sizeof(checktext));  
//	//printf("\n");  
//
//	return 0;  
//}
//
//void handleErrors(void)  
//{  
//	ERR_print_errors_fp(stderr);  
//	abort();  
//}  
//
//int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,  
//			unsigned char *iv, unsigned char *ciphertext)  
//{  
//	EVP_CIPHER_CTX *ctx;  
//
//	int len;  
//
//	int ciphertext_len;  
//
//	/* Create and initialise the context */  
//	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
//
//	/* Initialise the encryption operation. IMPORTANT - ensure you use a key 
//	* and IV size appropriate for your cipher 
//	* In this example we are using 256 bit AES (i.e. a 256 bit key). The 
//	* IV size for *most* modes is the same as the block size. For AES this 
//	* is 128 bits */  
//	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  
//		handleErrors();  
//
//	/* Provide the message to be encrypted, and obtain the encrypted output. 
//	* EVP_EncryptUpdate can be called multiple times if necessary 
//	*/  
//	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))  
//		handleErrors();  
//	ciphertext_len = len;  
//
//	/* Finalise the encryption. Further ciphertext bytes may be written at 
//	* this stage. 
//	*/  
//	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();  
//	ciphertext_len += len;  
//
//	/* Clean up */  
//	EVP_CIPHER_CTX_free(ctx);  
//
//	return ciphertext_len;  
//}  
//
//int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,  
//			unsigned char *iv, unsigned char *plaintext)  
//{  
//	EVP_CIPHER_CTX *ctx;  
//
//	int len;  
//
//	int plaintext_len;  
//
//	/* Create and initialise the context */  
//	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();  
//
//	/* Initialise the decryption operation. IMPORTANT - ensure you use a key 
//	* and IV size appropriate for your cipher 
//	* In this example we are using 256 bit AES (i.e. a 256 bit key). The 
//	* IV size for *most* modes is the same as the block size. For AES this 
//	* is 128 bits */  
//	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  
//		handleErrors();  
//
//	/* Provide the message to be decrypted, and obtain the plaintext output. 
//	* EVP_DecryptUpdate can be called multiple times if necessary 
//	*/  
//	if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))  
//		handleErrors();  
//	plaintext_len = len;  
//
//	/* Finalise the decryption. Further plaintext bytes may be written at 
//	* this stage. 
//	*/  
//	if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();  
//	plaintext_len += len;  
//
//	/* Clean up */  
//	EVP_CIPHER_CTX_free(ctx);  
//
//	return plaintext_len;  
//}  

#endif __AES_H__
