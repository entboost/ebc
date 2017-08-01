#ifndef __PBKDF2_H__
#define __PBKDF2_H__
#include <math.h>  

class CPBKDF2
{
public:
	CPBKDF2(void){}

	// for SHA1,HMAC
	std::string GetHString(void) const
	{
		std::string sHString;
		char lpszBuffer[18];
		for(int i=0;i<=4;++i)
		{
			sprintf(lpszBuffer,"%x",(unsigned int)H[i]);
			sHString.append(lpszBuffer);
		}
		return sHString;
	}

	unsigned long H[5];
	unsigned long T[512];

	unsigned long f(unsigned long B,unsigned long C,unsigned long D, int t)  
	{  
		if (t < 20)  
		{  
			return ((B & C)^((~B) & D));  
		}  
		if ((t > 19) & (t < 40))  
		{  
			return (B ^ C ^ D);  
		}  
		if ((t > 39) & (t < 60))  
		{  
			return ((B & C)^(B & D)^(C & D));  
		}   
		if (t > 59)  
		{  
			return (B ^ C ^ D);  
		}  
		return B;  
	}  

	unsigned long Rol(unsigned long x, int y)  
	{  
		if (y % 32 == 0) {return x;}   
		else {return ((x << y)^(x >> -y));}  
	}  
	unsigned long Ror(unsigned long x, int y)  
	{  
		if (y % 32 == 0) {return x;}  
		else {return ((x >> y)^(x << -y));}  
	}  
	//inline void append(char* s, int slen, char c)  
	//{  
	//	//int len = strlen(s);  
	//	s[slen] = c;  
	//	s[slen+1] = '\0';  
	//}  

	void SHA1(const char* s,int slen)  
	{  
		unsigned long K[80];  
		unsigned long A,B,C,D,E,TEMP;  
		int r,k,ln,t,l,i,j;  
		H[0]=0x67452301;  
		H[1]=0xefcdab89;  
		H[2]=0x98badcfe;  
		H[3]=0x10325476;  
		H[4]=0xc3d2e1f0;  

		ln=slen;  
		r = (int)((ln+1)/64);  

		if (((ln+1) % 64) > 56)  
		{  
			r=r+1;  
		}  

		// initialize Constants  
		for(t=0; t<80; t++)  
		{  
			if (t<20)  
			{  
				K[t] = 0x5a827999;  
			}  


			if ((t>19)&(t<40))  
			{  
				K[t] = 0x6ED9EBA1;  
			}  
			if ((t>39)&(t<60))  
			{  
				K[t] = 0x8F1BBCDC;  
			}  
			if (t>59)  
			{  
				K[t] = 0xca62c1d6;  
			}  
		}  


		for(l=0; l <= r; l++)  
		{  
			unsigned long W[80]={0};  
			//Initialize Text  
			for (i=0; i<16; i++)  
			{  
				for(j=0; j<4; j++)  
				{  
					if (4*i+j <= ln)  
					{  
						k = s[64*l+4*i+j];  
					}  
					else  
					{  
						k =0;  
					}  

					if (k<0)  
					{  
						k = k +256;  
					}  


					if (4*i+j == ln)  
					{  
						k = 0x80;  
					}  


					W[i]= W[i] + k*(unsigned long)pow(256.0,(double)3-j);  
				}  
			}  
			if ((W[14]==0)&(W[15]==0))  
			{  
				W[15]=8*slen;  
			}  


			// Hash Cycle  


			for (t = 16; t <80; t++)  
			{  
				W[t] = Rol(W[t-3]^W[t-8]^W[t-14]^W[t-16],1);  
			}  


			A = H[0];  
			B = H[1];  
			C = H[2];  
			D = H[3];  
			E = H[4];  


			for(t = 0; t < 80; t++)  
			{  
				TEMP = Rol(A,5) + f(B,C,D,t) + E + W[t] + K[t];  
				E = D;  
				D = C;  
				C = Rol(B,30);  
				B = A;    
				A = TEMP;  
			}  


			H[0] = H[0] + A;  
			H[1] = H[1] + B;  
			H[2] = H[2] + C;  
			H[3] = H[3] + D;  
			H[4] = H[4] + E;  

			ln = ln - 64;  
		}
	}   

	void HMAC(const char* text, int tlen, const char* key, int klen)  
	{  
		char c;  
		char s[256] = "";
		int slen = 0;  
		unsigned long Key[16] = {0};  
		unsigned long X[16] = {0};  
		unsigned long Y[16] = {0};  
		unsigned long ipad = 0x36363636;  
		unsigned long opad = 0x5c5c5c5c;  
		int k,i,j;  

		//Process string key into sub-key  
		//Hash key in case it is less than 64 bytes  
		if (klen > 64)  
		{  
			SHA1(key, klen);  
			Key[0] = H[0];  
			Key[1] = H[1];  
			Key[2] = H[2];  
			Key[3] = H[3];  
			Key[4] = H[4];  
		}  
		else  
		{  
			for(i=0; i<16; i++)  
			{  
				for(j=0; j<4; j++)  
				{  
					if (4*i+j <= klen)  
					{  
						k = key[4*i+j];  
					}  
					else  
					{  
						k = 0;  
					}  
					if (k<0)  
					{  
						k = k + 256;  
					}  
					Key[i]= Key[i] + k*(unsigned long)pow(256.0,(double)3-j);  
				}  
			}  
		}  

		for(i=0; i<16; i++)  
		{  
			X[i] = Key[i]^ipad;  
			Y[i] = Key[i]^opad;  
		}  

		//Turn X-Array into a String  
		for(i=0; i<16; i++)  
		{  
			for(j=0; j<4; j++)  
			{  
				c = ((X[i] >> 8*(3-j)) % 256);  
				s[slen] = c;
				//s = s + c;  
				//append(s,slen,c);  
				slen++;  
			}  
		}  

		//Append text to string  
		memcpy(s+slen,text,tlen);
		slen = slen + tlen;  

		//Hash X-Array  
		SHA1(s,slen);  

		memset(s,0,sizeof(s));
		slen = 0;  

		//Turn Y-Array into a String  
		for(i=0; i<16; i++)  
		{  
			for(j=0; j<4; j++)  
			{  
				c = ((Y[i] >> 8*(3-j)) % 256);  
				s[slen] = c;
				//s = s + c;  
				//append(s,slen,c);  
				slen++;  
			}  
		}

		//Append Hashed X-Array to Y-Array in string  
		for(i=0; i<5; i++)  
		{  
			for(j=0; j<4; j++)  
			{  
				c = ((H[i] >> 8*(3-j)) % 256);  
				s[slen] = c;
				//s = s + c;  
				//append(s,slen,c);  
				slen++;  
			}  
		}  
		//Hash final concatenated string  
		SHA1(s,slen);  
	}
	// c=10000
	// dkLen=64(128B)/128(256B)/256(512B)
	std::string PBKDF2(const char P[],int Plen, const char S[], int Slen, int c,int dkLen)  
	{  
		unsigned long L[5]={0,0,0,0,0}; 
		int hlen,l,r,i,j,x,y;  
		char t;  

		hlen = 20;                          // from sha1  
		//requires new version of Ceiling Function: l = ceil(dkLen/hlen)  
		l = (dkLen/hlen)+1;                 // hLen-octets 32/20 = 2 always  
		r = dkLen -(l-1)*hlen;              // number of octets in the last block (32-20) = 12  

		char * U = new char[Slen+10];
		for(i=1; i<=l; i++)                  // passes  
		{
			/////////  
			char appended[] = {(char)0, (char)0, (char)0, (char)i};
			memcpy(U,S,Slen);
			memcpy(U+Slen,&appended,4);
			int Ulen = Slen + 4;  

			memset(L,0,sizeof(L));
			//L[0] = L[1] = L[2] = L[3] = L[4] = 0;  
			for(j=1; j<=c; j++)          // number of iterations (4096)  
			{
				HMAC(U,Ulen,P,Plen);  
				L[0] = L[0]^H[0];       // ^ is the XOR function  
				L[1] = L[1]^H[1];  
				L[2] = L[2]^H[2];  
				L[3] = L[3]^H[3];  
				L[4] = L[4]^H[4];  

				memset(U,0,sizeof(U));
				Ulen=0;  

				for(x=0; x<5; x++)  
				{
					for(y=0; y<4; y++)
					{
						t = ((H[x] >> 8*(3-y)) % 256);
						U[Ulen] = t;
						Ulen++;
					}  
				}  
			}  

			T[5*(i-1)] = L[0];  // moves the results down 5 each iteration  
			T[5*(i-1)+1] = L[1];  
			T[5*(i-1)+2] = L[2];  
			T[5*(i-1)+3] = L[3];  
			T[5*(i-1)+4] = L[4];  
		}
		delete[] U;

		std::string sPBKDF2;
		char lpszBuffer[18];
		for(int j=0; j<(dkLen/4); j++)
		{
			sprintf(lpszBuffer,"%08x",(unsigned int)T[j]);
			sPBKDF2.append(lpszBuffer);
		}
		return sPBKDF2;
	}
};

/*
unsigned long H[5];
unsigned long T[512]={0};

unsigned long f(unsigned long B,unsigned long C,unsigned long D, int t)  
{  
	if (t < 20)  
	{  
		return ((B & C)^((~B) & D));  
	}  
	if ((t > 19) & (t < 40))  
	{  
		return (B ^ C ^ D);  
	}  
	if ((t > 39) & (t < 60))  
	{  
		return ((B & C)^(B & D)^(C & D));  
	}   
	if (t > 59)  
	{  
		return (B ^ C ^ D);  
	}  
	return B;  
}  




unsigned long Rol(unsigned long x, int y)  
{  
	if (y % 32 == 0) {return x;}   
	else {return ((x << y)^(x >> -y));}  
}  


unsigned long Ror(unsigned long x, int y)  
{  
	if (y % 32 == 0) {return x;}  
	else {return ((x >> y)^(x << -y));}  
}  


void append(char* s, int slen, char c)  
{  
	//int len = strlen(s);  
	s[slen] = c;  
	s[slen+1] = '\0';  
}  


void SHA1(char s[],int slen)  
{  
	unsigned long K[80];  
	unsigned long A,B,C,D,E,TEMP;  
	int r,k,ln,t,l,i,j;  
	H[0]=0x67452301;  
	H[1]=0xefcdab89;  
	H[2]=0x98badcfe;  
	H[3]=0x10325476;  
	H[4]=0xc3d2e1f0;  


	ln=slen;  
	r = (int)((ln+1)/64);  


	if (((ln+1) % 64) > 56)  
	{  
		r=r+1;  
	}  


	// initialize Constants  


	for(t=0; t<80; t++)  
	{  
		if (t<20)  
		{  
			K[t] = 0x5a827999;  
		}  


		if ((t>19)&(t<40))  
		{  
			K[t] = 0x6ED9EBA1;  
		}  
		if ((t>39)&(t<60))  
		{  
			K[t] = 0x8F1BBCDC;  
		}  
		if (t>59)  
		{  
			K[t] = 0xca62c1d6;  
		}  
	}  


	for(l=0; l <= r; l++)  
	{  
		unsigned long W[80]={0};  
		//Initialize Text  
		for (i=0; i<16; i++)  
		{  
			for(j=0; j<4; j++)  
			{  
				if (4*i+j <= ln)  
				{  
					k = s[64*l+4*i+j];  
				}  
				else  
				{  
					k =0;  
				}  

				if (k<0)  
				{  
					k = k +256;  
				}  


				if (4*i+j == ln)  
				{  
					k = 0x80;  
				}  


				W[i]= W[i] + k*(unsigned long)pow(256,(double)3-j);  
			}  
		}  
		if ((W[14]==0)&(W[15]==0))  
		{  
			W[15]=8*slen;  
		}  


		// Hash Cycle  


		for (t = 16; t <80; t++)  
		{  
			W[t] = Rol(W[t-3]^W[t-8]^W[t-14]^W[t-16],1);  
		}  


		A = H[0];  
		B = H[1];  
		C = H[2];  
		D = H[3];  
		E = H[4];  


		for(t = 0; t < 80; t++)  
		{  
			TEMP = Rol(A,5) + f(B,C,D,t) + E + W[t] + K[t];  
			E = D;  
			D = C;  
			C = Rol(B,30);  
			B = A;    
			A = TEMP;  
		}  


		H[0] = H[0] + A;  
		H[1] = H[1] + B;  
		H[2] = H[2] + C;  
		H[3] = H[3] + D;  
		H[4] = H[4] + E;  


		ln = ln - 64;  
	}  


}   


void HMAC(char text[], int tlen, char key[], int klen)  
{  
	char c;  
	char s[256] = "";  
	int slen = 0;  
	unsigned long Key[16] = {0};  
	unsigned long X[16] = {0};  
	unsigned long Y[16] = {0};  
	unsigned long ipad = 0x36363636;  
	unsigned long opad = 0x5c5c5c5c;  
	int k,i,j;  




	//Process string key into sub-key  
	//Hash key in case it is less than 64 bytes  
	if (klen > 64)  
	{  
		SHA1(key, klen);  
		Key[0] = H[0];  
		Key[1] = H[1];  
		Key[2] = H[2];  
		Key[3] = H[3];  
		Key[4] = H[4];  
	}  
	else  
	{  
		for(i=0; i<16; i++)  
		{  
			for(j=0; j<4; j++)  
			{  
				if (4*i+j <= klen)  
				{  
					k = key[4*i+j];  
				}  
				else  
				{  
					k = 0;  
				}  
				if (k<0)  
				{  
					k = k + 256;  
				}  
				Key[i]= Key[i] + k*(unsigned long)pow(256,(double)3-j);  
			}  
		}  
	}  

	for(i=0; i<16; i++)  
	{  
		X[i] = Key[i]^ipad;  
		Y[i] = Key[i]^opad;  
	}  


	//Turn X-Array into a String  
	for(i=0; i<16; i++)  
	{  
		for(j=0; j<4; j++)  
		{  
			c = ((X[i] >> 8*(3-j)) % 256);  
			//s = s + c;  
			append(s,slen,c);  
			slen++;  
		}  
	}  


	//Append text to string  
	//s = s + text  
	for (i=0;i<tlen;++i)  
	{  
		s[slen+i] = text[i];  
	}  
	slen = slen + tlen;  


	//Hash X-Array  
	SHA1(s,slen);  


	for (i=0;i<slen;++i)  
	{  
		s[i] = 0;  
	}  
	slen = 0;  


	//Turn Y-Array into a String  
	for(i=0; i<16; i++)  
	{  
		for(j=0; j<4; j++)  
		{  
			c = ((Y[i] >> 8*(3-j)) % 256);  
			//s = s + c;  
			append(s,slen,c);  
			slen++;  
		}  
	}  


	//Append Hashed X-Array to Y-Array in string  
	for(i=0; i<5; i++)  
	{  
		for(j=0; j<4; j++)  
		{  
			c = ((H[i] >> 8*(3-j)) % 256);  
			//s = s + c;  
			append(s,slen,c);  
			slen++;  
		}  
	}  


	//Hash final concatenated string  
	SHA1(s,slen);  
}
// c=10000
// dkLen=64(128B)/128(256B)/256(512B)
void PBKDF2(char P[],int Plen, char S[], int Slen, int c,int dkLen)  
{  
	unsigned long L[5]={0,0,0,0,0};  
	int hlen,l,r,i,j,x,y;  
	char t;  

	hlen = 20;                          // from sha1  
	//requires new version of Ceiling Function: l = ceil(dkLen/hlen)  
	l = (dkLen/hlen)+1;                 // hLen-octets 32/20 = 2 always  
	r = dkLen -(l-1)*hlen;              // number of octets in the last block (32-20) = 12  

	for(i=1; i<=l; i++)                  // passes  
	{  

		// t = 0;  
		// U = S + t + t + t ;              // append characters  
		// t = i;  
		// U = U + t;  
		/////////  
		char U[50];  // larger than it needs to be (SSID<32 + 4)  
		int Ulen = 0;  
		char appended[] = {(char)0, (char)0, (char)0, (char)i};  

		for (j=0;j<Slen;++j)  
		{  
			U[j]=S[j];  
		}  
		for (j=0;j<5;++j)  
		{  
			U[14+j] = appended[j];  
		}  
		Ulen = Slen + 4;  
		/////////  
		//  
		//  
		//  
		//  


		L[0] = L[1] = L[2] = L[3] = L[4] = 0;  
		for(j=1; j<=c; j++)          // number of iterations (4096)  
		{  


			HMAC(U,Ulen,P,Plen);  
			L[0] = L[0]^H[0];       // ^ is the XOR function  
			L[1] = L[1]^H[1];  
			L[2] = L[2]^H[2];  
			L[3] = L[3]^H[3];  
			L[4] = L[4]^H[4];  

			//  
			// U = "";  
			/////////  
			for (x=0;x<Ulen;++x)  
			{  
				U[x] = 0;  
			}  
			Ulen=0;  
			/////////  
			//  
			//  


			for(x=0; x<5; x++)  
			{  
				for(y=0; y<4; y++)  
				{  
					t = ((H[x] >> 8*(3-y)) % 256);  

					//  
					// U = U + t;  
					/////////  
					append(U,Ulen,t);  
					Ulen++;  
					/////////  
					//  
					//  
				}  
			}  
		}  

		T[5*(i-1)] = L[0];  // moves the results down 5 each iteration  
		T[5*(i-1)+1] = L[1];  
		T[5*(i-1)+2] = L[2];  
		T[5*(i-1)+3] = L[3];  
		T[5*(i-1)+4] = L[4];  
	}  
}
*/
/*
int main()  
{   
	int i,j;  
	printf("\n\n****---***---***---***---***---***---****\n\n");  
	SHA1("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",56);  
	printf("SHA1 tests (Test Cases from FIPS PUBS 180-2)\n");  
	printf("\nText     : abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\n");  
	printf("Expected : 84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1\n");  
	printf("Returned :");  
	for(i=0;i<=4;++i)  
	{  
		printf(" %x",H[i]);  
	}  
	SHA1("abc",3);  
	printf("\n\nText     : abc\n");  
	printf("Expected : a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d\n");  
	printf("Returned :");  
	for(i=0;i<=4;++i)  
	{  
		printf(" %x",H[i]);  
	}  
	printf("\n\n****---***---***---***---***---***---****\n\n");  
	HMAC("what do ya want for nothing?",28,"Jefe",4);  
	printf("HMAC test (Test Case from RFC2202)\n\n");  
	printf("Key      : Jefe\n");  
	printf("Text     : what do ya want for nothing?\n");  
	printf("Expected : effcdf6a e5eb2fa2 d27416d5 f184df9c 259a7c79\n");  
	printf("Returned :");  
	for(i=0;i<=4;++i)  
	{  
		printf(" %x",H[i]);  
	}  
	printf("\n\n****---***---***---***---***---***---****\n\n");  
	// use http://www.xs4all.nl/~rjoris/wpapsk.html  
	// and http://anandam.name/pbkdf2/  
	// for PBKDF2 Verification  
	PBKDF2("Password22",10,"itsMEjessica22",14,4096,32);  
	printf("PBKDF2 test (Test Cases from internet, see comments)\n");  
	printf("SSID     : itsMEjessica22\n");  
	printf("Password : Password22\n");  
	printf("Expected : 2944bf07 17219278 f0e990ac 34e4e151 1c4233ae b38b63cc f4d39035 3426233e\n");  
	printf("Returned :");  
	for(j=0; j<(32/4); j++)  
	{  
		printf(" %x",T[j]);  
	}  
}
*/
#endif // __PBKDF2_H__
