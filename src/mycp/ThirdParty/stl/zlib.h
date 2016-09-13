// zlib.h file here
#ifndef __zlib_H__
#define __zlib_H__

#define ZLIB_WINAPI
#ifndef useconds_t
#define useconds_t unsigned int
#endif // useconds_t

#include <zlib.h>
#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib,"zlibd.lib")
#else
#pragma comment(lib,"zlib.lib")
#endif
#endif // WIN32


#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// return false to cancel ZipApi
#define ZIP_FLAG_LAST_SOURCE	1
#define ZIP_FLAG_LAST_DATA		2
typedef bool (*ZipDataCallBack) (uLong nSourceIndex, const unsigned char* pData, uLong nSize, unsigned int nUserData);
//const uLong ZIP_DEFAULT_PACK_SIZE1	= 16*1024;
const uLong ZIP_DEFAULT_PACK_SIZE2	= 64*1024;
const uLong ZIP_DEFAULT_PACK_SIZE3	= 640*1024;
#define CONST_ZIP_PACK_SIZE(s) ((s>2*ZIP_DEFAULT_PACK_SIZE3)?(ZIP_DEFAULT_PACK_SIZE3):(s>2*ZIP_DEFAULT_PACK_SIZE2?ZIP_DEFAULT_PACK_SIZE2:(s+20)))
#define CONST_UZIP_PACK_SIZE(s) ((s>=ZIP_DEFAULT_PACK_SIZE3)?(ZIP_DEFAULT_PACK_SIZE3):(s>=ZIP_DEFAULT_PACK_SIZE2?ZIP_DEFAULT_PACK_SIZE2:(s*2)))
#define DEFAULT_WINDOWBITS (MAX_WBITS + 16)

//inline int ZipFile(FILE *source, size_t nFileOffset, FILE *dest, int level=Z_DEFAULT_COMPRESSION)
//{
//	int ret, flush;
//	unsigned have;
//	z_stream strm;
//	unsigned char in[const_pack_size];
//	unsigned char out[const_pack_size];
//
//	/* allocate deflate state */
//	strm.zalloc = Z_NULL;
//	strm.zfree = Z_NULL;
//	strm.opaque = Z_NULL;
//	ret = deflateInit(&strm, level);
//	if (ret != Z_OK)
//		return ret;
//
//	/* compress until end of file */
//	do {
//		strm.avail_in = fread(in, 1, const_pack_size, source);
//		if (ferror(source)) {
//			(void)deflateEnd(&strm);
//			return Z_ERRNO;
//		}
//		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//		strm.next_in = in;
//
//		/* run deflate() on input until output buffer not full, finish
//		compression if all of source has been read in */
//		do {
//			strm.avail_out = const_pack_size;
//			strm.next_out = out;
//			ret = deflate(&strm, flush);    /* no bad return value */
//			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//			have = const_pack_size - strm.avail_out;
//			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//				(void)deflateEnd(&strm);
//				return Z_ERRNO;
//			}
//		} while (strm.avail_out == 0);
//		assert(strm.avail_in == 0);     /* all input will be used */
//
//		/* done when last data in file processed */
//	} while (flush != Z_FINISH);
//	assert(ret == Z_STREAM_END);        /* stream will be complete */
//
//	/* clean up and return */
//	(void)deflateEnd(&strm);
//	return Z_OK;
//}

//int def(FILE *source, FILE *dest, int level)
//{
//    int ret, flush;
//    unsigned have;
//    z_stream strm;
//    unsigned char in[CHUNK];
//    unsigned char out[CHUNK];
// 
//    /* allocate deflate state */
//    strm.zalloc = Z_NULL;
//    strm.zfree = Z_NULL;
//    strm.opaque = Z_NULL;
//    ret = deflateInit(&strm, level);
//    if (ret != Z_OK)
//        return ret;
// 
//    /* compress until end of file */
//    do {
//        strm.avail_in = fread(in, 1, CHUNK, source);
//        if (ferror(source)) {
//            (void)deflateEnd(&strm);
//            return Z_ERRNO;
//        }
//        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//        strm.next_in = in;
// 
//        /* run deflate() on input until output buffer not full, finish
//           compression if all of source has been read in */
//        do {
//            strm.avail_out = CHUNK;
//            strm.next_out = out;
//            ret = deflate(&strm, flush);    /* no bad return value */
//            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//            have = CHUNK - strm.avail_out;
//            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//                (void)deflateEnd(&strm);
//                return Z_ERRNO;
//            }
//        } while (strm.avail_out == 0);
//        assert(strm.avail_in == 0);     /* all input will be used */
// 
//        /* done when last data in file processed */
//    } while (flush != Z_FINISH);
//    assert(ret == Z_STREAM_END);        /* stream will be complete */
// 
//    /* clean up and return */
//    (void)deflateEnd(&strm);
//    return Z_OK;
//}
// 

//inline int ZipData(const unsigned char *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
inline int ZipFile2Data(FILE *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
{
	int ret, flush;
	unsigned have;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (gzip)
		ret = deflateInit2(&strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	else
		ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_ZIP_PACK_SIZE(nSourceSize);
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)deflateEnd(&strm);
		return Z_MEM_ERROR;
	}

	const uLong nDestSize = *pDestSize;
	*pDestSize = 0;
	uLong nSourceIndex = 0;
	/* compress until end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		nSourceIndex += nIn;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			delete[] in;
			return Z_ERRNO;
		}
		flush = (nSourceIndex>=nSourceSize||feof(source)) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			const unsigned int nOut = min((nDestSize-(*pDestSize)),const_pack_size);
			if (nOut==0)
			{
				(void)deflateEnd(&strm);
				delete[] in;
				return Z_MEM_ERROR;
			}
			strm.avail_out = nOut;
			strm.next_out = dest+(*pDestSize);
			ret = deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = nOut - strm.avail_out;
			*pDestSize += have;
		} while (strm.avail_out == 0);
		//assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);
	delete[] in;
	return Z_OK;
}
inline int GZipFile2Data(FILE *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION)
{
	return ZipFile2Data(source, nSourceSize, dest, pDestSize, level, 1);
}
inline int ZipFile2Cb(FILE *source, uLong nSourceSize, ZipDataCallBack pCb, unsigned int nUserData, uLong* pOutSize=NULL, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	if (nSourceSize==0)
	{
#ifdef WIN32
			_fseeki64(source, 0, SEEK_END);
			nSourceSize = (uLong)_ftelli64(source);
			_fseeki64(source, 0, SEEK_SET);
#else
			fseeko(source, 0, SEEK_END);
			nSourceSize = (uLong)ftello(source);
			fseeko(source, 0, SEEK_SET);
#endif
	}

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (gzip)
		ret = deflateInit2(&strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	else
		ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_ZIP_PACK_SIZE(nSourceSize);	
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)deflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)deflateEnd(&strm);
		delete[] in;
		return Z_MEM_ERROR;
	}

	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	/* compress until end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		nSourceIndex += nIn;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			delete[] in;
			delete[] out;
			return Z_ERRNO;
		}
		flush = (nSourceIndex>=nSourceSize||feof(source)) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (!pCb((nSourceIndex-nIn),out,have,nUserData))
			{
				(void)deflateEnd(&strm);
				delete[] in;
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		//assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);
	delete[] in;
	delete[] out;
	return Z_OK;
}
inline int GZipFile2Cb(FILE *source, uLong nSourceSize, ZipDataCallBack pCb, unsigned int nUserData, uLong* pOutSize, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
{
	return ZipFile2Cb(source, nSourceSize, pCb, nUserData, pOutSize, level, 1);
}
inline int ZipFile2File(FILE *source, uLong nSourceSize, FILE *dest, uLong* pOutSize, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
{
	int ret, flush;
	unsigned have;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (gzip)
		ret = deflateInit2(&strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	else
		ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_ZIP_PACK_SIZE(nSourceSize);	
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)deflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)deflateEnd(&strm);
		delete[] in;
		return Z_MEM_ERROR;
	}

	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	/* compress until end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		nSourceIndex += nIn;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			delete[] in;
			delete[] out;
			return Z_ERRNO;
		}
		flush = (nSourceIndex>=nSourceSize||feof(source)) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;

		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)deflateEnd(&strm);
				delete[] in;
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		//assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);
	delete[] in;
	delete[] out;
	fflush(dest);
	return Z_OK;
}
inline int GZipFile2File(FILE *source, uLong nSourceSize, FILE *dest, uLong* pOutSize, int level=Z_DEFAULT_COMPRESSION)
{
	return ZipFile2File(source, nSourceSize, dest, pOutSize, level, 1);
}

inline int UnZipData2File(const unsigned char *source, uLong nSourceSize, FILE *dest, int gzip, unsigned int * pOutSize=NULL)
{
	int ret;
	unsigned int have;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)inflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	/* decompress until deflate stream ends or end of file */
	do {
		strm.avail_in = min(const_pack_size,(nSourceSize-nSourceIndex));
		if (strm.avail_in == 0)
			break;
		strm.next_in = (unsigned char*)source+nSourceIndex;
		nSourceIndex += strm.avail_in;

		//strm.avail_in = fread(in, 1, CHUNK, source);
		//if (ferror(source)) {
		//	(void)inflateEnd(&strm);
		//	return Z_ERRNO;
		//}
		//if (strm.avail_in == 0)
		//	break;
		//strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				delete[] out;
				return ret;
			}
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)inflateEnd(&strm);
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	delete[] out;
	if (ret == Z_STREAM_END)
		fflush(dest);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipData2File(const unsigned char *source, uLong nSourceSize, FILE *dest, unsigned int * pOutSize=NULL)
{
	return UnZipData2File(source, nSourceSize, dest, 1, pOutSize);
}
inline int UnZipFile2File(FILE *source, uLong nSourceSize, FILE *dest, int gzip, unsigned int * pOutSize=NULL)
{
	int ret;
	unsigned have;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	if (nSourceSize==0)
	{
#ifdef WIN32
		_fseeki64(source, 0, SEEK_END);
		nSourceSize = (uLong)_ftelli64(source);
		_fseeki64(source, 0, SEEK_SET);
#else
		fseeko(source, 0, SEEK_END);
		nSourceSize = (uLong)ftello(source);
		fseeko(source, 0, SEEK_SET);
#endif
	}

	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);	
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)inflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)inflateEnd(&strm);
		delete[] in;
		return Z_MEM_ERROR;
	}

	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	/* decompress until deflate stream ends or end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		if (nIn==0)
			break;
		nSourceIndex += nIn;
		//nSourceIndex += strm.avail_in;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			delete[] in;
			delete[] out;
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				delete[] in;
				delete[] out;
				return ret;
			}
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
				(void)inflateEnd(&strm);
				delete[] in;
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	delete[] in;
	delete[] out;
	if (ret == Z_STREAM_END)
		fflush(dest);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipFile2File(FILE *source, uLong nSourceSize, FILE *dest, unsigned int * pOutSize=NULL)
{
	return UnZipFile2File(source, nSourceSize, dest, 1, pOutSize);
}
inline int UnZipFile2Data(FILE *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int gzip)
{
	int ret;
	unsigned have;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);	
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)inflateEnd(&strm);
		return Z_MEM_ERROR;
	}

	const uLong nDestSize = *pDestSize;
	*pDestSize = 0;
	uLong nSourceIndex = 0;
	/* decompress until deflate stream ends or end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		if (nIn==0)
			break;
		nSourceIndex += nIn;
		//nSourceIndex += strm.avail_in;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			delete[] in;
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			const unsigned int nOut = min((nDestSize-(*pDestSize)),const_pack_size);
			if (nOut==0)
			{
				(void)inflateEnd(&strm);
				delete[] in;
				return Z_MEM_ERROR;
			}
			strm.avail_out = nOut;
			strm.next_out = dest+(*pDestSize);
			//strm.avail_out = const_pack_size;
			//strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				delete[] in;
				return ret;
			}
			have = nOut - strm.avail_out;
			*pDestSize += have;
			//have = const_pack_size - strm.avail_out;
			//if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
			//	(void)inflateEnd(&strm);
			//	delete[] in;
			//	delete[] out;
			//	return Z_ERRNO;
			//}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	delete[] in;
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipFile2Data(FILE *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize)
{
	return UnZipFile2Data(source, nSourceSize, dest, pDestSize, 1);
}
inline int UnZipFile2Cb(FILE *source, uLong nSourceSize, int gzip, unsigned int * pOutSize, ZipDataCallBack pCb, unsigned int nUserData)
{
	assert(pCb != NULL);
	int ret;
	unsigned have;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	if (nSourceSize==0)
	{
#ifdef WIN32
			_fseeki64(source, 0, SEEK_END);
			nSourceSize = (uLong)_ftelli64(source);
			_fseeki64(source, 0, SEEK_SET);
#else
			fseeko(source, 0, SEEK_END);
			nSourceSize = (uLong)ftello(source);
			fseeko(source, 0, SEEK_SET);
#endif
	}
	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);	
	unsigned char * in = new unsigned char[const_pack_size];
	if (in==NULL)
	{
		(void)inflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)inflateEnd(&strm);
		delete[] in;
		return Z_MEM_ERROR;
	}

	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	/* decompress until deflate stream ends or end of file */
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		if (nIn==0)
			break;
		nSourceIndex += nIn;
		//nSourceIndex += strm.avail_in;
		strm.avail_in = (unsigned int)fread(in, 1, nIn, source);
		if (ferror(source)) {
			(void)inflateEnd(&strm);
			delete[] in;
			delete[] out;
			return Z_ERRNO;
		}
		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				delete[] in;
				delete[] out;
				return ret;
			}
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (!pCb((nSourceIndex-nIn),out,have,nUserData))
			{
				(void)inflateEnd(&strm);
				delete[] in;
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	delete[] in;
	delete[] out;
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipFile2Cb(FILE *source, uLong nSourceSize, unsigned int * pOutSize, ZipDataCallBack pCb, unsigned int nUserData)
{
	return UnZipFile2Cb(source, nSourceSize, 1, pOutSize, pCb, nUserData);
}
inline int ZipData(const unsigned char *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION, int gzip=0)
{
	int ret, flush;
	unsigned int have=0;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	// deflateInit2(&strm, DEFAULT_COMPRESSION,Z_DEFLATED, DEFAULT_WINDOWSIZE,DEFAULT_MEMLEVEL, Z_DEFAULT_STRATEGY)
	if (gzip)
		ret = deflateInit2(&strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	else
		ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_ZIP_PACK_SIZE(nSourceSize);
	const uLong nDestSize = *pDestSize;
	*pDestSize = 0;
	uLong nSourceIndex = 0;
	do {
		strm.avail_in = min(const_pack_size,(nSourceSize-nSourceIndex));
		strm.next_in = (unsigned char*)source+nSourceIndex;
		nSourceIndex += strm.avail_in;
		flush = (nSourceIndex>=nSourceSize) ? Z_FINISH : Z_NO_FLUSH;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			const unsigned int nOut = min((nDestSize-(*pDestSize)),const_pack_size);
			if (nOut==0)
			{
				(void)deflateEnd(&strm);
				return Z_MEM_ERROR;
			}
			strm.avail_out = nOut;
			strm.next_out = dest+(*pDestSize);
			ret = deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = nOut - strm.avail_out;
			*pDestSize += have;
		} while (strm.avail_out == 0);
		//assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);
	return Z_OK;
}
inline int GZipData(const unsigned char *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION)
{
	return ZipData(source, nSourceSize, dest, pDestSize, level,1);
}

// * return false cancel
inline int ZipDataCb(const unsigned char *source, uLong nSourceSize, int gzip, int level, ZipDataCallBack pCb, unsigned int nUserData)
{
	assert(pCb != NULL);
	int ret, flush;
	unsigned int have=0;
	z_stream strm;

	/* allocate deflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	if (gzip)
		ret = deflateInit2(&strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	else
		ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_ZIP_PACK_SIZE(nSourceSize);
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)deflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	uLong nSourceIndex = 0;
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		strm.avail_in = nIn;
		strm.next_in = (unsigned char*)source+nSourceIndex;
		nSourceIndex += strm.avail_in;
		flush = (nSourceIndex>=nSourceSize) ? Z_FINISH : Z_NO_FLUSH;

		/* run deflate() on input until output buffer not full, finish
		compression if all of source has been read in */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = deflate(&strm, flush);    /* no bad return value */
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			have = const_pack_size - strm.avail_out;
			if (!pCb((nSourceIndex-nIn),out,have,nUserData))
			{
				(void)deflateEnd(&strm);
				delete[] out;
				return Z_ERRNO;
			}
		} while (strm.avail_out == 0);
		//assert(strm.avail_in == 0);     /* all input will be used */

		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	//assert(ret == Z_STREAM_END);        /* stream will be complete */

	/* clean up and return */
	(void)deflateEnd(&strm);
	delete[] out;
	return Z_OK;
}
inline int GZipDataCb(const unsigned char *source, uLong nSourceSize, int level, ZipDataCallBack pCb, unsigned int nUserData)
{
	return ZipDataCb(source, nSourceSize, 1, level, pCb, nUserData);
}
inline int UnZipData(const unsigned char *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize, int gzip=0)
{
	int ret;
	unsigned int have = 0;
	z_stream strm;
	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	//inflateInit2(&d_stream, MAX_WBITS + 16)
	//inflateInit2(&strm,  47);	// for gzip
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);
	const uLong nDestSize = *pDestSize;
	*pDestSize = 0;
	uLong nSourceIndex = 0;
	do {
		strm.avail_in = min(const_pack_size,(nSourceSize-nSourceIndex));
		if (strm.avail_in == 0)
			break;
		strm.next_in = (unsigned char*)source+nSourceIndex;
		nSourceIndex += strm.avail_in;

		/* run inflate() on input until output buffer not full */
		do {
			const unsigned int nOut = min((nDestSize-(*pDestSize)),const_pack_size);
			if (nOut==0)
			{
				(void)inflateEnd(&strm);
				return Z_MEM_ERROR;
			}
			strm.avail_out = nOut;
			strm.next_out = dest+(*pDestSize);
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
			have = nOut - strm.avail_out;
			*pDestSize += have;
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipData(const unsigned char *source, uLong nSourceSize, unsigned char *dest, uLong* pDestSize)
{
	return UnZipData(source, nSourceSize, dest, pDestSize, 1);
}

// * return false cancel
inline int UnZipDataCb(const unsigned char *source, uLong nSourceSize, int gzip, unsigned int* pOutSize, ZipDataCallBack pCb, unsigned int nUserData)
{
	assert(pCb != NULL);
	int ret;
	unsigned int have = 0;
	z_stream strm;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	if (gzip)
		ret = inflateInit2(&strm, DEFAULT_WINDOWBITS);
	else
		ret = inflateInit(&strm);
	if (ret != Z_OK)
		return ret;

	const uLong const_pack_size = CONST_UZIP_PACK_SIZE(nSourceSize);
	unsigned char * out = new unsigned char[const_pack_size];
	if (out==NULL)
	{
		(void)inflateEnd(&strm);
		return Z_MEM_ERROR;
	}
	if (pOutSize!=NULL)
		*pOutSize = 0;
	uLong nSourceIndex = 0;
	do {
		const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
		strm.avail_in = nIn;
		if (strm.avail_in == 0)
			break;
		strm.next_in = (unsigned char*)source+nSourceIndex;
		nSourceIndex += strm.avail_in;

		/* run inflate() on input until output buffer not full */
		do {
			strm.avail_out = const_pack_size;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				delete[] out;
				return ret;
			}
			have = const_pack_size - strm.avail_out;
			if (pOutSize!=NULL)
				*pOutSize += have;
			if (!pCb((nSourceIndex-nIn),out,have,nUserData))
			{
				(void)inflateEnd(&strm);
				delete[] out;
				return Z_ERRNO;
			}
			//*pDestSize += have;
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm);
	delete[] out;
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}
inline int UnGZipDataCb(const unsigned char *source, uLong nSourceSize, unsigned int* pOutSize, ZipDataCallBack pCb, unsigned int nUserData)
{
	return UnZipDataCb(source, nSourceSize, 1, pOutSize, pCb, nUserData);
}
//int Zip(const unsigned char* pSrc, uLong nSrcSize, unsigned char* pDest, uLong* pDestSize, int level=Z_DEFAULT_COMPRESSION)
//{
//	//const uLong nCompressSizeTemp = compressBound((uLong)min(const_pack_size,nSrcSize));
//	//unsigned char* lpszZipBuffer = new unsigned char[nCompressSizeTemp+1];
//	const uLong nDestSize = (*pDestSize);
//	uLong nSrcIndex = 0;
//	*pDestSize = 0;
//	while (nSrcIndex<nSrcSize && (*pDestSize)<nDestSize)
//	{
//		const uLong nCompressSize = min(const_pack_size,(nSrcSize-nSrcIndex));
//		uLong nCompressSizeReal = nSrcSize-nSrcIndex;
//		const int nRet = compress2(pDest+(*pDestSize),&nCompressSizeReal,(const unsigned char*)pSrc+nSrcIndex,nCompressSize,level);
//		if (nRet != Z_OK)
//			return nRet;
//
//		nSrcIndex += nCompressSize;
//		(*pDestSize) += nCompressSizeReal;
//	}
//	return Z_OK;
//}
//int UnZip(const unsigned char* pSrc, uLong nSrcSize, unsigned char* pDest, uLong* pDestSize)
//{
//	const uLong nDestSize = *pDestSize;
//	uLong nSrcIndex = 0;
//	*pDestSize = 0;
//	while (nSrcIndex<nSrcSize && (*pDestSize)<nDestSize)
//	{
//		const uLong nUncompressSize = min(const_pack_size,(nSrcSize-nSrcIndex));
//		uLong nUncompressSizeReal = nSrcSize-nSrcIndex;
//		const int nRet = uncompress(pDest+(*pDestSize),&nUncompressSizeReal,(const unsigned char*)pSrc+nSrcIndex,nUncompressSize);
//		if (nRet != Z_OK)
//			return nRet;
//
//		nSrcIndex += nUncompressSize;
//		(*pDestSize) += nUncompressSizeReal;
//	}
//	return Z_OK;
//}

class CZIB
{
public:
	bool OpenZib(int level=Z_DEFAULT_COMPRESSION, int gzip=0)
	{
		if (m_strm == NULL)
		{
			m_bZip = true;
			int ret;
			m_strm = new z_stream;
			memset(m_strm,0,sizeof(z_stream));
			//m_strm->zalloc = Z_NULL;
			//m_strm->zfree = Z_NULL;
			//m_strm->opaque = Z_NULL;
			if (gzip)
				ret = deflateInit2(m_strm, level, Z_DEFLATED, DEFAULT_WINDOWBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
			else
				ret = deflateInit(m_strm, level);
			if (ret != Z_OK)
			{
				delete m_strm;
				m_strm = NULL;
				return false;
			}
		}
		return true;
	}
	bool OpenUnZib(int level=Z_DEFAULT_COMPRESSION, int gzip=0)
	{
		if (m_strm == NULL)
		{
			m_bZip = false;
			int ret;
			m_strm = new z_stream;
			memset(m_strm,0,sizeof(z_stream));
			//m_strm->zalloc = Z_NULL;
			//m_strm->zfree = Z_NULL;
			//m_strm->opaque = Z_NULL;
			//m_strm->avail_in = 0;
			//m_strm->next_in = Z_NULL;
			if (gzip)
				ret = inflateInit2(m_strm, DEFAULT_WINDOWBITS);
			else
				ret = inflateInit(m_strm);
			if (ret != Z_OK)
			{
				delete m_strm;
				m_strm = NULL;
				return false;
			}
		}
		return true;
	}
	bool IsInited(void) const {return m_strm==NULL?false:true;}
	void Close(void)
	{
		if (m_strm!=NULL)
		{
			if (m_bZip)
				(void)deflateEnd(m_strm);
			else
				(void)inflateEnd(m_strm);
			delete m_strm;
			m_strm = NULL;
		}
	}
	bool IsLastDataFlag(void) const {return (m_strm->avail_out!=0)?true:false;}	// for callback user
	int File2Cb(FILE *source, uLong nSourceSize, ZipDataCallBack pCb, unsigned int nUserData, uLong nPackSize=0, uLong* pOutSize=NULL)
	{
		if (!IsInited()) return Z_ERRNO;
		int ret, flush;
		unsigned have;

		if (nSourceSize==0)
		{
#ifdef WIN32
			_fseeki64(source, 0, SEEK_END);
			nSourceSize = (uLong)_ftelli64(source);
			_fseeki64(source, 0, SEEK_SET);
#else
			fseeko(source, 0, SEEK_END);
			nSourceSize = (uLong)ftello(source);
			fseeko(source, 0, SEEK_SET);
#endif
		}
		const uLong const_pack_size = nPackSize>0?nPackSize:CONST_ZIP_PACK_SIZE(nSourceSize);	
		unsigned char * in = new unsigned char[const_pack_size];
		if (in==NULL)
		{
			return Z_MEM_ERROR;
		}
		unsigned char * out = new unsigned char[const_pack_size];
		if (out==NULL)
		{
			delete[] in;
			return Z_MEM_ERROR;
		}

		if (pOutSize!=NULL)
			*pOutSize = 0;
		uLong nSourceIndex = 0;
		/* compress until end of file */
		do {
			const unsigned int nIn = min(const_pack_size,(nSourceSize-nSourceIndex));
			nSourceIndex += nIn;
			m_strm->avail_in = (unsigned int)fread(in, 1, nIn, source);
			if (ferror(source)) {
				delete[] in;
				delete[] out;
				return Z_ERRNO;
			}
			flush = (nSourceIndex>=nSourceSize||feof(source)) ? Z_FINISH : Z_NO_FLUSH;
			m_strm->next_in = in;

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do {
				m_strm->avail_out = const_pack_size;
				m_strm->next_out = out;
				ret = deflate(m_strm, flush);    /* no bad return value */
				//assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = const_pack_size - m_strm->avail_out;
				if (pOutSize!=NULL)
					*pOutSize += have;
				if (!pCb((nSourceIndex-nIn),out,have,nUserData))
				{
					delete[] in;
					delete[] out;
					return Z_ERRNO;
				}
			} while (m_strm->avail_out == 0);
			//assert(strm.avail_in == 0);     /* all input will be used */

			/* done when last data in file processed */
		} while (flush != Z_FINISH);
		//assert(ret == Z_STREAM_END);        /* stream will be complete */

		/* clean up and return */
		delete[] in;
		delete[] out;
		return Z_OK;
	}
	CZIB(void)
		: m_bZip(false), m_strm(NULL)
	{}
	virtual ~CZIB(void)
	{
		Close();
	}
private:
	bool m_bZip;
	z_stream* m_strm;
};

#endif // __zlib_H__
