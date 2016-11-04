#include <iostream>
#include <windows.h>
#include <lm.h>
#include <wininet.h>
#include <fstream>
#include <shlwapi.h>
using namespace std;

HANDLE hFile;
HINTERNET HINETOPEN, HINETCONNECT;
TCHAR uAgent [20]="GetAdminTool",MARKER[MAX_PATH+1];
DWORD AccessType = INTERNET_OPEN_TYPE_DIRECT;
TCHAR ftpServer[20] = "ftpserver";
TCHAR ftpUname[20]="ftpuser", ftpPass[14]="ftppass";
DWORD inetService = INTERNET_SERVICE_FTP;
FILETIME CTIME,STIME;
SYSTEMTIME SCTIME,SSTIME;
ULARGE_INTEGER UCTIME,USTIME;

int main(int argc, char** argv)
{
	wchar_t LOCALGRP[100]= L"Administrators", HOSTNAME[MAX_COMPUTERNAME_LENGTH+1], FILEPATH[MAX_PATH+1], MERGED[MAX_PATH+1]=L"\0";
	DWORD bufSize = MAX_COMPUTERNAME_LENGTH;
	DWORD LEVEL=1, prefmaxlength=MAX_PREFERRED_LENGTH, entriesread, totalentries;
	PLOCALGROUP_MEMBERS_INFO_1 bufptr;
	FILE *RESULTFILE=NULL;

/*------------------------------------ Check for the presense of Marker file ------------------------------------------------*/
	GetWindowsDirectory(MARKER,MAX_PATH);
	strcat(MARKER,"\\TEMP\\AdminMarker.txt"); //Build Marker file path
	if(PathFileExists(MARKER)) //Check if marker is created on same day
	{
		hFile=CreateFile(MARKER,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
		GetFileTime(hFile,&CTIME,NULL,NULL);
		GetSystemTimeAsFileTime(&STIME);
		UCTIME.LowPart=CTIME.dwLowDateTime;
		UCTIME.HighPart=CTIME.dwHighDateTime;
		USTIME.LowPart=STIME.dwLowDateTime;
		USTIME.HighPart=STIME.dwHighDateTime;
		FileTimeToSystemTime(&CTIME,&SCTIME);
		FileTimeToSystemTime(&STIME,&SSTIME);
		if(!(SSTIME.wYear-SCTIME.wYear))
		{
			if(!(SSTIME.wMonth-SCTIME.wMonth))
			{
				if(!(SSTIME.wDay-SCTIME.wDay))
				{
					goto EXIT_PROGRAM; //Exit program if marrker is created on same day.
				}
			}
		}
		CloseHandle(hFile);
		DeleteFile(MARKER); //Delete marker if it is not created on same day.
	}
	
	hFile=CreateFile(MARKER,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
	GetCurrentDirectoryW(MAX_PATH,FILEPATH);
	GetComputerNameW(HOSTNAME,&bufSize);
	wcscat(FILEPATH,L"\\");
	wcscat(FILEPATH,HOSTNAME);
	wcscat(FILEPATH,L".csv");
	RESULTFILE = _wfopen(FILEPATH,L"w");
	
	if(RESULTFILE)
	{
		fputws(L"\"Hostname\",\"Username\"\n",RESULTFILE);
		fputws(L"\"",RESULTFILE);
		fputws(HOSTNAME,RESULTFILE);
		fputws(L"\",\"",RESULTFILE);

/*------------------------------------------ Get list of objects from Administrators group of a system ------------------------------------------*/
		if(NetLocalGroupGetMembers(NULL,LOCALGRP,LEVEL,(LPBYTE *)&bufptr,prefmaxlength,&entriesread,&totalentries,0) == NERR_Success)
		{
			for(int i=0; i<entriesread; i++)
			{
				if(bufptr[i].lgrmi1_sidusage == SidTypeUser)
				{
					wcscat(MERGED,bufptr[i].lgrmi1_name);
					if(i!=entriesread-1)
					{
						wcscat(MERGED,L",");
					}
				}
			}
		}
		else
		{
			wcscpy(MERGED,L"Error while fetching data.");
		}

/*------------------------------------------ Write results to file ------------------------------------------*/
		fputws(MERGED,RESULTFILE);
		fputws(L"\"\n",RESULTFILE);	
		NetApiBufferFree(&bufptr);
		fclose(RESULTFILE);

/*------------------------------------------ Open network connection ------------------------------------------*/
		HINETOPEN = InternetOpen(uAgent,AccessType,NULL,NULL,INTERNET_FLAG_ASYNC);
		if(!HINETOPEN){ goto EXIT_PROGRAM; }
	
/*------------------------------------------ Connect to Remote FTP service -----------------------------------------*/
		HINETCONNECT = InternetConnect(HINETOPEN,ftpServer,21,ftpUname,ftpPass,inetService,INTERNET_FLAG_PASSIVE,0);
		if(!HINETCONNECT){ goto EXIT_PROGRAM; }

/*------------------------------------------ Upload file to FTP server ------------------------------------------*/
		if(!FtpPutFileW(HINETCONNECT,wcscat(HOSTNAME,L".csv"),HOSTNAME,FTP_TRANSFER_TYPE_BINARY,0)) { goto EXIT_PROGRAM; }

/*------------------------------------------- Free handles and buffers ---------------------------------------------*/
	}
	
	EXIT_PROGRAM:
	if(hFile){ CloseHandle(hFile); }
	if(HINETCONNECT) { InternetCloseHandle(HINETCONNECT); }
	if(HINETOPEN) { InternetCloseHandle(HINETOPEN); }
	
	return 0;
}
