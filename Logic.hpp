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

	unsigned long ulScore;//�÷�
	unsigned long ulFallInterval;//������
	unsigned long ulLastFallTime;//��һ�������ʱ��
private:
	void RandomBlock(Block& csRandomBlock)
	{
		//����·��飬���������תλ�ã����÷������λ��
		DefaultBlock::BlockData stNewBlock = csDefaultBlock[rand() % csDefaultBlock.GetBlockCount()];
		csRandomBlock.SetBlock(stNewBlock.bArrBlock, stNewBlock.ulSide, stNewBlock.ulRotationForm, false);
		csRandomBlock.SetRotation(rand());//SetRotation�Դ���ģ
	}

	void NextBlock(void)//��ȡ��һ�����鲢���õ�ǰ��
	{
		csBlock = std::move(csNextBlock);//�õ�ǰ�������һ������
		//���õ�ǰ��ĳ���λ��
		csBlock.SetBlockX(csMap.GetMapX() + csMap.GetWidth() / 2 - 1);
		csBlock.SetBlockY(csMap.GetMapY() - csBlock.GetSide() + 1);

		RandomBlock(csNextBlock);//��ǰ������һ������
		//������һ����Ļ���λ��
		csNextBlock.SetBlockX(csMap.GetMapX() + csMap.GetWidth() + 1);
		csNextBlock.SetBlockY(csMap.GetMapY());
	}

public:
	Logic(unsigned long _ulFallInterval = 16, long lStartX = 0, long lStartY = 0) :
		ulScore(0), ulFallInterval(_ulFallInterval), ulLastFallTime(0)
	{
		csMap.SetMapX(lStartX);
		csMap.SetMapY(lStartY);
		RandomBlock(csNextBlock);//ˢ����һ����ʼ����
		NextBlock();//��ȡ��һ�����鲢���õ�ǰ��
	}
	~Logic(void) = default;

	static void MoveLeft(void *pLogic)//����
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(-1,0))
		{
			csLogic.csBlock.Move(-1, 0);
		}
	}

	static void MoveRight(void *pLogic)//����
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(1, 0))
		{
			csLogic.csBlock.Move(1, 0);
		}
	}

	static void MoveUp(void *pLogic)//ֱ�ӵ�λ����һ��ֱ���䵽�ף�
	{
		return;//͵������
	}

	static void MoveDown(void *pLogic)//�������䣨��һ������һ��
	{
		Logic &csLogic = *(Logic *)pLogic;
		if (csLogic.MoveDetection(0, 1))
		{
			csLogic.csBlock.Move(0, 1);
		}
	}

	static void RotateLeft(void *pLogic)//����ת
	{
		Logic &csLogic = *(Logic *)pLogic;
		long lXMove, lYMove;
		if (csLogic.RotateDetection(-1, lXMove, lYMove))
		{
			csLogic.csBlock.Rotate(-1);
			csLogic.csBlock.Move(lXMove, lYMove);
		}
	}

	static void RotateRight(void *pLogic)//����ת
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

	bool MoveDetection(long lXMove, long lYMove)//�ƶ��жϣ��ж��Ƿ����ƶ���
	{
		long lNewX = csBlock.GetBlockX() - csMap.GetMapX() + lXMove;//ͨ��������������������
		long lNewY = csBlock.GetBlockY() - csMap.GetMapY() + lYMove;

		for (long j = 0; j < csBlock.GetSide(); ++j)
		{
			for (long i = 0; i < csBlock.GetSide(); ++i)
			{
				if (!csBlock[j][i])//block�����ڣ��鿴�������е���һ��
				{
					continue;
				}

				//��ʼ����Ӷ������µ�������⴦��
				if (lNewY + j < 0)//�������滹��һ�����ڵ�ͼ��û�����ͼ
				{
					if (csMap.IsCross(lNewX + i, 0))//ֻ�ж�x�Ƿ�Խ�磬���Խ����ʧ��
					{
						return false;
					}
					else//ûԽ��ֱ�Ӳ鿴��һ������
					{
						continue;
					}
				}

				//���block���ڲ���(Խ���ûԽ�������mapҲ����)��ô�����ƶ�
				if (csMap.IsCross(lNewX + i, lNewY + j) || 
					csMap[lNewY + j][lNewX + i])//��·��ֵ���ԣ�Cross����²������Map
				{
					return false;
				}
			}
		}

		return true;
	}

	bool RotateDetection(long lRotate, long &lXMove, long &lYMove)//��ת�жϣ��ж��Ƿ�����ת��
	{
		csBlock.Rotate(lRotate);//��ǰ��ת

		//������ת��ѹ�ƶ������ж���ת�󷽿鳬���߽类��ѹ���ͼ����Ҫ���ƶ�����
		lXMove = 0;
		for (long i = 0; i < csBlock.GetSide(); ++i)
		{
			for (long j = 0; j < csBlock.GetSide(); ++j)
			{
				if (!csBlock[j][i])//block�����ڲ鿴�����е���һ��
				{
					continue;
				}
				//�������Խ��������һ��
				if (csBlock.GetBlockX() - csMap.GetMapX() + i < 0)
				{
					++lXMove;
					break;
				}
				else if (csBlock.GetBlockX() - csMap.GetMapX() + i >= csMap.GetWidth())//��������Խ������һ��
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
				if (!csBlock[j][i])//block�����ڲ鿴�����е���һ��
				{
					continue;
				}
				//�������Խ�������
				if (csBlock.GetBlockY() - csMap.GetMapY() + j < 0)
				{
					//++lYMove;
					break;
				}
				else if (csBlock.GetBlockY() - csMap.GetMapY() + j >= csMap.GetHigh())//��������Խ������һ��
				{
					--lYMove;
					break;
				}
			}
		}

		bool bCanMove = MoveDetection(lXMove, lYMove);//�����ת���ƶ����λ���Ƿ���ײ

		csBlock.Rotate(-lRotate);//��ԭ��ת֮ǰ��ֵ

		return bCanMove;
	}

	bool SolidifyBlockAtMap(long lRelativeX, long lRelativeY)//�̻�����
	{
		for (long y = 0; y < csBlock.GetSide(); ++y)
		{
			for (long x = 0; x < csBlock.GetSide(); ++x)
			{
				if (csBlock[y][x] &&//���ڲ����
					csMap.IsCross(lRelativeX + x, lRelativeY + y))
				{
					return false;//�����ͼ���̻�ʧ��
				}

				csMap[lRelativeY + y][lRelativeX + x] |= csBlock[y][x];
			}
		}

		return true;
	}

	bool ProcessMove(void)
	{
		//���ʱ������ȷ
		//now=now();
		//if(now-last>=interval)
		//{
		//	last=now;
		//	do-somting;
		//}
		// 
		//���ʱ����
		//now=now();
		//if(now>=next)
		//{
		//	next=now+interval;//next������ƣ���now��û������͵��±���ѭ������
		//	do-somting;
		//}

		//��ȡ��ǰʱ��
		unsigned long ulNowTime = (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();//ms����
		
		if (ulNowTime - ulLastFallTime < ulFallInterval)//��û��ʱ��
		{
			return true;
		}

		//���õ�ǰʱ��Ϊ��һ������ʱ��
		ulLastFallTime = ulNowTime;

		//������
		if (MoveDetection(0, 1))
		{
			csBlock.Move(0, 1);
			return true;
		}

		//��������
		long lRelativeX = csBlock.GetBlockX() - csMap.GetMapX();//���û�����������㷽�����ͼ���������
		long lRelativeY = csBlock.GetBlockY() - csMap.GetMapY();//���û�����������㷽�����ͼ���������
		if (!SolidifyBlockAtMap(lRelativeX, lRelativeY))//�̻�����
		{
			return false;//��Ϸ����
		}


		//���̻�λ�����µ�ͼcsBlock.GetSideLength()���Ƿ���Ҫ����
		for (long i = 0; i < csBlock.GetSide(); ++i)
		{
			if (csMap.IsFullLine(lRelativeY + i))//��Ϊɾ���Ǵ��������ƶ������Դ������±������ᱻӰ��
			{
				csMap.RemoveLine(lRelativeY + i);
				++ulScore;//�÷�����
			}
		}

		//ˢ�·���
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

	void Draw(Draw &csDraw, const char *cBlock = "��", const char *cBorder = "��")
	{
		//���Ƶ÷�
		char cScore[32];
		int iScoreStrLen = sprintf_s(cScore, "�� ��: %ld", ulScore);
		csDraw.SetCursorPos({csMap.GetMapX() + (csMap.GetWidth() + 1) * 2 + 1, 0});
		csDraw.WriteBuffer(cScore, iScoreStrLen);

		csBlock.Draw(csDraw, cBlock);
		csNextBlock.Draw(csDraw, cBlock);
		csMap.Draw(csDraw, cBlock, cBorder);
	}
};