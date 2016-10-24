#include <iostream>
#include <windows.h>
#include <lm.h>
#include <wininet.h>
#include <fstream>
using namespace std;

HINTERNET HINETOPEN, HINETCONNECT;
TCHAR uAgent [20]="GetAdminTool";
DWORD AccessType = INTERNET_OPEN_TYPE_DIRECT;
TCHAR ftpServer[20] = "ftpserver";
TCHAR ftpUname[20]="ftpuser", ftpPass[14]="ftppass";
DWORD inetService = INTERNET_SERVICE_FTP;

int main(int argc, char** argv)
{
	wchar_t LOCALGRP[100]= L"Administrators", HOSTNAME[MAX_COMPUTERNAME_LENGTH+1], PWD[MAX_PATH+1], MERGED[300]=L"\0";
	DWORD bufSize = MAX_COMPUTERNAME_LENGTH;
	DWORD LEVEL=1, prefmaxlength=MAX_PREFERRED_LENGTH, entriesread, totalentries;
	PLOCALGROUP_MEMBERS_INFO_1 bufptr;
	FILE *RESULTFILE;
			
	GetCurrentDirectoryW(MAX_PATH,PWD);
	GetComputerNameW(HOSTNAME,&bufSize);
	wcscat(PWD,L"\\");
	wcscat(PWD,HOSTNAME);
	wcscat(PWD,L".csv");
	RESULTFILE = _wfopen(PWD,L"w");
//	if(!RESULTFILE) { goto EXIT_PROGRAM; }

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
	EXIT_PROGRAM:
	if(HINETCONNECT) { InternetCloseHandle(HINETCONNECT); }
	if(HINETOPEN) { InternetCloseHandle(HINETOPEN); }
	
	return 0;
}
