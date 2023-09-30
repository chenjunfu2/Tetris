#pragma once

#include "Map.hpp"
#include "Block.hpp"
#include "Input.hpp"

#include <chrono>
#include <stdlib.h>

class Logic
{
private:
	Map csMap;
	Block csBlock;
	Block csNextBlock;
	DefaultBlock csDefaultBlock;

	unsigned long ulScore;//得分
	unsigned long ulFallInterval;//下落间隔
	unsigned long ulLastFallTime;//上一次下落的时间
private:
	void RandomBlock(Block& csRandomBlock)
	{
		//随机新方块，随机方块旋转位置，设置方块出现位置
		DefaultBlock::BlockData stNewBlock = csDefaultBlock[rand() % csDefaultBlock.GetBlockCount()];
		csRandomBlock.SetBlock(stNewBlock.bArrBlock, stNewBlock.ulSide, stNewBlock.ulRotationForm, false);
		csRandomBlock.SetRotation(rand());//SetRotation自带求模
	}

	void NextBlock(void)//获取下一个方块并设置当前块
	{
		csBlock = std::move(csNextBlock);//让当前块等于下一个方块
		//设置当前块的出现位置
		csBlock.SetBlockX(csMap.GetMapX() + csMap.GetWidth() / 2 - 1);
		csBlock.SetBlockY(csMap.GetMapY() - csBlock.GetSide() + 1);

		RandomBlock(csNextBlock);//提前生成下一个方块
		//设置下一个块的绘制位置
		csNextBlock.SetBlockX(csMap.GetMapX() + csMap.GetWidth() + 1);
		csNextBlock.SetBlockY(csMap.GetMapY());
	}

public:
	Logic(unsigned long _ulFallInterval = 16, long lStartX = 0, long lStartY = 0) :
		ulScore(0), ulFallInterval(_ulFallInterval), ulLastFallTime(0)
	{
		csMap.SetMapX(lStartX);
		csMap.SetMapY(lStartY);
		RandomBlock(csNextBlock);//刷出第一个初始方块
		NextBlock();//获取下一个方块并设置当前块
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
		long lNewX = csBlock.GetBlockX() - csMap.GetMapX() + lXMove;//通过绝对坐标算出相对坐标
		long lNewY = csBlock.GetBlockY() - csMap.GetMapY() + lYMove;

		for (long j = 0; j < csBlock.GetSide(); ++j)
		{
			for (long i = 0; i < csBlock.GetSide(); ++i)
			{
				if (!csBlock[j][i])//block不存在，查看此列行中的下一列
				{
					continue;
				}

				//初始方块从顶上落下的情况特殊处理
				if (lNewY + j < 0)//方块上面还有一部分在地图外没落入地图
				{
					if (csMap.IsCross(lNewX + i, 0))//只判断x是否越界，如果越界则失败
					{
						return false;
					}
					else//没越界直接查看下一个格子
					{
						continue;
					}
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
		for (long i = 0; i < csBlock.GetSide(); ++i)
		{
			for (long j = 0; j < csBlock.GetSide(); ++j)
			{
				if (!csBlock[j][i])//block不存在查看此列中的下一行
				{
					continue;
				}
				//如果向左越界则右移一格
				if (csBlock.GetBlockX() - csMap.GetMapX() + i < 0)
				{
					++lXMove;
					break;
				}
				else if (csBlock.GetBlockX() - csMap.GetMapX() + i >= csMap.GetWidth())//否则向右越界左移一格
				{
					--lXMove;
					break;
				}
			}
		}

		lYMove = 0;
		for (long j = 0; j < csBlock.GetSide(); ++j)
		{
			for (long i = 0; i < csBlock.GetSide(); ++i)
			{
				if (!csBlock[j][i])//block不存在查看此行中的下一列
				{
					continue;
				}
				//如果向上越界则忽略
				if (csBlock.GetBlockY() - csMap.GetMapY() + j < 0)
				{
					//++lYMove;
					break;
				}
				else if (csBlock.GetBlockY() - csMap.GetMapY() + j >= csMap.GetHigh())//否则向下越界上移一格
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

	bool SolidifyBlockAtMap(long lRelativeX, long lRelativeY)//固化方块
	{
		for (long y = 0; y < csBlock.GetSide(); ++y)
		{
			for (long x = 0; x < csBlock.GetSide(); ++x)
			{
				if (csBlock[y][x] &&//存在并溢出
					csMap.IsCross(lRelativeX + x, lRelativeY + y))
				{
					return false;//溢出地图，固化失败
				}

				csMap[lRelativeY + y][lRelativeX + x] |= csBlock[y][x];
			}
		}

		return true;
	}

	bool ProcessMove(void)
	{
		//溢出时保持正确
		//now=now();
		//if(now-last>=interval)
		//{
		//	last=now;
		//	do-somting;
		//}
		// 
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
			return true;
		}

		//设置当前时间为上一次下落时间
		ulLastFallTime = ulNowTime;

		//能下落
		if (MoveDetection(0, 1))
		{
			csBlock.Move(0, 1);
			return true;
		}

		//不能下落
		long lRelativeX = csBlock.GetBlockX() - csMap.GetMapX();//从用户界面坐标计算方块与地图的相对坐标
		long lRelativeY = csBlock.GetBlockY() - csMap.GetMapY();//从用户界面坐标计算方块与地图的相对坐标
		if (!SolidifyBlockAtMap(lRelativeX, lRelativeY))//固化方块
		{
			return false;//游戏结束
		}


		//检测固化位置向下地图csBlock.GetSideLength()行是否需要消除
		for (long i = 0; i < csBlock.GetSide(); ++i)
		{
			if (csMap.IsFullLine(lRelativeY + i))//因为删除是从上往下移动，所以从上往下遍历不会被影响
			{
				csMap.RemoveLine(lRelativeY + i);
				++ulScore;//得分增加
			}
		}

		//刷新方块
		NextBlock();

		return true;
	}

	const Map &GetMap(void)
	{
		return csMap;
	}

	const Block &GetBlock(void)
	{
		return csBlock;
	}

	void Draw(Draw &csDraw, const char *cBlock = "□", const char *cBorder = "■")
	{
		//绘制得分
		char cScore[32];
		int iScoreStrLen = sprintf_s(cScore, "分 数: %ld", ulScore);
		csDraw.SetCursorPos({csMap.GetMapX() + (csMap.GetWidth() + 1) * 2 + 1, 0});
		csDraw.WriteBuffer(cScore, iScoreStrLen);

		csBlock.Draw(csDraw, cBlock);
		csNextBlock.Draw(csDraw, cBlock);
		csMap.Draw(csDraw, cBlock, cBorder);
	}
};