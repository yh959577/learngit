#include "stdafx.h"
#include "Tools.h"


Tools::Tools()
{
}


Tools::~Tools()
{
}

int Tools::char_to_int(char b1, char b2, char b3, char b4)
{
	return (((b1 & 0xFF) << 24) | ((b2 & 0xFF) << 16) | ((b3 & 0xFF) << 8) | (b4 & 0xFF));
}

char * Tools::int_to_char(int num)
{
	return new char[4] {(char)(num>>24&0xFF),(char)(num>>16&0xFF),(char)(num>>8&0xFF),(char)(num&0xFF)};
}

short Tools::char_to_short(char b1, char b2)
{
	return (short)((b1 & 0xff) << 8 | (b2 & 0xff));
}

bool Tools::isThreadExit(CWinThread * pThread)
{
	DWORD exitCode = 0;
	GetExitCodeThread(pThread,&exitCode);
	return exitCode==STILL_ACTIVE;
}


