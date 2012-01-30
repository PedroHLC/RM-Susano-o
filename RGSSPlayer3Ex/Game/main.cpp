/*
 *	@file		main.h
 *
 *	@desc		µÚÈý·½ RGSS Player
 *
 *	@author		×ÆÑÛµÄÏÄÄÈ
 *
 *	@history	2009/07/21	³õ°æ
 */

//#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <Psapi.h>
#include "resource.h"

//RGSSPP
#define EXE_EXTERN extern "C" __declspec(dllexport)

#define Qnil   4
#define NIL_P(v) ((VALUE)(v) == Qnil)

typedef unsigned long VALUE;

struct RBasic {
    unsigned long flags;
    VALUE klass;
};

struct RString {
    struct RBasic basic;
    long len;
    char *ptr;
    union {
        long capa;
        VALUE shared;
    } aux;
};

EXE_EXTERN int ByPedroHLC(int str_iD, int psw_iD){
             //Getinfo
             RString* str_d = ((RString*)(str_iD << 1));
             RString* psw_d = ((RString*)(psw_iD << 1));
             char* str_c = str_d->ptr;
             char* psw_c = psw_d->ptr;
             int str_l = str_d->len;
             int psw_l = psw_d->len;
             //Decrypt
             int di=0, pi=0;
             while(di < str_l){
                      for(pi=0; pi < psw_l; pi++){
                                str_c[di] -= psw_c[pi];
                                di++;
                                if(di >= str_l) break;
                      }
             }
             return(di);
}

void LoadResFile(int ResID, LPCWSTR ResFileName)
{
    DWORD lpNumBytes;
	HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE (ResID), RT_RCDATA);
	
	HGLOBAL hResLoad = LoadResource(NULL, hRes);
	LPVOID lpResLock = LockResource(hResLoad);
	DWORD dwSizeRes = SizeofResource(NULL, hRes);
	
	HANDLE hFile = CreateFileW(ResFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, lpResLock, dwSizeRes, &lpNumBytes, NULL);
	
	FreeResource(hRes);
	FreeResource(hResLoad);
	CloseHandle(hFile);
}

LPWSTR GetPIN(HINSTANCE hInstance){
	wchar_t exeImageName[512];
	GetModuleFileNameW(hInstance, exeImageName, 512);
	LPWSTR baseExeImageName = wcsrchr(exeImageName, '\\');
    baseExeImageName = baseExeImageName ? baseExeImageName : exeImageName;
	return(baseExeImageName);
}

//RGSS Player

static const wchar_t*	pWndClassName	= L"RGSS3 Player";

static const wchar_t*	pDefaultLibrary	= L"rgss3.dll";
static const wchar_t*	pDefaultTitle	= L"Untitled";
static const wchar_t*	pDefaultScripts	= L"Data\\Scripts.rxdata";

static const int nScreenWidth	= 544;
static const int nScreenHeight	= 416;

static const int nEvalErrorCode	= 6;

void ShowErrorMsg(HWND hWnd, const wchar_t* szTitle, const wchar_t* szFormat, ...)
{
	static wchar_t szError[1024];

	va_list ap;
	va_start(ap, szFormat);
	vswprintf_s(szError, szFormat, ap);
	va_end(ap);

	MessageBoxW(hWnd, szError, szTitle, MB_ICONERROR);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	if(FindWindowW(pWndClassName, NULL)!=0) {
		MessageBoxW(0, L"Um jogo em RGSS já está sendo executado, fizalize-o e reincie este jogo.", L"", 0);
		return 0;
	}

	wchar_t szAppPath[MAX_PATH], szIniPath[MAX_PATH], szRgssadPath[MAX_PATH];
	wchar_t szLibrary[MAX_PATH], szTitle[MAX_PATH], szScripts[MAX_PATH];
	wchar_t*	pRgssad = 0;
	HWND	hWnd = NULL;
	HMODULE hRgssCore = NULL;

	LPWSTR cPIN=GetPIN(hInstance);
	//  EXE Image Name
	if(wcscmp(cPIN, L"\\Game.exe")){
		ShowErrorMsg(0, L"Teste", L"O aplicativo deve chamar \"Game.exe\"", cPIN);
		return(0);
	}

	//	appÂ·¾¶
	DWORD len = ::GetModuleFileNameW(hInstance, szAppPath, MAX_PATH);
	for (--len; len > 0; --len)
	{
		if (szAppPath[len] == '\\' || szAppPath[len] == '/')
		{
			szAppPath[len] = 0;
			break;
		}
	}
	SetCurrentDirectoryW(szAppPath);

	//	iniÎÄ¼þÂ·¾¶
	len = ::GetModuleFileNameW(hInstance, szIniPath, MAX_PATH);
	szIniPath[len - 1] = 'i';
	szIniPath[len - 2] = 'n';
	szIniPath[len - 3] = 'i';

	//	¼ÓÃÜ°üÂ·¾¶
	len = ::GetModuleFileNameW(hInstance, szRgssadPath, MAX_PATH);
	for (--len; len > 0; --len)
	{
		if (szRgssadPath[len] == '.')
		{
			memcpy(&szRgssadPath[len + 1], "rgss3a", strlen("rgss3a") + 1);
			break;
		}
	}

	// dll
	wchar_t fulllibrarypath[520], *temppath;
	temppath = _wgetenv(L"TEMP");
	if(temppath != NULL){
		wcscpy(fulllibrarypath, temppath);
		wcscat(fulllibrarypath, L"\\");
		wcscat(fulllibrarypath, pDefaultLibrary);
		wcscpy(szLibrary,	fulllibrarypath);	
	}else{
		memcpy(szLibrary,	pDefaultLibrary,	wcslen(pDefaultLibrary) + 1);
	}

	//	iniÎÄ¼þ´æÔÚ
	if (GetFileAttributesW(szIniPath) != INVALID_FILE_ATTRIBUTES)
	{
		//GetPrivateProfileStringW(L"Game", L"Library", pDefaultLibrary,	szLibrary,	MAX_PATH, szIniPath);
		GetPrivateProfileStringW(L"Game", L"Title",	pDefaultTitle,		szTitle,	MAX_PATH, szIniPath);
		GetPrivateProfileStringW(L"Game", L"Scripts", pDefaultScripts,	szScripts,	MAX_PATH, szIniPath);
	}
	else
	{
		//memcpy(szLibrary,	pDefaultLibrary,	wcslen(pDefaultLibrary) + 1);
		memcpy(szTitle,		pDefaultTitle,		wcslen(pDefaultTitle) + 1);
		memcpy(szScripts,	pDefaultScripts,	wcslen(pDefaultScripts) + 1);
	}

	if (GetFileAttributesW(szRgssadPath) != INVALID_FILE_ATTRIBUTES)
		pRgssad = szRgssadPath;

	//	´´½¨´°¿Ú
	WNDCLASSW winclass;

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= DefWindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hInstance;
	winclass.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL; 
	winclass.lpszClassName	= pWndClassName;	
	
	if (!RegisterClassW(&winclass))
	{
		ShowErrorMsg(hWnd, szTitle, L"A seguinte classe não pode ser criada: '%s'", pWndClassName);
		return 0;
	}
	
	int width	= nScreenWidth + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	int height	= nScreenHeight + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

	RECT rt;
	{
		rt.left		= (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
		rt.top		= (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		rt.right	= rt.left + width;
		rt.bottom	= rt.top + height;
	}

	DWORD dwStyle = (WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE);

	hWnd = ::CreateWindowExW(WS_EX_WINDOWEDGE, pWndClassName, szTitle, dwStyle,
		rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, 0, 0, hInstance, 0);
	if (!hWnd)
	{
		ShowErrorMsg(hWnd, szTitle, L"A seguinte janela não pode ser encontrada: '%s'", szTitle);
		goto __exit;
	}

	ShowWindow(hWnd, SW_SHOW);

	//	¼ÓÔØRGSSºËÐÄ¿â
	LoadResFile(IDR_RCDATA1, szLibrary);
	hRgssCore = ::LoadLibraryW(szLibrary);
	if (!hRgssCore)
	{
		ShowErrorMsg(hWnd, szTitle, L"A Lybrary %s está corrompida.", szLibrary);
		goto __exit;
	}

	typedef BOOL		(*RGSSSetupRTP)(const wchar_t* pIniPath, wchar_t* pErrorMsgBuffer, int iBufferLength);
	typedef void		(*RGSSInitialize3)(HMODULE hRgssDll);
	typedef int			(*RGSSEval)(const char* pScripts);
	typedef void		(*RGSSGameMain)(HWND hWnd, const wchar_t* pScriptNames, wchar_t** pRgssadName);
	typedef BOOL		(*RGSSExInitialize)(HWND hWnd);

	RGSSSetupRTP		pRGSSSetupRTP		= NULL;
	RGSSInitialize3		pRGSSInitialize3	= NULL;
	RGSSEval			pRGSSEval			= NULL;
	RGSSGameMain		pRGSSGameMain		= NULL;
	RGSSExInitialize	pRGSSExInitialize	= (RGSSExInitialize)::GetProcAddress(hRgssCore, "RGSSExInitialize");

	#define __get_check(fn)														\
	do																			\
	{																			\
		p##fn = (fn)::GetProcAddress(hRgssCore, #fn);							\
		if (!p##fn)																\
		{																		\
			ShowErrorMsg(hWnd, szTitle, L"O RGSS não pode ser inicializado. Argumento: '%s'", #fn);\
			goto __exit;														\
		}																		\
	} while (0)
	{
		__get_check(RGSSSetupRTP);
		__get_check(RGSSInitialize3);
		__get_check(RGSSEval);
		__get_check(RGSSGameMain);
	}
	#undef __get_check

	//	1¡¢ÉèÖÃRTP
	wchar_t szRtpName[1024];

	if (!pRGSSSetupRTP(szIniPath, szRtpName, 1024))
	{
		ShowErrorMsg(hWnd, szTitle, L"O RGSS-RTP %s não está instalado.", szRtpName);
		goto __exit;
	}

	//	2¡¢³õÊ¼»¯
	pRGSSInitialize3(hRgssCore);

	//	2.1¡¢À©Õ¹¿â³õÊ¼»¯£¨²¹¶¡Ä£Ê½£©
	if (pRGSSExInitialize)
	{
		if (!pRGSSExInitialize(hWnd))
		{
			ShowErrorMsg(hWnd, szTitle, L"RGSS Error: 2.1", "RGSSExInitialize");
			goto __exit;
		}
	}

	/*//  Check if key.cfg exists
	FILE * rgssppkeyFile;
    rgssppkeyFile = fopen ("key.cfg","rb");
	if (rgssppkeyFile == NULL)
	{
		ShowErrorMsg(hWnd, szTitle, "O arquivo %s não pode ser encontrado!", "key.cfg");
		goto __exit;
	}
	fclose(rgssppkeyFile);*/

	//  RGSSPP
	int i, z;
	//Data
	char cmd1[] = "mprtyF…+H+Qtwp9z{py329gRlxp9pƒp27+2}m249}ploF/}r~~{{jvp„+H+3…f;ƒCQ;;=h9zj~+6+…f;ƒCQ;;?h9zj~+6+…f;ƒCQ;;Ah9zj~49zjtF/}r~~{{jqy+H+bty>=L[T9yp‚329gRlxp9pƒp27+2M„[po}zSWN27+2TT27+2T24F}p~n€pF{}ty+2qltwpo+z+opn}„{+rlxp2Fpƒt3>>4FpyoFopq+wzlojoll3{ls4F~}+H+Qtwp9z{py3{ls7+2}m249}ploFq~t…p+H+~}9~t…pF{~‚+H+333~}9~t…p+:+=455=+6+/}r~~{{jvp„+6+<=4:=49zj~F/}r~~{{jqy9nlww3~}9jjtojj7+{~‚9jjtojj4F}p€}y+Xl}~slw9wzlo3~}4FpyoF";
	for(i=0; i < 427; i++) cmd1[i] -= 11;
	z = pRGSSEval(cmd1);
	if(z == 6) MessageBoxA(hWnd, "Sorry, data decryption will not work.", "RGSS Player", 0);
	for(i=0; i < 427; i++) cmd1[i] += 11;
	/*//Graphics
	char cmd2[] = ")YJRUNRL%B%JS[`,Yjru,b%0%,yjrunrl3ify,@ijk%ijhw~uydnrl-ufym.@ijxhtrunqjidnrl%B%Knqj3tujs-,34,%0%ufym%0%,3wrn,1%,wg,.3wjfi@ux|%B%---ijxhtrunqjidnrl3xnj%4%7.//7%0%)wlxxuudpj~%0%67.47.3ytdx@)wlxxuudks3hfqq-ijxhtrunqjidnrl3ddnidd1%ux|3ddnidd.@ux|%B%snq@ijxhtrunqjidknqj%B%Knqj3tujs-)YJRUNRL1%,|g,.@ijxhtrunqjidknqj3|wnyj-ijxhtrunqjidnrl.@ijxhtrunqjidknqj3hqtxj@jsi@";
	for(i=0; i < 362; i++) cmd2[i] -= 5;
	z = pRGSSEval(cmd2);
	if(z == 6) MessageBoxA(hWnd, "Graphics decryption will not work.", "RGSS Player", 0);
	for(i=0; i < 362; i++) cmd2[i] += 5;
	//Audio
	char cmd3[] = "(XIQTEYH$A$IRZ_+Xiqt+a$/$+xiqteyh2hex+?(XIQTWI$A$IRZ_+Xiqt+a$/$+xiqtwi2hex+?qshypi$Eyhms?gpeww$@@$wipj?epmew$keqicgvmtxceyhmscfkqctpe}$fkqctpe}?epmew$keqicgvmtxceyhmscfkwctpe}$fkwctpe}?epmew$keqicgvmtxceyhmscqictpe}$$qictpe}?epmew$keqicgvmtxceyhmscwictpe}$$wictpe}?hij$fkqctpe},reqi0$zspyqiA5440$tmxglA544-?vixyvr$mj$reqi$AA$Dsphcfkqcreqi?Dsphcfkqcreqi$A$reqi?mj$reqi$%A$++$erh$Jmpi2i|mwxC,+23+$/$reqi$/$+2vqe+-?keqicgvmtxceyhmscfkqctpe},+23Eyhms3Iqtx}2vqe+0$5440$544-?hiwgsqtmpihceyh$A$Jmpi2stir,+23+$/$reqi$/$+2vqe+0$+vf+-2vieh2xscw?tw{$A$,,,hiwgsqtmpihceyh2wm~i$3$6-..6$/$(vkwwttcoi}$/$56-36-2xscw?(vkwwttcjr2gepp,hiwgsqtmpihceyh2ccmhcc0$tw{2ccmhcc-?tw{$A$rmp?hiwgsqtmpihcjmpi$A$Jmpi2stir,(XIQTEYH0$+{f+-?hiwgsqtmpihcjmpi2{vmxi,hiwgsqtmpihceyh-?hiwgsqtmpihcjmpi2gpswi?reqi$A$(XIQTEYH?irh?viwypx$A$keqicgvmtxceyhmscfkqctpe},reqi0$zspyqi0$tmxgl-?Jmpi2hipixi,(XIQTEYH-$mj$Jmpi2i|mwxC,(XIQTEYH-?vixyvr$viwypx?irh?hij$fkwctpe},reqi0$zspyqiA5440$tmxglA544-?vixyvr$mj$reqi$AA$Dsphcfkwcreqi?Dsphcfkwcreqi$A$reqi?mj$reqi$%A$++$erh$Jmpi2i|mwxC,+23+$/$reqi$/$+2vqe+-?keqicgvmtxceyhmscfkwctpe},+23Eyhms3Iqtx}2vqe+0$5440$544-?hiwgsqtmpihceyh$A$Jmpi2stir,+23+$/$reqi$/$+2vqe+0$+vf+-2vieh2xscw?tw{$A$,,,hiwgsqtmpihceyh2wm~i$3$6-..6$/$(vkwwttcoi}$/$56-36-2xscw?(vkwwttcjr2gepp,hiwgsqtmpihceyh2ccmhcc0$tw{2ccmhcc-?tw{$A$rmp?hiwgsqtmpihcjmpi$A$Jmpi2stir,(XIQTEYH0$+{f+-?hiwgsqtmpihcjmpi2{vmxi,hiwgsqtmpihceyh-?hiwgsqtmpihcjmpi2gpswi?reqi$A$(XIQTEYH?irh?viwypx$A$keqicgvmtxceyhmscfkwctpe},reqi0$zspyqi0$tmxgl-?Jmpi2hipixi,(XIQTEYH-$mj$Jmpi2i|mwxC,(XIQTEYH-?vixyvr$viwypx?irh?hij$qictpe},reqi0$zspyqiA5440$tmxglA544-?vixyvr$mj$reqi$AA$Dsphcqicreqi?Dsphcqicreqi$A$reqi?mj$reqi$%A$++$erh$Jmpi2i|mwxC,+23+$/$reqi$/$+2vqe+-?keqicgvmtxceyhmscqictpe},+23Eyhms3Iqtx}2vqe+0$5440$544-?hiwgsqtmpihceyh$A$Jmpi2stir,+23+$/$reqi$/$+2vqe+0$+vf+-2vieh2xscw?tw{$A$,,,hiwgsqtmpihceyh2wm~i$3$6-..6$/$(vkwwttcoi}$/$56-36-2xscw?(vkwwttcjr2gepp,hiwgsqtmpihceyh2ccmhcc0$tw{2ccmhcc-?tw{$A$rmp?hiwgsqtmpihcjmpi$A$Jmpi2stir,(XIQTEYH0$+{f+-?hiwgsqtmpihcjmpi2{vmxi,hiwgsqtmpihceyh-?hiwgsqtmpihcjmpi2gpswi?reqi$A$(XIQTEYH?irh?viwypx$A$keqicgvmtxceyhmscqictpe},reqi0$zspyqi0$tmxgl-?Jmpi2hipixi,(XIQTEYH-$mj$Jmpi2i|mwxC,(XIQTEYH-?vixyvr$viwypx?irh?hij$wictpe},reqi0$zspyqiA5440$tmxglA544-?mj$reqi$%A$++$erh$Jmpi2i|mwxC,+23+$/$reqi$/$+2vqe+-?vixyvr$keqicgvmtxceyhmscwictpe},(XIQTWI0$zspyqi0$tmxgl-$mj$Dsphcwicreqi$AA$reqi?keqicgvmtxceyhmscwictpe},+23Eyhms3Iqtx}2vqe+0$5440$544-?hiwgsqtmpihceyh$A$Jmpi2stir,+23+$/$reqi$/$+2vqe+0$+vf+-2vieh2xscw?tw{$A$,,,hiwgsqtmpihceyh2wm~i$3$6-..6$/$(vkwwttcoi}$/$56-36-2xscw?(vkwwttcjr2gepp,hiwgsqtmpihceyh2ccmhcc0$tw{2ccmhcc-?tw{$A$rmp?hiwgsqtmpihcjmpi$A$Jmpi2stir,(XIQTWI0$+{f+-?hiwgsqtmpihcjmpi2{vmxi,hiwgsqtmpihceyh-?hiwgsqtmpihcjmpi2gpswi?reqi$A$(XIQTWI?irh?Dsphcwicreqi$A$reqi?viwypx$A$keqicgvmtxceyhmscwictpe},reqi0$zspyqi0$tmxgl-?vixyvr$viwypx?irh?irh?irh?";
	for(i=0; i < 2851; i++) cmd3[i] -= 4;
	z = pRGSSEval(cmd3);
	if(z == 6) MessageBoxA(hWnd, "Audio decryption will not work.", "RGSS Player", 0);
	for(i=0; i < 2851; i++) cmd3[i] += 4;
	//pRGSSEval("eval(File.open('Reader_Audio;.rb','r').read)");
	
	//	3¡¢ÉèÖÃÔËÐÐÊ±±äÁ¿
	//pRGSSEval("$TEST = false;$BTEST = false;");*/
	
	//	4¡¢Ö÷Âß¼­
	pRGSSGameMain(hWnd, szScripts, (pRgssad ? (wchar_t**)pRgssad : &pRgssad));	//	???

__exit:

	if (hRgssCore)
	{
		FreeLibrary(hRgssCore);
		hRgssCore = NULL;
	}

	if (hWnd)
	{
		DestroyWindow(hWnd);
		hWnd = NULL;
	}

	UnregisterClassW(pWndClassName, hInstance);

	return 0;
}
