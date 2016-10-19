#include <iostream>
#include <windows.h>
#include <lm.h>
using namespace std;

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv)
{
	wchar_t LOCALGRP[100]= L"Administrators";
	DWORD LEVEL=1;
	LPBYTE bufptr,*USERINFO;
	DWORD prefmaxlength = MAX_PREFERRED_LENGTH;
	DWORD entriesread;
	DWORD totalentries;
	int i=0;
	
	system("cls");
	NetLocalGroupGetMembers(NULL,LOCALGRP,LEVEL,&bufptr,prefmaxlength,&entriesread,&totalentries,0);
	cout<<entriesread<<" "<<totalentries<<endl;
	
	LOCALGROUP_MEMBERS_INFO_1 *USER = new LOCALGROUP_MEMBERS_INFO_1[entriesread];
	USER=&(LOCALGROUP_MEMBERS_INFO_1 &)bufptr;
	cout<<sizeof(USER)<<endl;
		
	delete[] USER;
	NetApiBufferFree(&bufptr);
		
//	system("PAUSE");
	return 0;
}
