// colorconver.h file here
#ifndef __colorconfer_h__
#define __colorconfer_h__

#ifndef RGB
#define RGB(r,g,b)          ((unsigned int)(((unsigned char)(r)|((unsigned short)((unsigned char)(g))<<8))|(((unsigned int)(unsigned char)(b))<<16)))
#endif
#ifndef LOBYTE
#define LOBYTE(w)           ((unsigned char)(((unsigned int)(w)) & 0xff))
#endif
#ifndef GetRValue
#define GetRValue(rgb)      (LOBYTE(rgb))
#endif
#ifndef GetGValue
#define GetGValue(rgb)      (LOBYTE(((unsigned short)(rgb)) >> 8))
#endif
#ifndef GetBValue
#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

inline unsigned char HueToRGB(float rm1,float rm2,float rh)
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
    int m = MIN((int)n,255);
    m = MAX(0,m);
    return (unsigned char)m;//(rm1 * 255);
}

inline void RGBtoHSL(unsigned char R,unsigned char G,unsigned char B,float* H,float* S,float* L)
{
    unsigned char minval = MIN(R,MIN(G,B));
    unsigned char maxval = MAX(R,MAX(G,B));
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

inline unsigned int HSLtoRGB(float H,float S,float L)
{
    unsigned char r,g,b;

    L = MIN((float)1,L);
    S = MIN((float)1,S);

    if(S == 0.0)
    {
        r = g = b = (unsigned char)(255 * L);
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
