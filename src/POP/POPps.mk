
POPps.dll: dlldata.obj POP_p.obj POP_i.obj
	link /dll /out:POPps.dll /def:POPps.def /entry:DllMain dlldata.obj POP_p.obj POP_i.obj \
		kernel32.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \
.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del POPps.dll
	@del POPps.lib
	@del POPps.exp
	@del dlldata.obj
	@del POP_p.obj
	@del POP_i.obj
