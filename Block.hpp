#pragma once
#include "Draw.hpp"

class Block
{
private:
	//������
	long lBlockX;//���û���������
	long lBlockY;//���û���������
	unsigned long ulRotation;//Rotation���� Rotate����
	unsigned long ulRotCount;//��ת��̬��
	unsigned long ulSideLength;//�߳�
	unsigned long ulSquareArea;//���
	const bool *bArrCurBlock;//��ǰ��Ļ���ַ
	const bool *bArrBlock;//���п����ַ
	bool bNew;
public:
	template<size_t Size, size_t Side>
	Block(bool(&bArrBlock)[Size][Side][Side]) :Block((bool *)bArrBlock, Side, Size)//ί�й���
	{}

	Block(bool *_bArrBlock = nullptr, unsigned long _ulSideLength = 0, unsigned long _ulRotCount = 0, bool _bNew = true) :
		lBlockX(0), lBlockY(0), ulRotation(0), ulRotCount(0), ulSideLength(0), ulSquareArea(0), bArrCurBlock(nullptr), bArrBlock(nullptr), bNew(false)
	{
		SetBlock(_bArrBlock, _ulSideLength, _ulRotCount, _bNew);
	}

	Block(const Block &_Copy)
	{
		operator=(_Copy);
	}

	Block(Block &&_Move) noexcept
	{
		operator=((Block &&)_Move);
	}
	
	~Block(void)
	{
		FreeBlock();
	}


	Block &operator=(const Block &_Copy)
	{
		//���и�ֵ���������bArrBlock��Ϊnullptr��
		SetBlock(_Copy.bArrBlock, _Copy.ulSideLength, _Copy.ulRotCount, true);
		//����ʣ���Ա
		lBlockX = _Copy.lBlockX;
		lBlockY = _Copy.lBlockY;
		ulRotation = _Copy.ulRotation;
	}

	Block &operator=(Block &&_Move) noexcept
	{
		lBlockX = _Move.lBlockX;
		lBlockY = _Move.lBlockY;
		ulRotation = _Move.ulRotation;
		ulRotCount = _Move.ulRotCount;
		ulSideLength = _Move.ulSideLength;
		ulSquareArea = _Move.ulSquareArea;
		bArrCurBlock = _Move.bArrCurBlock;
		bArrBlock = _Move.bArrBlock;
		bNew = _Move.bNew;
		//�����Ա
		_Move.lBlockX = 0;
		_Move.lBlockY = 0;
		_Move.ulRotation = 0;
		_Move.ulRotCount = 0;
		_Move.ulSideLength = 0;
		_Move.ulSquareArea = 0;
		_Move.bArrCurBlock = nullptr;
		_Move.bArrBlock = nullptr;//����nullptr��ֹ����������������������
		_Move.bNew = false;
	}

	void FreeBlock(void)
	{
		if (bArrBlock != nullptr)//��Ϊnullptr������
		{
			lBlockX = 0;
			lBlockY = 0;
			ulRotation = 0;
			ulRotCount = 0;
			ulSideLength = 0;
			ulSquareArea = 0;

			if (bNew)//�����new�����Ĳ��ͷţ�����ֱ�ӷ������ڴ��
			{
				delete[] bArrBlock;
			}
			bArrBlock = nullptr;
		}
	}

	void SetBlock(const bool *_bArrBlock, unsigned long _ulSideLength, unsigned long _ulRotCount, bool _bNew = true)
	{
		if (_bArrBlock == nullptr)
		{
			return;
		}

		const bool *bArrNew;

		if (_bNew)
		{
			unsigned long ulBlockArrLength = (_ulSideLength * _ulSideLength) * _ulRotCount;//�߳��˱߳���һ�������С���ٳ��Է������������ڴ�
			if (ulBlockArrLength == 0)
			{
				return;
			}
			bArrNew = new bool[ulBlockArrLength];
			memcpy((void *)bArrNew, _bArrBlock, ulBlockArrLength * sizeof(bool));
		}
		else
		{
			bArrNew = _bArrBlock;
		}

		//�ͷ�֮ǰ���ڴ棨����У�
		FreeBlock();
		//��Ա��ֵ
		bArrBlock = bArrNew;
		bArrCurBlock = bArrNew;
		ulSquareArea = _ulSideLength * _ulSideLength;
		ulSideLength = _ulSideLength;
		ulRotCount = _ulRotCount;
		bNew = _bNew;
	}

	void Move(long lMoveX, long lMoveY)
	{
		lBlockX += lMoveX;
		lBlockY += lMoveY;
	}

	void SetBlockX(long _lBlockX)
	{
		lBlockX = _lBlockX;
	}

	void SetBlockY(long _lBlockY)
	{
		lBlockY = _lBlockY;
	}

	long GetBlockX(void) const
	{
		return lBlockX;
	}

	long GetBlockY(void) const
	{
		return lBlockY;
	}

	unsigned long GetRotCount(void)
	{
		return ulRotCount;
	}

	unsigned long GetSideLength(void)
	{
		return ulSideLength;
	}

	unsigned long GetSquareArea(void)
	{
		return ulSquareArea;
	}

	void Rotate(long lRotate)//���ҷ���
	{
		ulRotation = (ulRotation + lRotate) % ulRotCount;//�л���ת��̬
		bArrCurBlock = &bArrBlock[ulSquareArea * ulRotation];//��ת����*��������Ӧ���λ���ַ
	}

	void SetRotation(unsigned long _ulRotation)
	{
		ulRotation = _ulRotation % ulRotCount;
		bArrCurBlock = &bArrBlock[ulSquareArea * ulRotation];//��ת����*��������Ӧ���λ���ַ
	}

	long GetRotation(void) const
	{
		return ulRotation;
	}

	const bool* operator[](long y) const
	{
		return &bArrCurBlock[y * ulSideLength];//��ǰ���λ���ַ+�߳���С*y�����Ӧ�л���ַ
	}

	void Draw(Draw &csDraw, const char *cBlock = "��") const
	{
		for (long y = 0; y < ulSideLength; ++y)
		{
			csDraw.SetCursorPos({lBlockX, lBlockY + y});//����
			for (long x = 0; x < ulSideLength; ++x)
			{
				if (bArrCurBlock[y * ulSideLength + x])
				{
					csDraw.WriteBuffer(cBlock, 2);//������
				}
				else
				{
					csDraw.MoveCursorPos({2, 0});//���հ�
				}
			}
		}
	}
};


template <typename T>
void RotateArr(const T *tArrIn, T *tArrOut, long lArrSide, long lRotation)
{
	unsigned long ulRotation = (unsigned long)lRotation % 4;//4->4����ת����ÿ����ת��90��

	for (long y = 0; y < lArrSide; ++y)
	{
		for (long x = 0; x < lArrSide; ++x)
		{
			switch (ulRotation)
			{
			case 0://ֱ�ӿ���
				tArrOut[y * lArrSide + x] = tArrIn[y * lArrSide + x];
				break;
			case 1://��ת90
				tArrOut[y * lArrSide + x] = tArrIn[(lArrSide - 1 - x) * lArrSide + y];
				break;
			case 2://��ת180
				tArrOut[y * lArrSide + x] = tArrIn[(lArrSide - 1 - y) * lArrSide + (lArrSide - 1 - x)];
				break;
			case 3://��ת90
				tArrOut[y * lArrSide + x] = tArrIn[x * lArrSide + (lArrSide - 1 - y)];
				break;
			default:
				return;//������
				break;
			}
		}
	}
}


//�Զ�ά���������ת
template <typename T, size_t Side>
void RotateArr(const T(&ArrIn)[Side][Side], T(&ArrOut)[Side][Side], long lRotation)
{
	RotateArr<T>((const T *)ArrIn, (T *)ArrOut, Side, lRotation);
}


class DefaultBlock
{
public:
	struct BlockData
	{
		bool *bArrBlock;
		unsigned long ulSideLength;
		unsigned long ulRotCount;
	};

private:
	bool bBlockO[1][2][2] =//1����ת��̬����С2*2
	{
		1,1,
		1,1,
	};

	bool bBlockI[2][4][4] =//2����ת��̬����С4*4
	{
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
	};

	bool bBlockS[2][3][3] =//2����ת��̬����С3*3
	{
		0,0,0,
		0,1,1,
		1,1,0,
	};

	bool bBlockZ[2][3][3] =//2����ת��̬����С3*3
	{
		0,0,0,
		1,1,0,
		0,1,1,
	};

	bool bBlockL[4][3][3] =//4����ת��̬����С3*3
	{
		0,1,0,
		0,1,0,
		0,1,1,
	};

	bool bBlockJ[4][3][3] =//4����ת��̬����С3*3
	{
		0,1,0,
		0,1,0,
		1,1,0,
	};

	bool bBlockT[4][3][3] =//4����ת��̬ ��С3*3
	{
		0,0,0,
		1,1,1,
		0,1,0,
	};

	BlockData stBlockData[7];

private:
	template<typename T, size_t Size, size_t Side>
	void RotateArrAll(T(&Arr)[Size][Side][Side], unsigned long ulBlockDataIndex)
	{
		for (long i = 1; i < Size; ++i)
		{
			RotateArr(Arr[0], Arr[i], i);
		}

		stBlockData[ulBlockDataIndex] =
		{
			(bool *)Arr,
			Side,
			Size,
		};
	}
public:
	DefaultBlock(void)
	{
		RotateArrAll(bBlockO,0);
		RotateArrAll(bBlockI,1);
		RotateArrAll(bBlockS,2);
		RotateArrAll(bBlockZ,3);
		RotateArrAll(bBlockL,4);
		RotateArrAll(bBlockJ,5);
		RotateArrAll(bBlockT,6);
	}
	~DefaultBlock(void) = default;

	const BlockData &operator[](unsigned long ulBlockDataIndex)
	{
		return stBlockData[ulBlockDataIndex];
	}
};