/*
The purpose of this test program is to check the behaviour of the "CreateHardLinkW" function on various Windows platforms
This programm shall be compiled with UNICODE defined

Christophe Baribaud 28/12/2011
*/

#include <windows.h>
#include <stdio.h>

static void *la_GetFunctionKernel32(const char *name)
{
	static HINSTANCE lib;
	static int set;
	if (!set)
	{
		set = 1;
		lib = LoadLibrary(L"kernel32.dll");
	}
	if (lib == NULL)
	{
		fprintf(stderr, "Can't load kernel32.dll?!\n");
		exit(1);
	}
	return (void *)GetProcAddress(lib, name);
}

bool RemoveBigPath(const WCHAR* _path)
{
	WCHAR *path = new WCHAR[wcslen(_path)+1];
	const WCHAR *s = _path;
	WCHAR *t = path;
	while (*s)
		*t++ = *s++;
	while (t > path+1)
	{
		t--;
		while (t > path && *t != L'\\')
			t--;
		if (t > path)
		{
			*t = 0;
			if (!wcscmp(path,L"C:"))
				break; // we are done
			if (!wcscmp(path,L"\\\\?\\C:"))
				break; // we are done
			if (!RemoveDirectory(path))
			{
				fwprintf(stderr,L"Unable to remove directory %ls error code is %d\n",path,GetLastError());
				delete path;
				return false;
			}
		}
	}
	delete path;
	return true;
}

WCHAR *CreateBigPath(int len,bool prefix,WCHAR fillChar)
{
	WCHAR *ret;
	if (prefix)
		ret = new WCHAR[len+1+4];
	else
		ret = new WCHAR[len+1];
	WCHAR *t = ret;
	if (prefix)
	{
		*t++ = L'\\';
		*t++ = L'\\';
		*t++ = L'?';
		*t++ = L'\\';
	}
	*t++ = L'C';
	len--;
	*t++ = L':';
	len--;
	WCHAR *base = t;
	while (t-base<len)
	{
		if (len-(t-base) < 16)
		{
			*t++ = L'\\';
			while (t-base<len)
			{
				*t++ = fillChar;
			}
		}
		else
		{
			WCHAR *lastGoodT = t;
			WCHAR *s = L"\\abcdef";
			while (*s)
				*t++ = *s++;
			*t++ = fillChar;
			*t = 0;
			if (!CreateDirectory(ret,NULL))
			{
				fwprintf(stderr,L"Unable to create path %ls, error code is %d\n",ret,GetLastError());
				*lastGoodT++ = L'\\';
				*lastGoodT = 0;
				RemoveBigPath(ret); // try a rollback
				delete ret;
				return NULL;
			}
		}
	}
	*t = 0;
	return ret;
}

bool CreateDummyFile(WCHAR *path)
{
	HANDLE hRet = CreateFile(
		path,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hRet == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr,"Unable to create file, last error code is %d\n",GetLastError());
		return false;
	}
	DWORD written = 0;
	if (!WriteFile(
		hRet,
		"dummy",
		5,
		&written,
		NULL) || written != 5)
	{
		fprintf(stderr,"Unable to write file, last error code is %d\n",GetLastError());
		CloseHandle(hRet);
		return false;
	}
	CloseHandle(hRet);
	return true;
}

bool DeleteDummyFile(const WCHAR *path)
{
	if (!DeleteFile(path))
	{
		fwprintf(stderr,L"Unable to delete file %ls, error code is %d\n",path,GetLastError());
		return false;
	}
	return true;
}

bool CreateHardLink(const WCHAR *linkname,const WCHAR *target,bool removePrefix)
{
	typedef BOOLEAN (WINAPI *f_CreateHardLink)(LPCWSTR, const LPCWSTR, LPSECURITY_ATTRIBUTES);
	static f_CreateHardLink f;
	static int set = 0;
	if (!set)
	{
		set = 1;
		f = (f_CreateHardLink)la_GetFunctionKernel32("CreateHardLinkW");
		if (!f)
		{
			fprintf(stderr,"CreateHardLinkW function not found in kernel32.dll\n");
			return false;
		}
	}
	if (removePrefix)
	{
		if (!wcsncmp(linkname,L"\\\\?\\",4))
			linkname+=4;
		if (!wcsncmp(target,L"\\\\?\\",4))
			target+=4;
	}
	if (!(*f)(linkname, target, NULL))
	{
		fwprintf(stderr,L"Unable to create hard link %ls -> %ls, error code is %d\n",linkname,target,GetLastError());
		return false;
	}
	return true;
}

int main(int,char**)
{
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH-10,false,L'x');
		if (p1)
		{
			//wprintf(L"%ls\n",p1);
			if (CreateDummyFile(p1) && DeleteDummyFile(p1) && RemoveBigPath(p1))
				fprintf(stdout,"Test 1 PASSED\n");
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH-10,true,L'x');
		if (p1)
		{
			//wprintf(L"%ls\n",p1);
			if (CreateDummyFile(p1) && DeleteDummyFile(p1) && RemoveBigPath(p1))
				fprintf(stdout,"Test 2 PASSED\n");
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH+30,false,L'x');
		if (p1)
		{
			//wprintf(L"%ls\n",p1);
			if (CreateDummyFile(p1) && DeleteDummyFile(p1) && RemoveBigPath(p1))
				fprintf(stdout,"Test 3 PASSED\n");
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH+30,true,L'x');
		if (p1)
		{
			//wprintf(L"%ls\n",p1);
			if (CreateDummyFile(p1) && DeleteDummyFile(p1) && RemoveBigPath(p1))
				fprintf(stdout,"Test 4 PASSED\n");
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH-10,true,L'x');
		if (p1)
		{
			WCHAR *p2 = CreateBigPath(_MAX_PATH-10,true,L'y');
			if (p2)
			{
				if (CreateDummyFile(p1))
				{
					if (CreateHardLink(p2,p1,false))
					{
						fprintf(stdout,"Test 5 PASSED\n");
						DeleteDummyFile(p2);
					}
					DeleteDummyFile(p1);
				}
				RemoveBigPath(p2);
				delete p2;
			}
			RemoveBigPath(p1);
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH-10,true,L'x');
		if (p1)
		{
			WCHAR *p2 = CreateBigPath(_MAX_PATH-10,true,L'y');
			if (p2)
			{
				if (CreateDummyFile(p1))
				{
					if (CreateHardLink(p2,p1,true))
					{
						fprintf(stdout,"Test 6 PASSED\n");
						DeleteDummyFile(p2);
					}
					DeleteDummyFile(p1);
				}
				RemoveBigPath(p2);
				delete p2;
			}
			RemoveBigPath(p1);
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH+30,true,L'x');
		if (p1)
		{
			WCHAR *p2 = CreateBigPath(_MAX_PATH+30,true,L'y');
			if (p2)
			{
				if (CreateDummyFile(p1))
				{
					if (CreateHardLink(p2,p1,false))
					{
						fprintf(stdout,"Test 7 PASSED\n");
						DeleteDummyFile(p2);
					}
					DeleteDummyFile(p1);
				}
				RemoveBigPath(p2);
				delete p2;
			}
			RemoveBigPath(p1);
			delete p1;
		}
	}
	{
		WCHAR *p1 = CreateBigPath(_MAX_PATH+30,true,L'x');
		if (p1)
		{
			WCHAR *p2 = CreateBigPath(_MAX_PATH+30,true,L'y');
			if (p2)
			{
				if (CreateDummyFile(p1))
				{
					if (CreateHardLink(p2,p1,true))
					{
						fprintf(stdout,"Test 8 PASSED\n");
						DeleteDummyFile(p2);
					}
					DeleteDummyFile(p1);
				}
				RemoveBigPath(p2);
				delete p2;
			}
			RemoveBigPath(p1);
			delete p1;
		}
	}
	return 0;
}
