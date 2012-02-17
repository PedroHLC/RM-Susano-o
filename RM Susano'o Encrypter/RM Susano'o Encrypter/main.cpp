// RM Susano'o Encrypter (Console Version).cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <string>
#include <direct.h>
#include <windows.h>
#include "resource.h"

using namespace std;

double password = HEY_PUT_A_SECRET_NUMBER_HERE;

//==============================================================================================================

const bool GetFolderExist(const char path[MAX_PATH]){
	return(GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
}
//==============================================================================================================
FILE* debugfile;
int dprintf(char* format, ...){
	va_list ap;
	va_start(ap, format);
	vfprintf(debugfile, format, ap);
	int result = vprintf(format, ap);
	va_end(ap);
	return result;
}
int dputs(char* str){
	fputs(str, debugfile);
	fputs("\n", debugfile);
	return puts(str);
}
//==============================================================================================================
int simplify_afloat_s(char* str, const size_t strsize){
     for(int i=(strsize - 1); i >= 0; i--){
       if(str[i]=='.'){
		   str[i] = 0;
           return i;
	   }else str[i] = 0;
     }
     return 0;
}
//==============================================================================================================
void strdowncase(char* str){
	int s = strlen(str);
	for(int i = 0; i < s; i++){
		str[i] = tolower(str[i]);
	}
}
//==============================================================================================================
void fcpy(char infname[MAX_PATH], char outfname[MAX_PATH]){
	char command[MAX_PATH * 2 + 12];
	sprintf(command, "copy \"%s\" \"%s\"", infname, outfname);
	system(command);
	return;
}
//==============================================================================================================
int encrypt_str(char* str){
     char cpassword[256], cbuffer;
	 double ipassword, strsize;
     int passsize, ci, pi;
     
	 strsize = strlen(str);
	 if(strsize < 1) return 1;
     ipassword = (pow(strsize/2, 2) + password + 12)/2;
	 sprintf(cpassword, "%f", ipassword, 64);
     simplify_afloat_s(cpassword, 256);
     passsize = strlen(cpassword);
	 
     ci = 0;
     while(ci < strsize){
              for(pi = 0; pi < passsize; pi++){
                     cbuffer = str[ci] + cpassword[pi];
					 str[ci] = cbuffer;
                     ci++;
                     if(ci >= strsize) break;
              }
              if(ci >= strsize) break;
     }
     
     return 0;
}
//==============================================================================================================
int encrypt_file(char* pathi, char* patho){
     FILE *original_file, *new_file;
     char *buffer="", cpassword[256], cbuffer;
	 double ipassword, filesize;
     int passsize, ci, pi;
     
     original_file = fopen(pathi, "rb");
     if(original_file == 0) return 1;
     new_file = fopen(patho, "wb");
     if(new_file == 0) return 2;
     
     fseek(original_file, 0, SEEK_END);
     filesize = ftell(original_file);
     if(filesize == 0) return 3;
     rewind(original_file);
     
	 strcpy(cpassword, "");
     ipassword = (pow((double)(int)(filesize/2), 2) + password + 12)/2;
	 sprintf_s(cpassword, 256, "%f", ipassword);
     simplify_afloat_s(cpassword, 256);
     passsize = strlen(cpassword);
     
	 buffer = (char*)malloc(sizeof(char)*filesize + 1);
	 fread(buffer, 1, filesize, original_file);

	 ci = 0;
     while(ci < filesize){
              for(pi = 0; pi < passsize; pi++){
                     cbuffer = buffer[ci] + cpassword[pi];
					 //if(cbuffer < 0) cbuffer += 256;
					 //if(cbuffer > 255) cbuffer -= 256;
					 buffer[ci] = cbuffer;
                     ci++;
                     if(ci >= filesize) break;
              }
              if(ci >= filesize) break;
     }

	 fwrite(buffer, 1, filesize, new_file);
     free(buffer);

     fclose(original_file);
     fclose(new_file);

     return 0;
}
//==============================================================================================================
int encrypt_data(char* subpath){
	int r, filenamesz=0;
	bool current=(strcmp(subpath, ".")==0);

	char fullipath[MAX_PATH]=".\\Backup\\Data\\", fullopath[MAX_PATH]=".\\Data";
	if(current)
		system("move .\\Data .\\Backup\\Data");
	else{
		strcat(fullipath, subpath);
		strcat(fullopath, "\\");
		strcat(fullopath, subpath);
	}
	strcat(fullipath, "\\*");

	_mkdir(fullopath);
	dputs("-------------------------------------------------------------------------------");

	WIN32_FIND_DATAA filesargs;
	HANDLE hFind;
	hFind = FindFirstFileA(fullipath, &filesargs);
	char tempi[MAX_PATH], tempo[MAX_PATH];

	if(hFind == INVALID_HANDLE_VALUE){
		dputs("Erro: Nao e possivel pesquisar arquivos na pasta \".\\Backup\\Data\".");
		FindClose(hFind);
		return 0;
	}else if(hFind == 0){
		dputs("Erro: Nao existem arquivos na pasta \".\\Backup\\Data\".");
		FindClose(hFind);
		return 1;
	}

	while(true){
		filenamesz = strlen(filesargs.cFileName);
		strdowncase(filesargs.cFileName);
		if((filesargs.cFileName[0] != '.') & (filesargs.cFileName[filenamesz-1] != '.'))
			if(strrchr(filesargs.cFileName,'.') != NULL){
				if(strstr(filesargs.cFileName,".rxdata") != NULL){
					dprintf("Encriptando: %s\n", filesargs.cFileName);
					
					strcpy(tempi, ".\\Backup\\Data\\");
					strcpy(tempo, ".\\Data\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);
		
					r = encrypt_file(tempi, tempo);
					if(r != 0){
						dputs("Falhou!");
						dprintf("Erro: A encriptacao do arquivo falhou, codigo do erro: EFALL%i\n", r);
						return 0;
					}
				}else{
					strcpy(tempi, ".\\Backup\\Data\\");
					strcpy(tempo, ".\\Data\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);

					fcpy(tempi, tempo);
				}
			}else{
				strcpy(tempo, "");
				if(!current){
					strcat(tempo, subpath);
					strcat(tempo, "\\");
				}
				strcat(tempo, filesargs.cFileName);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Encriptando a subpasta: %s...\n", tempo);
				encrypt_data(tempo);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Voltando a encriptar a pasta: Data...\n");
				dputs("-------------------------------------------------------------------------------");
			}
		if(FindNextFileA(hFind, &filesargs) == 0) break;
	}

	FindClose(hFind);
	return 1;
}
//==============================================================================================================
int encrypt_graphics(char* subpath){
	int r, filenamesz=0, ofilenamesz=0;
	bool current=(strcmp(subpath, ".")==0);

	char fullipath[MAX_PATH]=".\\Backup\\Graphics\\", fullopath[MAX_PATH]=".\\Graphics";
	if(current)
		system("move .\\Graphics .\\Backup\\Graphics");
	else{
		strcat(fullipath, subpath);
		strcat(fullopath, "\\");
		strcat(fullopath, subpath);
	}
	strcat(fullipath, "\\*");

	_mkdir(fullopath);
	dputs("-------------------------------------------------------------------------------");

	WIN32_FIND_DATAA filesargs;
	HANDLE hFind;
	hFind = FindFirstFileA(fullipath, &filesargs);
	char tempi[MAX_PATH], tempo[MAX_PATH];

	if(hFind == INVALID_HANDLE_VALUE){
		dputs("Erro: Nao e possivel pesquisar arquivos na pasta \".\\Backup\\Graphics\".");
		FindClose(hFind);
		return 0;
	}else if(hFind == 0){
		dputs("Erro: Nao existem arquivos na pasta \".\\Backup\\Graphics\".");
		FindClose(hFind);
		return 1;
	}

	while(true){
		filenamesz = strlen(filesargs.cFileName);
		strdowncase(filesargs.cFileName);
		if((filesargs.cFileName[0] != '.') & (filesargs.cFileName[filenamesz-1] != '.'))
			if(strrchr(filesargs.cFileName,'.') != NULL){
				if( (strstr(filesargs.cFileName,".jpg") != NULL) |
					(strstr(filesargs.cFileName,".bmp") != NULL) |
					(strstr(filesargs.cFileName,".png") != NULL) ){

					dprintf("Encriptando: %s\n", filesargs.cFileName);
					
					strcpy(tempi, ".\\Backup\\Graphics\\");
					strcpy(tempo, ".\\Graphics\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);
					ofilenamesz = strlen(tempo);
					tempo[ofilenamesz-3] = 'r';
					tempo[ofilenamesz-2] = 'm';
					tempo[ofilenamesz-1] = 'i';

					r = encrypt_file(tempi, tempo);
					if(r != 0){
						dputs("Falhou!");
						dprintf("Erro: A encriptacao do arquivo falhou, codigo do erro: EFALL%i\n", r);
						return 0;
					}
				}else{
					strcpy(tempi, ".\\Backup\\Graphics\\");
					strcpy(tempo, ".\\Graphics\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);

					fcpy(tempi, tempo);
				}
			}else{
				strcpy(tempo, "");
				if(!current){
					strcat(tempo, subpath);
					strcat(tempo, "\\");
				}
				strcat(tempo, filesargs.cFileName);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Encriptando a subpasta: %s...\n", tempo);
				encrypt_graphics(tempo);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Voltando a encriptar a pasta: Graphics...\n");
				dputs("-------------------------------------------------------------------------------");
			}
		if(FindNextFileA(hFind, &filesargs) == 0) break;
	}

	FindClose(hFind);
	return 1;
}
//==============================================================================================================
int encrypt_audios(char* subpath){
	int r, filenamesz=0, ofilenamesz=0;
	bool current=(strcmp(subpath, ".")==0);

	char fullipath[MAX_PATH]=".\\Backup\\Audio\\", fullopath[MAX_PATH]=".\\Audio";
	if(current)
		system("move .\\Audio .\\Backup\\Audio");
	else{
		strcat(fullipath, subpath);
		strcat(fullopath, "\\");
		strcat(fullopath, subpath);
	}
	strcat(fullipath, "\\*");

	_mkdir(fullopath);
	dputs("-------------------------------------------------------------------------------");

	WIN32_FIND_DATAA filesargs;
	HANDLE hFind;
	hFind = FindFirstFileA(fullipath, &filesargs);
	char tempi[MAX_PATH], tempo[MAX_PATH];

	if(hFind == INVALID_HANDLE_VALUE){
		dputs("Erro: Nao e possivel pesquisar arquivos na pasta \".\\Backup\\Audio\".");
		FindClose(hFind);
		return 0;
	}else if(hFind == 0){
		dputs("Erro: Nao existem arquivos na pasta \".\\Backup\\Audio\".");
		FindClose(hFind);
		return 1;
	}

	while(true){
		filenamesz = strlen(filesargs.cFileName);
		strdowncase(filesargs.cFileName);
		if((filesargs.cFileName[0] != '.') & (filesargs.cFileName[filenamesz-1] != '.'))
			if(strrchr(filesargs.cFileName,'.') != NULL){
				if( (strstr(filesargs.cFileName,".mid") != NULL) |
					(strstr(filesargs.cFileName,".wav") != NULL) |
					(strstr(filesargs.cFileName,".mp3") != NULL) |
					(strstr(filesargs.cFileName,".ogg") != NULL)
				){
					dprintf("Encriptando: %s\n", filesargs.cFileName);
					
					strcpy(tempi, ".\\Backup\\Audio\\");
					strcpy(tempo, ".\\Audio\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);
					ofilenamesz = strlen(tempo);
					tempo[ofilenamesz-3] = 'r';
					tempo[ofilenamesz-2] = 'm';
					tempo[ofilenamesz-1] = 'a';

					r = encrypt_file(tempi, tempo);
					if(r != 0){
						dputs("Falhou!");
						dprintf("Erro: A encriptacao do arquivo falhou, codigo do erro: EFALL%i\n", r);
						return 0;
					}
				}else{
					strcpy(tempi, ".\\Backup\\Audio\\");
					strcpy(tempo, ".\\Audio\\");
					if(!current){
						strcat(tempi, subpath);
						strcat(tempo, subpath);
						strcat(tempi, "\\");
						strcat(tempo, "\\");
					}
					strcat(tempi, filesargs.cFileName);
					strcat(tempo, filesargs.cFileName);

					fcpy(tempi, tempo);
				}
			}else{
				if(!current){
					strcpy(tempo, subpath);
					strcat(tempo, "\\");
				}else{
					strcpy(tempo, "");
				}
				strcat(tempo, filesargs.cFileName);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Encriptando a subpasta: %s...\n", tempo);
				encrypt_audios(tempo);
				dputs("-------------------------------------------------------------------------------");
				dprintf("Voltando a encriptar a pasta: Audio...\n");
				dputs("-------------------------------------------------------------------------------");
			}
		if(FindNextFileA(hFind, &filesargs) == 0) break;
	}

	FindClose(hFind);
	return 1;
}
//==============================================================================================================
int create_emptyaudio(){
	FILE* emptyrma=fopen(".\\Audio\\Empty.rma", "wb");
	if(emptyrma==0) return 0;
	fclose(emptyrma);
	return 1;
}
//==============================================================================================================
int main(){
	char szAppPath[MAX_PATH];
	HINSTANCE hInstance = GetModuleHandle(0);
	DWORD len = ::GetModuleFileNameA(hInstance, szAppPath, MAX_PATH);
	for (--len; len > 0; --len)
	{
		if (szAppPath[len] == '\\' || szAppPath[len] == '/')
		{
			szAppPath[len] = 0;
			break;
		}
	}
	SetCurrentDirectoryA(szAppPath);

	debugfile = fopen(".\\debug.txt", "w");

	int r=0;

    dputs("#==============================================================================");
    dputs("# Project RMSusano'o");
    dputs("# Por PedroHLC");
    dputs("# Ver 0.7.3 09/10/11");
    dputs("#------------------------------------------------------------------------------");
    dputs("# Agradecimentos a HugoLnx");
    dputs("#==============================================================================");

	dputs("-------------------------------------------------------------------------------\nCriando Backup...");
	if(GetFolderExist(".\\Backup")){
		dputs("Erro: A pasta \".\\Backup\" ja existe.\nAntes de continuar, verifique se o projeto ja esta compilado...\nCaso nao esteja, mova o seu conteudo para a pasta do projeto e delete-a.");
		goto __exit;
	}else if(_mkdir(".\\Backup") != 0){
		dputs("Erro: Nao foi possivel criar a pasta \".\\Backup\".");
		goto __exit;
	}
	
	dputs("-------------------------------------------------------------------------------\nEncriptando a pasta Data...");
	r = encrypt_data(".");
	if(r == 0) goto __exit;

	dputs("-------------------------------------------------------------------------------\nEncriptando a pasta Graphics...");
	r = encrypt_graphics(".");
	if(r == 0) goto __exit;

	dputs("-------------------------------------------------------------------------------\nEncriptando a pasta Audio...");
	r = encrypt_audios(".");
	if(r == 0) goto __exit;

	dputs("-------------------------------------------------------------------------------\nCriando \"Audio\\Empty.rma\"...");
	r = create_emptyaudio();
	if(r == 0) goto __exit;

__exit:
	dputs("-------------------------------------------------------------------------------");
	if(r != 0) dputs("Jogo encriptado com sucesso!\n");
	fclose(debugfile);
	dputs("Pressine ENTER para finalizar o aplicativo!");
    _getch();
    return EXIT_SUCCESS;
}