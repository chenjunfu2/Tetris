#pragma once
#include <Windows.h>
#include <utility>

class Draw
{
private:
	HANDLE hConsole;
public:
	enum class TextColor :unsigned char
	{
		//字体色
		black = 0,
		blue = FOREGROUND_BLUE,
		green = FOREGROUND_GREEN,
		light_green = FOREGROUND_GREEN | FOREGROUND_BLUE,
		red = FOREGROUND_RED,
		purple = FOREGROUND_BLUE | FOREGROUND_RED,
		yellow = FOREGROUND_GREEN | FOREGROUND_RED,
		white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED,
		gray = FOREGROUND_INTENSITY,
		bright_blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		bright_green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		bright_light_green = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		bright_red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_purple = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		bright_white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		//背景色(使用时与字体色按位或:|)
		bg_blue = BACKGROUND_BLUE,
		bg_green = BACKGROUND_GREEN,
		bg_light_green = BACKGROUND_GREEN | BACKGROUND_BLUE,
		bg_red = BACKGROUND_RED,
		bg_purple = BACKGROUND_BLUE | BACKGROUND_RED,
		bg_yellow = BACKGROUND_GREEN | BACKGROUND_RED,
		bg_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED,
		bg_gray = BACKGROUND_INTENSITY,
		bg_bright_blue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		bg_bright_green = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		bg_bright_light_green = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		bg_bright_red = BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_purple = BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_yellow = BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
		bg_bright_white = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY,
	};

	struct CursorPos
	{
		long x, y;

		CursorPos(long _x, long _y) :x(_x), y(_y)
		{}

		CursorPos(const COORD &_crd) :x(_crd.X), y(_crd.Y)
		{}

		operator COORD(void) const
		{
			return COORD{(SHORT)x,(SHORT)y};
		}

		CursorPos &operator+=(const CursorPos &_r)
		{
			x += _r.x;
			y += _r.y;
			return *this;
		}

		CursorPos &operator-=(const CursorPos &_r)
		{
			x -= _r.x;
			y -= _r.y;
			return *this;
		}

		CursorPos operator+(const CursorPos &_r) const
		{
			return CursorPos(x + _r.x, y + _r.y);
		}

		CursorPos operator-(const CursorPos &_r) const
		{
			return CursorPos(x - _r.x, y - _r.y);
		}
	};

public:
	Draw(HANDLE _hConsole = INVALID_HANDLE_VALUE) :hConsole(_hConsole)
	{}

	Draw(const Draw &) = default;

	Draw(Draw &&_Move) noexcept :hConsole(_Move.hConsole)
	{
		_Move.hConsole = INVALID_HANDLE_VALUE;
	}

	~Draw(void) = default;

	Draw &operator=(Draw &&_Move) noexcept
	{
		hConsole = _Move.hConsole;
		_Move.hConsole = INVALID_HANDLE_VALUE;
		return *this;
	}

	HANDLE GetConsole(void)
	{
		return hConsole;
	}

	void SetConsole(HANDLE _hConsole)
	{
		hConsole = _hConsole;
	}

	DWORD WriteBuffer(const char *cpBuffer, DWORD dwWriteLen)
	{
		DWORD dwWrittenLen;
		if (!WriteConsoleA(hConsole, cpBuffer, dwWriteLen, &dwWrittenLen, NULL))
		{
			return 0;
		}
		return dwWrittenLen;
	}

	DWORD ClearBuffer(void)
	{
		DWORD dwWrittenLen;
		CONSOLE_SCREEN_BUFFER_INFO stConsoleInfo;
		
		if (!GetConsoleScreenBufferInfo(hConsole, &stConsoleInfo) ||
			!FillConsoleOutputAttribute(hConsole, 0, stConsoleInfo.dwSize.X * stConsoleInfo.dwSize.Y, COORD{0, 0}, &dwWrittenLen))
		{
			return 0;
		}

		return dwWrittenLen;
	}

	//设置光标坐标
	bool SetCursorPos(const CursorPos &stCursorPos)
	{
		return SetConsoleCursorPosition(hConsole, (COORD)stCursorPos);
	}

	//获取光标坐标
	CursorPos GetCursorPos(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return CursorPos(screen_buffer_info.dwCursorPosition);
	}

	//从当前位置移动光标
	bool MoveCursorPos(const CursorPos &stCursorPos)
	{
		return SetCursorPos(GetCursorPos() + stCursorPos);
	}

	//设置文本颜色
	bool SetTextColor(TextColor enColor)
	{
		return SetConsoleTextAttribute(hConsole, (WORD)enColor);
	}

	//获取文本颜色
	TextColor GetTextColor(void)
	{
		CONSOLE_SCREEN_BUFFER_INFO screen_buffer_info;
		GetConsoleScreenBufferInfo(hConsole, &screen_buffer_info);
		return (TextColor)screen_buffer_info.wAttributes;
	}

	//得到当前光标设置
	bool GetCursorShow(void)//return true代表光标可见
	{
		CONSOLE_CURSOR_INFO cursor_info;
		GetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
		return cursor_info.bVisible;
	}

	//光标隐藏||显示
	bool SetCursorShow(bool bShow)//Show=false代表光标不可见
	{
		CONSOLE_CURSOR_INFO cursor_info = {1, bShow};//1代表由光标填充的字符单元格的百分比。 此值介于1到100之间。 光标外观会变化，范围从完全填充单元格到单元底部的水平线条。
		return SetConsoleCursorInfo(hConsole, &cursor_info);//STD_OUTPUT_HANDLE  STD_INPUT_HANDLE
	}
};

Draw::TextColor operator|(const Draw::TextColor &_l, const Draw::TextColor &_r)//重载或运算符方便运算TextColor
{
	return (Draw::TextColor)((unsigned long)_l | (unsigned long)_r);
}

Draw::TextColor GetForegroundColor(const Draw::TextColor &_t)//获得前景色（低4位）
{
	return (Draw::TextColor)((unsigned long)_t & 0xF);
}

Draw::TextColor GetBackgroundColor(const Draw::TextColor &_t)//获得背景色（高4位）
{
	return (Draw::TextColor)((unsigned long)_t >> 4 & 0xF);
}

class DoubleBufferDraw
{
private:
	static constexpr long lBufferNum = 2;
	unsigned long ulCurrent;
	Draw csBuffer[lBufferNum];
	Draw csOldBuf;//保存对象，以便之后恢复
public:
	DoubleBufferDraw(void)
	{
		ulCurrent = 0;
		csOldBuf = GetStdHandle(STD_OUTPUT_HANDLE);//保存当前控制台初始句柄
		for (long i = 0; i < lBufferNum; ++i)
		{
			HANDLE hNewConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
			if (hNewConsole == INVALID_HANDLE_VALUE)
			{
				hNewConsole = csOldBuf.GetConsole();//创建失败使用初始句柄
			}
			csBuffer[i].SetConsole(hNewConsole);
		}
	}

	DoubleBufferDraw(DoubleBufferDraw &&_Move) noexcept :
		ulCurrent(_Move.ulCurrent),
		csOldBuf(std::move(_Move.csOldBuf))
	{
		for (long i = 0; i < lBufferNum; ++i)
		{
			csBuffer[i] = std::move(_Move.csBuffer[i]);
		}
	}

	DoubleBufferDraw(const DoubleBufferDraw &) = delete;

	~DoubleBufferDraw(void)
	{
		//恢复成原来的
		SetConsoleActiveScreenBuffer(csOldBuf.GetConsole());
		//销毁句柄
		for (long i = 0; i < lBufferNum; ++i)
		{
			HANDLE hDelConsole = csBuffer[i].GetConsole();
			if (hDelConsole != csOldBuf.GetConsole())//关闭所有非初始句柄
			{
				CloseHandle(hDelConsole);
			}
		}
	}

	DoubleBufferDraw &operator=(const DoubleBufferDraw &) = delete;
	DoubleBufferDraw &operator=(DoubleBufferDraw &&) = delete;

	void BegPrint(void)
	{
		csBuffer[ulCurrent].ClearBuffer();//清理缓冲区
	}

	void EndPrint(void)
	{
		SetConsoleActiveScreenBuffer(csBuffer[ulCurrent].GetConsole());//绘制结束输出
		ulCurrent = (ulCurrent + 1) % lBufferNum;//切换到下一个缓冲区
	}

	Draw &GetDraw(void)
	{
		return csBuffer[ulCurrent];
	}
};