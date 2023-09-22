#pragma once

#include "Map.hpp"
#include "Block.hpp"
#include "Input.hpp"

#include <chrono>


class Logic
{
private:
	Map csMap;
	Block csBlock;
	DefaultBlock csDefaultBlock;

	unsigned long ulFallInterval;//下落间隔
	unsigned long ulLastFallTime;//上一次下落的时间
public:
	Logic(unsigned long _ulFallInterval = 10, long lStartX = 0, long lStartY = 0) :
		ulFallInterval(_ulFallInterval), ulLastFallTime(0)
	{
		csMap.SetMapX(lStartX);
		csMap.SetMapY(lStartY);
	}
	~Logic(void) = default;

	static void MoveLeft(void *pLogic)//左移
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(-1,0))
		{
			csLogic.csBlock.Move(-1, 0);
		}
	}

	static void MoveRight(void *pLogic)//右移
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(1, 0))
		{
			csLogic.csBlock.Move(1, 0);
		}
	}

	static void MoveUp(void *pLogic)//直接到位（按一下直接落到底）
	{
		return;//偷懒不做
	}

	static void MoveDown(void *pLogic)//加速下落（按一下下落一格）
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(0, 1))
		{
			csLogic.csBlock.Move(0, 1);
		}
	}

	static void RotateLeft(void *pLogic)//左旋转
	{
		Logic &csLogic = *(Logic *)pLogic;
		long lXMove, lYMove;
		if (csLogic.RotateDetection(-1, lXMove, lYMove))
		{
			csLogic.csBlock.Rotate(-1);
			csLogic.csBlock.Move(lXMove, lYMove);
		}
	}

	static void RotateRight(void *pLogic)//右旋转
	{
		Logic &csLogic = *(Logic *)pLogic;
		long lXMove, lYMove;
		if (csLogic.RotateDetection(1, lXMove, lYMove))
		{
			csLogic.csBlock.Rotate(1);
			csLogic.csBlock.Move(lXMove, lYMove);
		}
	}

	static void Pause(void *pLogic)
	{
		Input::Key stKey[] =
		{
			Input::Key('p'),
			Input::Key('P'),
		};

		while (!Input::GetKey(stKey, sizeof(stKey) / sizeof(Input::Key)))
		{
			Sleep(100);
		}
	}

	bool MoveDetection(long lXMove, long lYMove)//移动判断（判断是否能移动）
	{
		long lNewX = csBlock.GetBlockX() + lXMove;
		long lNewY = csBlock.GetBlockY() + lYMove;

		for (long j = 0; j < 4; ++j)
		{
			for (long i = 0; i < 4; ++i)
			{
				if (!csBlock[j][i])//block不存在，查看此列行中的下一列
				{
					continue;
				}
				//如果block存在并且(越界或没越界情况下map也存在)那么不可移动
				if (csMap.IsCross(lNewX + i, lNewY + j) || 
					csMap[lNewY + j][lNewX + i])//短路求值特性，Cross情况下不会访问Map
				{
					return false;
				}
			}
		}

		return true;
	}

	bool RotateDetection(long lRotate, long &lXMove, long &lYMove)//旋转判断（判断是否能旋转）
	{
		csBlock.Rotate(lRotate);//提前旋转

		//计算旋转挤压移动量（判断旋转后方块超出边界被挤压入地图内需要的移动量）
		lXMove = 0;
		for (long i = 0; i < 4; ++i)
		{
			for (long j = 0; j < 4; ++j)
			{
				if (!csBlock[j][i])//block不存在查看此列中的下一行
				{
					continue;
				}
				//如果向左越界则右移一格
				if (csBlock.GetBlockX() + i < 0)
				{
					++lXMove;
					break;
				}
				else if (csBlock.GetBlockX() + i >= csMap.GetWidth())//否则向右越界左移一格
				{
					--lXMove;
					break;
				}
			}
		}

		lYMove = 0;
		for (long j = 0; j < 4; ++j)
		{
			for (long i = 0; i < 4; ++i)
			{
				if (!csBlock[j][i])//block不存在查看此行中的下一列
				{
					continue;
				}
				//如果向上越界则下移一格
				if (csBlock.GetBlockY() + j < 0)
				{
					++lYMove;
					break;
				}
				else if (csBlock.GetBlockY() + j >= csMap.GetHigh())//否则向下越界上移一格
				{
					--lYMove;
					break;
				}
			}
		}

		bool bCanMove = MoveDetection(lXMove, lYMove);//检测旋转并移动后的位置是否碰撞

		csBlock.Rotate(-lRotate);//还原旋转之前的值

		return bCanMove;
	}

	void ProcessMove(void)
	{
		//溢出时保持正确
		//now=now();
		//if(now-last>=interval)
		//{
		//	last=now;
		//	do-somting;
		//}
		//溢出时出错
		//now=now();
		//if(now>=next)
		//{
		//	next=now+interval;//next溢出后环绕，而now还没溢出，就导致本次循环过快
		//	do-somting;
		//}

		//获取当前时间
		unsigned long ulNowTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();//ms毫秒
		
		if (ulNowTime - ulLastFallTime < ulFallInterval)//还没到时间
		{
			return;
		}

		//设置当前时间为上一次下落时间
		ulLastFallTime = ulNowTime;

		//能下落
		if (MoveDetection(0, 1))
		{
			csBlock.Move(0, 1);
			return;
		}

		//不能下落
		//固化方块
		long lRelativeX = csBlock.GetBlockX() - csMap.GetMapX();//从用户界面坐标计算方块与地图的相对坐标
		long lRelativeY = csBlock.GetBlockY() - csMap.GetMapY();//从用户界面坐标计算方块与地图的相对坐标

		for (long y = 0; y < csBlock.GetSideLength(); ++y)
		{
			for (long x = 0; x < csBlock.GetSideLength(); ++x)
			{
				csMap[lRelativeY + y][lRelativeX + x] |= csBlock[y][x];
			}
		}


		//检测固化位置向下地图csBlock.GetSideLength()行是否需要消除
		for (long i = 0; i < csBlock.GetSideLength(); ++i)
		{
			if (csMap.IsFullLine(lRelativeY + i))//因为删除是从上往下移动，所以从上往下遍历不会被影响
			{
				csMap.RemoveLine(lRelativeY + i);
			}
		}

		//随机新方块，随机方块旋转位置，设置方块出现位置




	}




};