// PBKDF2 Beta 1.1.cpp : Defines the entry point for the console application.
//PBKDF2 Beta 1.1 using the HMAC-SHA-1 as the Pseduo-Random Function

#include <iostream>
#include <string>
#include <cmath>
using namespace std;
unsigned long Rol(unsigned long x, int y);
unsigned long Ror(unsigned long x, int y);
unsigned long f(unsigned long B,unsigned long C,unsigned long D, int t);

unsigned long H[5];
unsigned long T[512]={0};
void HMAC(string text, string key);
void SHA1(string s);
std::string PBKDF2(string P,string S, int c, int dkLen);

//int main()
//
//{
//	char P[8192];
//	char S[8192];
//	int dkLen,c;
//	c = 4096;
//	dkLen = 32;
//
//	cout << "Password Based Key-Derivation function 2 (PBKDF2) Beta 1.1 using the" << endl << "SHA-1 Hashing Algorithm. \n \n";
//
//	cout << "Enter Password: ";
//	cin.getline(P, 8192, '\n');
//	cout << "Enter Salt: ";
//	cin.getline(S, 8192, '\n');;
//	cout << "Enter Number of Iterations c: ";
//	cin >> c;
//	cout << "Enter intended length in bytes of the derived key: ";
//	cin >> dkLen;
//
//	cout << "Derived Key: " << endl;
//	PBKDF2(P,S,c,dkLen);
//
//	for(int i=0; i<(dkLen/4); i++)
//	{
//		cout << hex << T[i];
//	}
//	cout << endl;
//
//	return 0;
//}

std::string PBKDF2(string P, string S,int c,int dkLen)
{
	string U;
	unsigned long L[5]={0,0,0,0,0};
	int hlen,l,r;
	char t;
	
	hlen = 20;
	//Needs a replacement for ceiling function: l = ceil(dkLen/hlen)+1;
	l = (dkLen/hlen)+1;
	r = dkLen -(l-1)*hlen;
		
	for(int i=1; i<=l; i++)
	{
		t = 0;
		U = S + t + t + t;
		t = i;
		U = U + t;
		L[0] = L[1] = L[2] = L[3] = L[4] = 0;
		for(int j=1; j<=c; j++)
		{
			HMAC(U,P);
			L[0] = L[0]^H[0];
			L[1] = L[1]^H[1];
			L[2] = L[2]^H[2];
			L[3] = L[3]^H[3];
			L[4] = L[4]^H[4];
			U = "";
			for(int x=0; x<5; x++)
			{
				for(int y=0; y<4; y++)
				{
					t = ((H[x] >> 8*(3-y)) % 256);
					U = U + t;
				}
			}
		}
		
		T[5*(i-1)] = L[0];
		T[5*(i-1)+1] = L[1];
		T[5*(i-1)+2] = L[2];
		T[5*(i-1)+3] = L[3];
		T[5*(i-1)+4] = L[4];
	}

	std::string sPBKDF2;
	char lpszBuffer[18];
	for(int j=0; j<(dkLen/4); j++)
	{
		sprintf(lpszBuffer,"%x",(unsigned int)T[j]);
		sPBKDF2.append(lpszBuffer);
	}
	return sPBKDF2;

}

// HMAC function
void HMAC(string text, string key)
{
	int i=0;
	char c;
	string s;
	unsigned long Key[16] = {0};
	unsigned long X[16] = {0};
	unsigned long Y[16] = {0};
	unsigned long ipad = 0x36363636;
	unsigned long opad = 0x5c5c5c5c;
	int k;
	s = "";

	//Process string key into sub-key
	//Hash key in case it is less than 64 bytes
	if (key.length() > 64)
	{
		SHA1(key);
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
			for(int j=0; j<4; j++)
			{
				if (4*i+j <= key.length())
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
				Key[i]= Key[i] + k*pow(256,(double)3-j);
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
		for(int j=0; j<4; j++)
		{
			c = ((X[i] >> 8*(3-j)) % 256);
			s = s + c;
		}
	}

	//Append text to string
	s = s + text;

	//Hash X-Array
	SHA1(s);

	s = "";

	//Turn Y-Array into a String
	for(i=0; i<16; i++)
	{
		for(int j=0; j<4; j++)
		{
			c = ((Y[i] >> 8*(3-j)) % 256);
			s = s + c;
		}
	}

	//Append Hashed X-Array to Y-Array in string
	for(i=0; i<5; i++)
	{
		for(int j=0; j<4; j++)
		{
			c = ((H[i] >> 8*(3-j)) % 256);
			s = s + c;
		}
	}

	//Hash final concatenated string
	SHA1(s);

}

// SHA-1 Algorithm
void SHA1(string s)
{
	int t=0;
	unsigned long K[80];
	unsigned long A,B,C,D,E,TEMP;
	int r,k,ln;
	H[0]=0x67452301;
	H[1]=0xefcdab89;
	H[2]=0x98badcfe;
	H[3]=0x10325476;
	H[4]=0xc3d2e1f0;

	ln=s.length();
	r = int((ln+1)/64);

	if (((ln+1) % 64) > 56)
		{
		r=r+1;
		}

	// initialize Constants
	for( t=0; t<80; t++)
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

	for(int l=0; l <= r; l++)
	{
		unsigned long W[80]={0};
		//Initialize Text
		for (int i=0; i<16; i++)
			{
			for(int j=0; j<4; j++)
				{
					if (4*i+j <= ln)
					{
						k = s[64*l+4*i+j];
					}
					else
					{
						k = 0;
					}
				
					if (k<0)
					{
						k = k +256;
					}

					if (4*i+j == ln)
					{
						k = 0x80;
					}

					W[i]= W[i] + k*pow(256,(double)3-j);
				}
			}
		if ((W[14]==0)&(W[15]==0))
		{
			W[15]=8*s.length();
		}

	// Hash Cycle

		for (int t = 16; t <80; t++)
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