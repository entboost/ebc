# 编译环境
 VC2008(9.0)，使用其他版本可能会导致DLL版本不一致，运行错误等问题；

# 第三方库文件（请使用VC2008编译后方便使用）
+ [boost 1.62](http://www.boost.org/)
+ [cef_binary_3.2272.32.gbda8dc7_windows32](https://bitbucket.org/chromiumembedded/cef)

# 除了 boost 和 cef 二个第三方库，其他需要用到的 .h 头文件和 .lib 库文件，全部在 \src 目录下，请自行添加VC路径；
# 编译成功后复制 ebc.exe 文件到对应版本的恩布客户端，替换文件，尝试运行是否成功；
