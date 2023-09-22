#pragma once

#include <conio.h>
#include <string.h>
#include <limits.h>

class Input//�û�����
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
	Func fRegisterTable[Arr_Size][UCHAR_MAX + 1];//ǰ���ֽڣ�������
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
				enCode = Code_NL;//��������
				//iInput���ֲ���
				break;
			case 0x00:
				enCode = Code_00;//ת��
				iInput = _getch();//��ȡ���㰴����
				break;
			case 0xE0:
				enCode = Code_E0;//ת��
				iInput = _getch();//��ȡ���㰴����
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

	void ProcessKey(void)//���������а������أ���������
	{
		Key stKey;

		while (GetAndTranslateKey(stKey))
		{
			//��ȡ����
			Func fFun = fRegisterTable[stKey.enLeadCode][stKey.ucKeyCode];
			if (fFun.pFunc != nullptr)//��Ϊ�������
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
�������ҷ����
0xE0��ʼ���
	 0x48
0x4B 0x50 0x4D

���ϰ�������0x00��ͷ����0xE0��ͷ

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