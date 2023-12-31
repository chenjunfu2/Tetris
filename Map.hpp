#pragma once

#include "Draw.hpp"

class Map
{
private:
	//地图数据
	long lMapX;//在用户界面坐标
	long lMapY;//在用户界面坐标
	bool bMap[20][10];//y20*x10的bool数组
public:
	Map(void) :lMapX(0), lMapY(0), bMap{0}
	{}

	~Map(void) = default;

	void Clear(void)
	{
		memset(bMap, 0, sizeof(bMap));
	}

	typedef bool(&RetMap)[10];
	RetMap operator[](long y)
	{
		return bMap[y];
	}

	bool IsCross(long x, long y)
	{
		return	x < 0 || x >= GetWidth() ||
				y < 0 || y >= GetHigh();
	}

	void RemoveLine(long lLineY)//删除一行
	{
		memmove(bMap[1], bMap[0], sizeof(bMap[0]) * lLineY);//把第0行开始的lLineY行拷贝到第1行（刚好覆盖lLineY那一行）
		memset(bMap[0], 0, sizeof(bMap[0]));//把第0行填充为0（空）
	}

	bool IsFullLine(long lLineY)
	{
		for (long lLineX = 0; lLineX < GetWidth(); ++lLineX)//遍历整行
		{
			if (!bMap[lLineY][lLineX])
			{
				return false;//存在一个空格
			}
		}
		return true;//不存在任何空格
	}


	long GetHigh(void) const
	{
		return 20;
	}

	long GetWidth(void) const
	{
		return 10;
	}

	void Move(long lMoveX, long lMoveY)
	{
		lMapX += lMoveX;
		lMapY += lMoveY;
	}

	void SetMapX(long _lMapX)
	{
		lMapX = _lMapX;
	}

	void SetMapY(long _lMapY)
	{
		lMapY = _lMapY;
	}

	long GetMapX(void) const
	{
		return lMapX;
	}

	long GetMapY(void) const
	{
		return lMapY;
	}

	void Draw(Draw &csDraw, const char *cBlock = "□", const char *cBorder = "■") const
	{
		//画外边框
		csDraw.SetCursorPos({GetMapX() - 1, GetMapY() - 1});
		for (long i = 0; i < GetWidth() + 2; ++i)
		{
			csDraw.WriteBuffer(cBorder, 2);
		}

		for (long y = 0; y < GetHigh(); ++y)
		{
			csDraw.SetCursorPos({GetMapX() - 1, GetMapY() + y});
			csDraw.WriteBuffer(cBorder, 2);//画外边框

			for (long x = 0; x < GetWidth(); ++x)
			{
				if (bMap[y][x])
				{
					csDraw.WriteBuffer(cBlock, 2);//画方块
				}
				else
				{
					csDraw.MoveCursorPos({2, 0});//跳空白
				}
			}

			csDraw.WriteBuffer(cBorder, 2);//画外边框
		}

		//画外边框
		csDraw.SetCursorPos({GetMapX() - 1, GetMapY() + GetHigh()});
		for (long i = 0; i < GetWidth() + 2; ++i)
		{
			csDraw.WriteBuffer(cBorder, 2);
		}
	}

};