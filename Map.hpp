#pragma once

#include "Draw.hpp"

class Map
{
private:
	//��ͼ����
	long lMapX;//���û���������
	long lMapY;//���û���������
	bool bMap[20][10];//y20*x10��bool����
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

	void RemoveLine(long lLineY)//ɾ��һ��
	{
		memmove(bMap[1], bMap[0], sizeof(bMap[0]) * lLineY);//�ѵ�0�п�ʼ��lLineY�п�������1�У��պø���lLineY��һ�У�
		memset(bMap[0], 0, sizeof(bMap[0]));//�ѵ�0�����Ϊ0���գ�
	}

	bool IsFullLine(long lLineY)
	{
		for (long lLineX = 0; lLineX < GetWidth(); ++lLineX)//��������
		{
			if (!bMap[lLineY][lLineX])
			{
				return false;//����һ���ո�
			}
		}
		return true;//�������κοո�
	}


	long GetHigh(void)
	{
		return 20;
	}

	long GetWidth(void)
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

	void Draw(Draw &csDraw, const char *cBlock = "��", const char *cBlank = "  ")
	{
		for (long y = 0; y < 4; ++y)
		{
			csDraw.SetCursorPos({lMapX, lMapY + y});
			for (long x = 0; x < 4; ++x)
			{
				if (bMap[y][x])
				{
					csDraw.WriteBuffer(cBlock, 2);
				}
				else
				{
					csDraw.WriteBuffer(cBlank, 2);
				}
			}
		}
	}

};