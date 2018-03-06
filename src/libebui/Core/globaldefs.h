//
// GlobalDefs.h
//

#ifndef __H_GlobalDefs__
#define __H_GlobalDefs__


//-----------------------------------------------------------------------------
// Miscellaneous helper functions
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

// Attention: we place TCP listening on the client
#define SERVER_UDP_PORT		10096
#define CLIENT_UDP_PORT		10098
#define CLIENT_TCP_PORT		10099 

// Role definitions
typedef enum 
{
	SERVER = 0, 
	CLIENT
} ROLE;

// Device types
typedef enum
{
	DT_DV = 0,
	DT_USB_Camera,
	DT_Audio_Capture,
	DT_VirtualVideo,
	DT_Unknown
} Device_Type;

// How devices config to the peer PCs
typedef enum 
{
	Local_Has_Video  = 0x01, 
	Local_Has_Audio  = 0x02,
	Remote_Has_Video = 0x04,
	Remote_Has_Audio = 0x08,
	//Record_Client_AV = 0x10
	Local_Video_MPEG4Enc	= 0x10,
	Remote_Video_MPEG4Dec	= 0x20,
	Local_Video_H264Enc		= 0x1000,
	Remote_Video_H264Dec	= 0x2000,
	Local_Audio_AACEnc		= 0x40,
	Remote_Audio_AACDec		= 0x80,
	Remote_AV_SameGraph		= 0x100,	// Both use video graph.
	Record_Remote_AV		= 0x200,
	Split_Local_AV			= 0x400
	
} Device_Config;

const long Local_Device_Mask	= 0x03;
const long Remote_Device_Mask	= 0x0c;

// Preferred capture size
const long Preferred_Width		= 320;
const long Preferred_Height		= 240;

/*
// TCP stream header
struct Pack_Header
{
	long pack_type;
	long pack_size;

	void my_hton(void) 
	{
		pack_type		= htonl(pack_type);
		pack_size		= htonl(pack_size);
	};

	void my_ntoh(void) 
	{
		pack_type		= ntohl(pack_type);
		pack_size		= ntohl(pack_size);
	};
};

// TCP pack types
const long PT_AudioMediaType	= 10001;
const long PT_VideoMediaType	= 10002;
const long PT_Payload			= 10003;
 
// Messages
const long msg_FilterGraphError		= 'avct' + 1;
const long msg_MediaTypeReceived	= 'avct' + 2;
const long msg_TCPSocketAccepted	= 'avct' + 3;
const long msg_UDPCommandReceived	= 'avct' + 4;
const long msg_ModifyFilterGraph	= 'avct' + 5;

// Let the main thread modify filter graph
#define WM_ModifyFilterGraph		(WM_USER+123)

// UDP command defines
const long MAX_COMMAND_SIZE			= 100;
const long cmd_ClientCalling		= 'avct' + 100;
const long cmd_DeviceConfig			= 'avct' + 101;
const long cmd_BuildFilterGraph		= 'avct' + 102;
const long cmd_DisconnectRequest	= 'avct' + 103;

// UDP command pack
struct UDP_Pack
{
	long command;
	long param1;
	long param2;
	long reserved;

	void my_hton(void) 
	{
		command		= htonl(command);
		param1		= htonl(param1);
		param2		= htonl(param2);
		reserved	= htonl(reserved);
	};

	void my_ntoh(void) 
	{
		command		= ntohl(command);
		param1		= ntohl(param1);
		param2		= ntohl(param2);
		reserved	= ntohl(reserved);
	};
};
*/
#endif // __H_GlobalDefs__
