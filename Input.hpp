#pragma once

#include <conio.h>
#include <string.h>
#include <limits.h>

class Input//用户交互
{
public:
	struct Func
	{
		void (*pFunc)(void *);
		void *pParam;
	};

	enum LeadCode :long
	{
		Code_NL = 0,
		Code_00,
		Code_E0,
		Arr_Size,
	};

	struct Key
	{
		LeadCode enLeadCode;
		unsigned char ucKeyCode;

		Key(void) = default;
		Key(unsigned char _ucKeyCode) :enLeadCode(Code_NL), ucKeyCode(_ucKeyCode)
		{}
		Key(LeadCode _enLeadCode, unsigned char _ucKeyCode) :enLeadCode(_enLeadCode), ucKeyCode(_ucKeyCode)
		{}
	};
private:
	Func fRegisterTable[Arr_Size][UCHAR_MAX + 1];//前导字节，按键码
protected:
	static bool GetAndTranslateKey(Key &stKey)
	{
		if (_kbhit() != 0)
		{
			int iInput = _getch();

			LeadCode enCode;
			switch (iInput)
			{
			default:
				enCode = Code_NL;//正常按键
				//iInput保持不变
				break;
			case 0x00:
				enCode = Code_00;//转义
				iInput = _getch();//获取二层按键码
				break;
			case 0xE0:
				enCode = Code_E0;//转义
				iInput = _getch();//获取二层按键码
				break;
			}

			stKey.enLeadCode = enCode;
			stKey.ucKeyCode = iInput;
			return true;
		}

		return false;
	}
public:
	Input(void):fRegisterTable{0}
	{}
	~Input(void) = default;

	void RegisterKey(const Key &stKey, const Func &fFunc)
	{
		fRegisterTable[stKey.enLeadCode][stKey.ucKeyCode] = fFunc;
	}

	void UnRegisterKey(const Key &stKey)
	{
		fRegisterTable[stKey.enLeadCode][stKey.ucKeyCode] = Func{0};
	}

	void Reset(void)
	{
		memset(fRegisterTable, 0, sizeof(fRegisterTable));
	}

	void ProcessKey(void)//处理完所有按键返回（无阻塞）
	{
		Key stKey;

		while (GetAndTranslateKey(stKey))
		{
			//获取函数
			Func fFun = fRegisterTable[stKey.enLeadCode][stKey.ucKeyCode];
			if (fFun.pFunc != nullptr)//不为空则调用
			{
				fFun.pFunc(fFun.pParam);
			}
		}
	}

	static bool GetKey(const Key *pstArrKey, long lArrLength)
	{
		Key stKey;

		while (GetAndTranslateKey(stKey))
		{
			for (long i = 0; i < lArrLength; ++i)
			{
				if (stKey.enLeadCode == pstArrKey[i].enLeadCode && stKey.ucKeyCode == pstArrKey[i].ucKeyCode)
				{
					return true;
				}
			}
		}

		return false;
	}
};


/*
上下左右方向键
0xE0开始后跟
	 0x48
0x4B 0x50 0x4D

复合按键不是0x00开头就是0xE0开头

#include <conio.h>
#include <stdio.h>

int main(void)
{
	while (true)
	{
		printf("0x%02X ", _getch());
	}

	return 0;
}
*/