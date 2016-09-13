// EbcLanguages.h file here
#ifndef __EbcLanguages_H__
#define __EbcLanguages_H__

#include <stl/lockmap.h>
//#include <CGCBase/cgcobject.h>
#include <CGCBase/cgcSmartString.h>
using namespace cgc;

const tstring ebc_language_default_font_family_0	= "default_font_family_0";
const tstring ebc_language_default_font_family_1	= "default_font_family_1";
const tstring ebc_language_min						= "min";
const tstring ebc_language_max						= "max";
const tstring ebc_language_restore					= "restore";
const tstring ebc_language_close					= "close";
const tstring ebc_language_ok						= "ok";
const tstring ebc_language_cancel					= "cancel";
const tstring ebc_language_select_color				= "select_color";
const tstring ebc_language_color_0					= "color_0";
const tstring ebc_language_color_1					= "color_1";
const tstring ebc_language_color_2					= "color_2";
const tstring ebc_language_color_3					= "color_3";
const tstring ebc_language_color_4					= "color_4";
const tstring ebc_language_color_5					= "color_5";
const tstring ebc_language_color_6					= "color_6";
const tstring ebc_language_color_7					= "color_7";
const tstring ebc_language_color_8					= "color_8";
const tstring ebc_language_color_9					= "color_9";
const tstring ebc_language_sunday					= "sunday";
const tstring ebc_language_monday					= "monday";
const tstring ebc_language_tuesday					= "tuesday";
const tstring ebc_language_wednesday				= "wednesday";
const tstring ebc_language_thursday					= "thursday";
const tstring ebc_language_friday					= "friday";
const tstring ebc_language_saturday					= "saturday";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";
//const tstring ebc_language_		= "";


class CEbcLanguages
{
public:
	bool LoadLanguage(const char* lpszLanguageFilePath);
	bool Get(const char* lpszKey, tstring& pOutValue) const;

public:
	CEbcLanguages(void);
	virtual ~CEbcLanguages(void);

private:
	CLockMap<tstring,tstring> m_pLanguageList;
};

#endif // __EbcLanguages_H__
