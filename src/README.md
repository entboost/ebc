
# ebc编译文档
## 编译环境
 VC2008(9.0)，使用其他版本可能会导致DLL版本不一致，运行错误等问题；
## 第三方库文件（请使用VC2008编译后方便使用）
+ [boost 1.55](http://www.boost.org/)
+ [cef_binary_3.2272.32.gbda8dc7_windows32](https://bitbucket.org/chromiumembedded/cef)

## cef_binary_3.2272.32.gbda8dc7_windows32\include\internal\cef_types.h 文件91行位置，增加以下三行代码：
 #ifndef uint32
 
 #define uint32 unsigned int
 
 #endif
 
 typedef uint32              cef_color_t;

## \src\POP\cef目录下 client_handler.h,client_handler.cpp 注释掉 virtual bool OnFileDialog 函数；

## 新建 \src\build 目录，复制恩布客户端安装目录 ebcontrol.dll,eboleimage.dll 二个文件到该目录下；
## 除了 boost 和 cef 二个第三方库，其他需要用到的 .h 头文件和 .lib 库文件，全部在 \src 目录下，请自行添加到VC工程里面；
## 编译的时候，请选择编译 Debug 和 Release ，其他不需要编译；
## 编译成功后复制 ebc.exe 文件到恩布客户端，替换文件，尝试运行是否成功；
 