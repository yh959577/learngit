#pragma once
class Tools
{
public:
	Tools();
	~Tools();
	static int char_to_int(char b1, char b2, char b3, char b4);
	static char* int_to_char(int num);
	static short char_to_short(char b1, char b2);
	static bool isThreadExit(CWinThread* pThread);

};

