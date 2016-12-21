#ifndef __RSA_H__
#define __RSA_H__

#include <openssl/rsa.h>  
#include<openssl/pem.h>  
#include<openssl/err.h>  
#include <openssl/bio.h>  
#include <string>  
#ifdef WIN32
#ifdef _DLL
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#else
#pragma comment(lib, "libeay32s.lib")
#pragma comment(lib, "ssleay32s.lib")
#endif
#endif

#define	RSA_BITS 2048

inline std::string GetSaltString(int bytes=32)
{
	BIGNUM* bne = BN_new();
	//BN_set_word(bne,RSA_F4);
	BN_rand(bne, bytes*4, 0, rand());
	char * s = BN_bn2hex(bne);
	std::string ret(s);
	BN_clear_free(bne);
	return ret;
}

class CRSA
{
public:
	void SetPublicFile(const std::string& sFile) {m_sPublicFile = sFile;}
	const std::string& GetPublicFile(void) const {return m_sPublicFile;}
	void SetPrivateFile(const std::string& sFile) {m_sPrivateFile = sFile;}
	const std::string& GetPrivateFile(void) const {return m_sPrivateFile;}
	void SetPrivatePwd(const std::string& sPwd) {m_sPrivatePwd = sPwd;}
	const std::string& GetPrivatePwd(void) const {return m_sPrivatePwd;}

	void SetPublicKey(const std::string& sKey) {m_sPublicKey = sKey;}
	const std::string& GetPublicKey(void) const {return m_sPublicKey;}
	void SetPrivateKey(const std::string& sKey) {m_sPrivateKey = sKey;}
	const std::string& GetPrivateKey(void) const {return m_sPrivateKey;}

	void rsa_generatekey_file(int bits=RSA_BITS){
		/* 生成公钥 */  
		RSA* rsa = RSA_new();
		BIGNUM* bne = BN_new();
		BN_set_word(bne,RSA_F4);
		BN_rand(bne, bits, 0, rand());
		RSA_generate_key_ex(rsa,bits,bne,NULL);
		//RSA* rsa = RSA_generate_key( bits, RSA_F4, NULL, NULL);  
		BIO *bp = BIO_new( BIO_s_file() );  
		BIO_write_filename( bp, (void*)m_sPublicFile.c_str() );  
		PEM_write_bio_RSAPublicKey(bp, rsa);  
		BIO_free_all( bp );  
		/* 生成私钥 */  
		bp = BIO_new_file(m_sPrivateFile.c_str(), "w+");
		// EVP_des_ede3
		PEM_write_bio_RSAPrivateKey(bp, rsa, EVP_des_ede3_cbc(), NULL, 0, &pem_password_cb, (void*)m_sPrivatePwd.c_str());
		BIO_free_all( bp );
		RSA_free(rsa);
		BN_clear_free(bne);
	}
	void rsa_generatekey_mem(int bits=RSA_BITS){
		/* 生成公钥 */  
		RSA* rsa = RSA_new();
		BIGNUM* bne = BN_new();
		BN_set_word(bne,RSA_F4);
		BN_rand(bne, bits, 0, rand());
		RSA_generate_key_ex(rsa,bits,bne,NULL);
		//RSA* rsa = RSA_generate_key( bits, RSA_F4, NULL, NULL);  
		BIO *bp = BIO_new( BIO_s_mem() );  
		PEM_write_bio_RSAPublicKey(bp, rsa);
		BUF_MEM *buffer=NULL;
		BIO_get_mem_ptr(bp,&buffer);
		if (buffer!=NULL && buffer->length>0)
			m_sPublicKey = std::string(buffer->data,buffer->length);
		else
			m_sPublicKey.clear();
		BIO_free_all( bp );
		/* 生成私钥 */  
		bp = BIO_new( BIO_s_mem() );  
		// EVP_des_ede3
		PEM_write_bio_RSAPrivateKey(bp, rsa, EVP_des_ede3_cbc(), NULL, 0, &pem_password_cb, (void*)m_sPrivatePwd.c_str());
		buffer=NULL;
		BIO_get_mem_ptr(bp,&buffer);
		if (buffer!=NULL && buffer->length>0)
			m_sPrivateKey = std::string(buffer->data,buffer->length);
		else
			m_sPrivateKey.clear();
		BIO_free_all( bp );
		RSA_free(rsa);
		BN_clear_free(bne);
	}
	static int pem_password_cb(char *buf, int size, int rwflag, void *userdata)
	{
		if (userdata==NULL)	return 0;
		strcpy(buf,(const char*)userdata);
		return (int)strlen(buf);
	}
	bool rsa_open_private_file(void){
		if (m_pPrivateRSA == NULL)
		{
			//OpenSSL_add_all_algorithms();
			m_pPrivateBIO = BIO_new( BIO_s_file() );
			BIO_read_filename( m_pPrivateBIO, m_sPrivateFile.c_str() );  
			//m_pPrivateRSA = PEM_read_bio_RSAPrivateKey( m_pPrivateBIO, NULL, NULL, NULL);  
			m_pPrivateRSA = PEM_read_bio_RSAPrivateKey( m_pPrivateBIO, NULL, &pem_password_cb, (void*)m_sPrivatePwd.c_str());  
			//PEM_read_bio_PrivateKey(m_pPrivateBIO,NULL
			//PEM_read_PrivateKey(
			if (m_pPrivateRSA==NULL)
			{
				BIO_free_all( m_pPrivateBIO );  
				m_pPrivateBIO = NULL;
			}
		}
		return m_pPrivateRSA==NULL?false:true;
	}
	bool rsa_open_private_mem(void){
		if (m_pPrivateRSA == NULL)
		{
			//OpenSSL_add_all_algorithms();
			m_pPrivateBIO = BIO_new( BIO_s_mem() );
			BIO_puts( m_pPrivateBIO, m_sPrivateKey.c_str());  
			m_pPrivateRSA = PEM_read_bio_RSAPrivateKey( m_pPrivateBIO, NULL, &pem_password_cb, (void*)m_sPrivatePwd.c_str());  
			if (m_pPrivateRSA==NULL)
			{
				BIO_free_all( m_pPrivateBIO );  
				m_pPrivateBIO = NULL;
			}
		}
		return m_pPrivateRSA==NULL?false:true;
	}
	void rsa_close_private(void){
		//CRYPTO_cleanup_all_ex_data();  
		if (m_pPrivateBIO!=NULL)
		{
			BIO_free_all(m_pPrivateBIO);
			m_pPrivateBIO = NULL;
		}
		if (m_pPrivateRSA!=NULL)
		{
			RSA_free(m_pPrivateRSA);
			m_pPrivateRSA = NULL;
		}
	}
	bool rsa_open_public_file(void){
		if (m_pPublicRSA == NULL)
		{
			//OpenSSL_add_all_algorithms();
			m_pPublicBIO = BIO_new( BIO_s_file() );
			BIO_read_filename( m_pPublicBIO, m_sPublicFile.c_str() );  
			m_pPublicRSA = PEM_read_bio_RSAPublicKey( m_pPublicBIO, NULL, NULL, NULL);  
			//r1 = PEM_read_bio_RSA_PUBKEY(key1, NULL,NULL,NULL);
			//openssl_to_keys(r, 1024, priv, pub);
			if (m_pPublicRSA==NULL)
			{
				BIO_free_all( m_pPublicBIO );  
				m_pPublicBIO = NULL;
			}
		}
		return m_pPublicRSA==NULL?false:true;
	}
	bool rsa_open_public_mem(void){
		if (m_pPublicRSA == NULL)
		{
			//OpenSSL_add_all_algorithms();
			m_pPublicBIO = BIO_new( BIO_s_mem() );
			BIO_puts( m_pPublicBIO, m_sPublicKey.c_str() );  
			m_pPublicRSA = PEM_read_bio_RSAPublicKey( m_pPublicBIO, NULL, NULL, NULL);
			//m_pPublicRSA = PEM_read_bio_PUBKEY(m_pPublicBIO, NULL, NULL, NULL);
			if (m_pPublicRSA==NULL)
			{
				BIO_free_all( m_pPublicBIO );  
				m_pPublicBIO = NULL;
			}
		}
		return m_pPublicRSA==NULL?false:true;
	}
	void rsa_close_public(void){
		//CRYPTO_cleanup_all_ex_data();  
		if (m_pPublicBIO!=NULL)
		{
			BIO_free_all(m_pPublicBIO);
			m_pPublicBIO = NULL;
		}
		if (m_pPublicRSA!=NULL)
		{
			RSA_free(m_pPublicRSA);
			m_pPublicRSA = NULL;
		}
	}
	int rsa_public_encrypt(const unsigned char* fdata,int flen,unsigned char** pToData){
		if (m_pPublicRSA == NULL)
			return -1;
		const int nLen = RSA_size(m_pPublicRSA);  
		unsigned char *pEncode = new unsigned char[nLen + 1];
		memset(pEncode,0,nLen + 1);
		const int ret = RSA_public_encrypt(flen,fdata,pEncode,m_pPublicRSA,RSA_PKCS1_PADDING);	// RSA_NO_PADDING(error)
		if (ret >= 0)
		{  
			pEncode[ret] = '\0';
			*pToData = pEncode;
		}else
		{
			delete[] pEncode;  
		}
		return ret;  
	}  
	int rsa_public_decrypt(const unsigned char* fdata,int flen,unsigned char** pToData){
		if (m_pPublicRSA == NULL)
			return -1;
		const int nLen = RSA_size(m_pPublicRSA);  
		unsigned char *pEncode = new unsigned char[nLen + 1];  
		const int ret = RSA_public_decrypt(flen,fdata,pEncode,m_pPublicRSA,RSA_PKCS1_PADDING);
		if (ret >= 0)
		{  
			pEncode[ret] = '\0';
			*pToData = pEncode;
		}else
		{
			delete[] pEncode;
		}
		return ret;  
	}  

	// **RSA_NO_PADDING 不能加密
	// **RSA_PKCS1_PADDING 可以加密
	// PKCS#1中主要提供了两种加密方案，RSAEX- OAEP和PSAES-PKCS1-v1_5
	// （反正就是两种加密过程了，有点复杂，它主要是先对先对需要加密的数据进行了编码，比如RSAES-OAEP 采用EME-OAEP编码，再进行加密或解密）
	int rsa_private_encrypt(const unsigned char* fdata,int flen,unsigned char** pToData){
		if (m_pPrivateRSA == NULL)
			return -1;
		const int nLen = RSA_size(m_pPrivateRSA);
		unsigned char *pEncode = new unsigned char[nLen + 1];  
		const int ret = RSA_private_encrypt(flen,fdata,pEncode,m_pPrivateRSA,RSA_PKCS1_PADDING);  
		if (ret >= 0)
		{
			pEncode[ret] = '\0';
			*pToData = pEncode;
		}else
		{
			delete[] pEncode;
		}
		return ret;
	}
	int rsa_private_decrypt(const unsigned char* fdata,int flen,unsigned char** pToData){
		if (m_pPrivateRSA == NULL)
			return -1;
		const int nLen = RSA_size(m_pPrivateRSA);
		unsigned char *pEncode = new unsigned char[nLen + 1];  
		const int ret = RSA_private_decrypt(flen,fdata,pEncode,m_pPrivateRSA,RSA_PKCS1_PADDING);  
		if (ret >= 0)
		{
			pEncode[ret] = '\0';
			*pToData = pEncode;
		}else
		{
			delete[] pEncode;
		}
		return ret;
	}  
	std::string rsa_private_decrypt(const unsigned char* fdata,int flen){
		std::string sRet;
		unsigned char *pEncode = NULL;
		const int ret = rsa_private_decrypt(fdata,flen,&pEncode);
		if (pEncode!=NULL)
		{
			if (ret>0)
				sRet = std::string((const char*)pEncode,ret);
			delete[] pEncode;
		}
		return sRet;
	}  

	CRSA(void)
		: m_sPublicFile("public.pem"), m_sPrivateFile("private.pem")
		, m_pPrivateBIO(NULL), m_pPrivateRSA(NULL)
		, m_pPublicBIO(NULL), m_pPublicRSA(NULL)
	{}
	~CRSA(void){
		rsa_close_private();
		rsa_close_public();
	}

private:
	std::string m_sPublicFile;
	std::string m_sPrivateFile;
	std::string m_sPrivatePwd;
	std::string m_sPublicKey;	// mem
	std::string m_sPrivateKey;	// mem

	BIO* m_pPrivateBIO;
	RSA* m_pPrivateRSA;
	BIO* m_pPublicBIO;
	RSA* m_pPublicRSA;
};

#endif __RSA_H__
