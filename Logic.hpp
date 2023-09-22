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

	unsigned long ulFallInterval;//������
	unsigned long ulLastFallTime;//��һ�������ʱ��
public:
	Logic(unsigned long _ulFallInterval = 10, long lStartX = 0, long lStartY = 0) :
		ulFallInterval(_ulFallInterval), ulLastFallTime(0)
	{
		csMap.SetMapX(lStartX);
		csMap.SetMapY(lStartY);
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
		long lNewX = csBlock.GetBlockX() + lXMove;
		long lNewY = csBlock.GetBlockY() + lYMove;

		for (long j = 0; j < 4; ++j)
		{
			for (long i = 0; i < 4; ++i)
			{
				if (!csBlock[j][i])//block�����ڣ��鿴�������е���һ��
				{
					continue;
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
		for (long i = 0; i < 4; ++i)
		{
			for (long j = 0; j < 4; ++j)
			{
				if (!csBlock[j][i])//block�����ڲ鿴�����е���һ��
				{
					continue;
				}
				//�������Խ��������һ��
				if (csBlock.GetBlockX() + i < 0)
				{
					++lXMove;
					break;
				}
				else if (csBlock.GetBlockX() + i >= csMap.GetWidth())//��������Խ������һ��
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
				if (!csBlock[j][i])//block�����ڲ鿴�����е���һ��
				{
					continue;
				}
				//�������Խ��������һ��
				if (csBlock.GetBlockY() + j < 0)
				{
					++lYMove;
					break;
				}
				else if (csBlock.GetBlockY() + j >= csMap.GetHigh())//��������Խ������һ��
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

	void ProcessMove(void)
	{
		//���ʱ������ȷ
		//now=now();
		//if(now-last>=interval)
		//{
		//	last=now;
		//	do-somting;
		//}
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
			return;
		}

		//���õ�ǰʱ��Ϊ��һ������ʱ��
		ulLastFallTime = ulNowTime;

		//������
		if (MoveDetection(0, 1))
		{
			csBlock.Move(0, 1);
			return;
		}

		//��������
		//�̻�����
		long lRelativeX = csBlock.GetBlockX() - csMap.GetMapX();//���û�����������㷽�����ͼ���������
		long lRelativeY = csBlock.GetBlockY() - csMap.GetMapY();//���û�����������㷽�����ͼ���������

		for (long y = 0; y < csBlock.GetSideLength(); ++y)
		{
			for (long x = 0; x < csBlock.GetSideLength(); ++x)
			{
				csMap[lRelativeY + y][lRelativeX + x] |= csBlock[y][x];
			}
		}


		//���̻�λ�����µ�ͼcsBlock.GetSideLength()���Ƿ���Ҫ����
		for (long i = 0; i < csBlock.GetSideLength(); ++i)
		{
			if (csMap.IsFullLine(lRelativeY + i))//��Ϊɾ���Ǵ��������ƶ������Դ������±������ᱻӰ��
			{
				csMap.RemoveLine(lRelativeY + i);
			}
		}

		//����·��飬���������תλ�ã����÷������λ��




	}




};