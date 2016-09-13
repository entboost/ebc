// colorconver.h file here
#ifndef __colorconfer_h__
#define __colorconfer_h__

inline BYTE HueToRGB(float rm1,float rm2,float rh)
{
	while(rh > 360.0f)
		rh -= 360.0f;
	while(rh < 0.0f)
		rh += 360.f;

	if(rh < 60.0f)
		rm1 = rm1 + (rm2 - rm1) * rh / 60.0f;
	else if(rh < 180.0f)
		rm1 = rm2;
	else if(rh < 240.0f)
		rm1 = rm1 + (rm2 - rm1) * (240.0f - rh) / 60.0f;

	float n = rm1*255;
	int m = min((int)n,255);
	m = max(0,m);
	return (BYTE)m;//(rm1 * 255);
}

inline void RGBtoHSL(BYTE R,BYTE G,BYTE B,float* H,float* S,float* L)
{
	BYTE minval = min(R,min(G,B));
	BYTE maxval = max(R,max(G,B));
	float mdiff = float(maxval) - float(minval);
	float msum  = float(maxval) + float(minval);
    
	*L = msum / 510.0f;

	if (maxval == minval) 
	{
		*S = 0.0f;
		*H = 0.0f; 
	}   
	else 
	{ 
		float rnorm = (maxval - R) / mdiff;      
		float gnorm = (maxval - G) / mdiff;
		float bnorm = (maxval - B) / mdiff;   
 
		*S = (*L <= 0.5f) ? (mdiff / msum) : (mdiff / (510.0f - msum));

		if(R == maxval) 
			*H = 60.0f * (6.0f + bnorm - gnorm);
		if(G == maxval) 
			*H = 60.0f * (2.0f + rnorm - bnorm);
		if(B == maxval) 
			*H = 60.0f * (4.0f + gnorm - rnorm);
		if (*H > 360.0f) 
			*H -= 360.0f;
	}
}

inline COLORREF HSLtoRGB(float H,float S,float L)
{
	BYTE r,g,b;

	L = min(1,L);
	S = min(1,S);

	if(S == 0.0)
	{
		r = g = b = (BYTE)(255 * L);
	} 
	else 
	{
		float rm1, rm2;

		if (L <= 0.5f) 
			rm2 = L + L * S;
		else
			rm2 = L + S - L * S;
		rm1 = 2.0f * L - rm2;   

		r = HueToRGB(rm1, rm2, H + 120.0f);
		g = HueToRGB(rm1, rm2, H);
		b = HueToRGB(rm1, rm2, H - 120.0f);
	}
	return RGB(r,g,b);
}

#endif // __colorconfer_h__
