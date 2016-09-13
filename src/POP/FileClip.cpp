// FileClip.cpp: implementation of the FileClip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "多个剪切板.h"
#include "FileClip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FileClip::FileClip()
{

}

FileClip::~FileClip()
{

}

void FileClip::Set_hWnd(HWND wnd)
{

	////////初始化/////////////////////////////
     hWnd=wnd;
	 count=0;
     fileClip.dataLength=0;
	 full=false;
	 show=false;
	 for(int i=0;i<5;i++){
	     fileData[i].time="";
		 fileData[i].fileclass="文件";
	 }
	////////////////////////////////////////
}



void FileClip::ReadData()
{

     if(OpenClipboard(hWnd)==false){ //打开剪切板
	    return;
	}
         HDROP hClip;

	     BYTE *pBuf=NULL;         //剪切板数据				
		 BYTE *buf=NULL;          //当前数据
		int format=0;           //格式
		unsigned int size=0;            //数据长度
		int sum;              //文件总数
        CTime time=CTime::GetCurrentTime();
		CString str,str1;         //显示的数据
   	 			 
///////////////////////////枚举数据类型////////////////////////
		while(1)
		{
			format++;
			if(format>=400){
			   CloseClipboard();       //关闭剪切板 
				return ;
			}
			if(IsClipboardFormatAvailable(format)){
				break;
			} 
				 
		}
//////////////////是何种类型//////////////////////////////////////////////////////
        if(format==CF_HDROP){           //文件
			   HDROP hDrop;
			   BYTE* buf=NULL;
			   unsigned int j=0;
			   int sum=0;           //个数
			   unsigned int dataLength=0;     //长度
               hDrop=(HDROP__ *)GetClipboardData(format);      //取出数据
			    GlobalUnlock(hDrop);               //解锁
			   sum=DragQueryFile(hDrop,0xFFFFFFFF,(char *)buf,0);   //得到文件个数
			   int i=0;
			  for(i=0;i<sum;i++){
			          j=DragQueryFile(hDrop,i,NULL,0)+1;       //得到当前文件名长度
                     
					  dataLength+=j;  //得到当前文件总名长度
			   }
               pBuf=(BYTE *)malloc(++(dataLength));    //开辟新的缓存
			   memset(pBuf,0,dataLength);
			  buf= pBuf;
			   for(i=0;i<sum;i++){
				      j=DragQueryFile(hDrop,i,NULL,0)+1;       //得到当前文件名长度
					  DragQueryFile(hDrop,i,(char *)buf,j);         //得到文件名
					  //buf[j]=NULL;                    //最后一个字符为空
					  //str1.Format("%s",buf);
					 
					  *(buf+j-1)='\0';      //格式为:"路径名\0文件1\0文件2\0" 
					   str+=PathToName(buf);//str+(str1+"\r\n");
					  buf=buf+j;
			   }
///////////////////////////判断数据是否一样//////////////////////////////////////////////////
               if(fileClip.dataLength!=dataLength){        //和当前长度不一样
			         fileClip.data=pBuf;
					 fileClip.dataLength=dataLength;
					 fileClip.sum=sum;
                     fileClip.fileclass="文件";
					 fileClip.time.Format("%s",time.Format("%H:%M:%S"));
					 fileClip.str=str;
                     WriteData(&fileClip);   //写入数据
			   }else{                         //和当前长度一样
				   if(!(ByteCmp(fileClip.data,pBuf,dataLength))){    //但数据不一样
				     fileClip.data=pBuf;
					 fileClip.dataLength=dataLength;
					 fileClip.sum=sum;
                     fileClip.fileclass="文件";
					 fileClip.time.Format("%s",time.Format("%H:%M:%S"));
					 fileClip.str=str;
					 WriteData(&fileClip);   //写入数据
				   }
			   }
			   
			    /*
			   buf=(unsigned char *)hDrop;
			   int ii=strlen((char *)buf);
               fileClip.str.Format("%s",buf+sizeof(DROPFILES));
			   */
			  
			   //fileClip.sum=DragQueryFile(hDrop,0xFFFFFFFF,(char *)buf,0)+1;
                //pBuf=(BYTE *)malloc(size);    //开辟新的缓存
               //DragQueryFile(fileClip.hClip,0,(char*)pBuf,size);
			    //memcpy(pBuf,fileClip.hClip,size);
        }
		CloseClipboard();       //关闭剪切板  
}

void FileClip::Put(int i)
{
     if(OpenClipboard(hWnd)){
		    HANDLE hClip;
	       DROPFILES  dFiles;
            BYTE *pBuf;
           
	      EmptyClipboard();
          hClip=GlobalAlloc(GMEM_SHARE|GMEM_MOVEABLE|GMEM_ZEROINIT,sizeof(DROPFILES)+(fileData[i].dataLength));            //分配移动堆
          
          pBuf=(BYTE*)GlobalLock(hClip);
		  tagPOINT pt;
		  pt.x=0; 
		  pt.y=0;
		  dFiles.pt=pt;
		  dFiles.fNC=false;
          dFiles.pFiles=sizeof(DROPFILES);
		  dFiles.fWide=false;
	      memcpy((char *)pBuf,((char *)&dFiles),sizeof(DROPFILES));
          BYTE *buf;
		  buf=pBuf+sizeof(DROPFILES);
		  //str+='\0';
	       //BYTE str[25]="d:\\123.gif\0d:\\lang.lang";
		   //fileData[i].data=str;
          memcpy(buf,fileData[i].data,fileData[i].dataLength);	
	      GlobalUnlock(hClip);         //解锁
          
	      SetClipboardData(CF_HDROP,hClip);        //设置数据
          CloseClipboard();                 //关闭  
		}

}

bool FileClip::ByteCmp(BYTE *left, BYTE *right,unsigned size)            //两个字符串是否一样
{
	  for(unsigned long i=0;i<size;i++){
	       if((BYTE)left[i]!=(BYTE)right[i]) 
		    	 return false;         //不一样
		}
     return true;
}

void FileClip::WriteData(FileData *fd)     //写入数据
{
	if(count>0||full==true){            //fileData中是否有一样的数据
	    Data_YN(fd);
	}

    if(!full){
	    fileData[count]=*fd; 
	}else{        
		free(fileData[0].data);       //释放
		for(int i=0;i<4;i++){
		       fileData[i]=fileData[i+1];
		}
		fileData[4]=*fd;	
	}
    
	count++;
	if(count==5){
	   count=0;
	   full=true;
	}
	show=true;

}

bool FileClip::Data_YN(FileData *fd)          //fileData中是否有一样的数据
{
    int count2=count;
    if(full) count2=5;
	for(int i=0;i<count2;i++){
		if(fd->dataLength==fileData[i].dataLength){  //长度一样
			if(ByteCmp(fd->data,fileData[i].data,fd->dataLength)){  //数据一样
				  free(fileData[i].data);   //释放
				  for(int j=i;j<count2-1;j++){
						  fileData[j]=fileData[j+1];
				  }
				  count--;     //总数减一
				  if(full){    //如果满了
					  full=false;
					  count=4;
				  }
			}
            
		}
	}
    
    count<0?count=4:count;
	 if(count==5){
		full=true;
	}
	return true;
}


CString FileClip::PathToName(BYTE *b)                 //取出文件名
{    CString name;
    int size=strlen((char *)b);
	for(int i=size-1;i>0;i--){        //找到'\'
		if(b[i]=='\\'){
		     name.Format("%s",b+i+1);
			 break;
		}
	}
	name+=",";
    return name;
}
