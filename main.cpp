#include <iostream>
#include <windows.h>
#include <lm.h>
#include <fstream>
using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv)
{
	wchar_t LOCALGRP[100]= L"Administrators", HOSTNAME[MAX_COMPUTERNAME_LENGTH+1], PWD[MAX_PATH+1];
	DWORD bufSize = MAX_COMPUTERNAME_LENGTH;
	DWORD LEVEL=1, prefmaxlength=MAX_PREFERRED_LENGTH, entriesread, totalentries;
	PLOCALGROUP_MEMBERS_INFO_1 bufptr;
	FILE *RESULTFILE;
	system("cls");
			
	GetCurrentDirectoryW(MAX_PATH,PWD);
	GetComputerNameW(HOSTNAME,&bufSize);
	wcscat(PWD,L"\\");
	wcscat(PWD,HOSTNAME);
	wcscat(PWD,L".csv");
	RESULTFILE = _wfopen(PWD,L"w");

	fputws(L"\"Hostname\",\"Username\"\n",RESULTFILE);
	if(NetLocalGroupGetMembers(NULL,LOCALGRP,LEVEL,(LPBYTE *)&bufptr,prefmaxlength,&entriesread,&totalentries,0) == NERR_Success)
	{
		for(int i=0; i<entriesread; i++)
		{
			if(bufptr[i].lgrmi1_sidusage == SidTypeUser)
			{
				fputws(L"\"",RESULTFILE);
				fputws(HOSTNAME,RESULTFILE);
				fputws(L"\",\"",RESULTFILE);
				fputws(bufptr[i].lgrmi1_name,RESULTFILE);
				fputws(L"\"\n",RESULTFILE);
			}
		}
	}
	else
	{
	}
	
	NetApiBufferFree(&bufptr);
	fclose(RESULTFILE);
//	system("PAUSE");
	
	return 0;
}
