// MrInfo.h file here
#ifndef __MrInfo_h__
#define __MrInfo_h__

namespace mr
{
#ifdef WIN32
	typedef __int64				bigint;
#else
	typedef long long			bigint;
#endif
} // mr namespace

// const CMrLine* pMrLine = (const CMrLine*)wParam;
// bool bSelected = lParam==1;
#define MR_WM_LINE_SELECTED (WM_USER+0x401)

typedef enum MR_ALIGNMENT
{
	MR_ALIGNMENT_LEFT
	, MR_ALIGNMENT_CENTER
	, MR_ALIGNMENT_RIGHT
};

typedef enum MR_BTN_TYPE
{
	MR_BTN_TYPE_NONE				= 0x000
	, MR_BTN_TYPE_LCLICK_CB			= 0x001
	, MR_BTN_TYPE_LDBLCLICK_CB		= 0x002
	, MR_BTN_TYPE_RCLICK_CB			= 0x004
	, MR_BTN_TYPE_LCLICK_OPEN		= 0x010
	, MR_BTN_TYPE_LDBLCLICK_OPEN	= 0x020
	, MR_BTN_TYPE_MOVE_ENTER_CB		= 0x100
	, MR_BTN_TYPE_MOVE_LEAVE_CB		= 0x200
};
//typedef enum MR_TIME_TYPE
//{
//	MR_TIME_TYPE_VIEW_YEAR_MONTH_DAY		= 0x1	// YYYY-mm-DD
//	, MR_TIME_TYPE_VIEW_HOUR_MINUTE_SECOND	= 0x2	// HH:MM:SS
//	, MR_TIME_TYPE_VIEW_HOUR_MINUTE			= 0x4	// HH:MM
//	, MR_TIME_TYPE_VIEW_DAY_OF_WEEK			= 0x8	// day of week
//};

typedef enum MR_ITEM_TYPE
{
	MR_ITEM_TYPE_TEXT
	, MR_ITEM_TYPE_CLICK_TEXT
	, MR_ITEM_TYPE_IMG
	, MR_ITEM_TYPE_TIME
	, MR_ITEM_TYPE_WAV
	, MR_ITEM_TYPE_URL
	, MR_ITEM_TYPE_LINE
	, MR_ITEM_TYPE_SPACE
	, MR_ITEM_TYPE_PIC
	, MR_ITEM_TYPE_OPENFILE	// 打开文件
	, MR_ITEM_TYPE_OPENDIR	// 打开文件目录
	, MR_ITEM_TYPE_ENTER	// 回车换行
	, MR_ITEM_TYPE_FRAME	// 显示外框

};

class CMrCallback
{
public:
	virtual MR_ALIGNMENT OnGetAlignment(void) const {return MR_ALIGNMENT_LEFT;}
	virtual int OnGetArcOffset(void) const {return 0;}								// 弧度
	virtual int OnGetPoloygonWidth(void) const {return 0;}							// 三角长度
	virtual int OnGetAlignmentValue1(void) const {return 10;}						// 左对齐，左边间隔
	virtual int OnGetAlignmentValue2(void) const {return 20;}						// 右对齐，左边间隔
	virtual int OnGetLineAlignmentValue(void) const {return 1;}					// 行间隔
	virtual const RectF& OnGetLineItemRect(void) const = 0;							// 行整体写字区域

};

#endif // __MrInfo_h__
