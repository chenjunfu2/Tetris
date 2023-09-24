#pragma once
#include "Draw.hpp"

class Block
{
private:
	//������
	long lBlockX;//���û���������
	long lBlockY;//���û���������
	unsigned long ulRotation;//Rotation���� Rotate����
	unsigned long ulRotationForm;//��ת��̬��
	unsigned long ulSide;//�߳�
	unsigned long ulArea;//���
	const bool *bArrCurBlock;//��ǰ��Ļ���ַ
	const bool *bArrBlock;//���п����ַ
	bool bNew;
public:
	template<size_t Size, size_t Side>
	Block(bool(&bArrBlock)[Size][Side][Side]) :Block((bool *)bArrBlock, Side, Size)//ί�й���
	{}

	Block(bool *_bArrBlock = nullptr, unsigned long _ulSide = 0, unsigned long _ulRotationForm = 0, bool _bNew = true) :
		lBlockX(0), lBlockY(0), ulRotation(0), ulRotationForm(0), ulSide(0), ulArea(0), bArrCurBlock(nullptr), bArrBlock(nullptr), bNew(false)
	{
		SetBlock(_bArrBlock, _ulSide, _ulRotationForm, _bNew);
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
		SetBlock(_Copy.bArrBlock, _Copy.ulSide, _Copy.ulRotationForm, true);
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
		ulRotationForm = _Move.ulRotationForm;
		ulSide = _Move.ulSide;
		ulArea = _Move.ulArea;
		bArrCurBlock = _Move.bArrCurBlock;
		bArrBlock = _Move.bArrBlock;
		bNew = _Move.bNew;
		//�����Ա
		_Move.lBlockX = 0;
		_Move.lBlockY = 0;
		_Move.ulRotation = 0;
		_Move.ulRotationForm = 0;
		_Move.ulSide = 0;
		_Move.ulArea = 0;
		_Move.bArrCurBlock = nullptr;
		_Move.bArrBlock = nullptr;//����nullptr��ֹ����������������������
		_Move.bNew = false;

		return *this;
	}

	void FreeBlock(void)
	{
		if (bArrBlock != nullptr)//��Ϊnullptr������
		{
			lBlockX = 0;
			lBlockY = 0;
			ulRotation = 0;
			ulRotationForm = 0;
			ulSide = 0;
			ulArea = 0;

			if (bNew)//�����new�����Ĳ��ͷţ�����ֱ�ӷ������ڴ��
			{
				delete[] bArrBlock;
			}
			bArrBlock = nullptr;
		}
	}

	void SetBlock(const bool *_bArrBlock, unsigned long _ulSide, unsigned long _ulRotationForm, bool _bNew = true)
	{
		if (_bArrBlock == nullptr)
		{
			return;
		}

		const bool *bArrNew;

		if (_bNew)
		{
			unsigned long ulBlockArrLength = (_ulSide * _ulSide) * _ulRotationForm;//�߳��˱߳���һ�������С���ٳ��Է������������ڴ�
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
		ulArea = _ulSide * _ulSide;
		ulSide = _ulSide;
		ulRotationForm = _ulRotationForm;
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

	unsigned long GetRotCount(void) const
	{
		return ulRotationForm;
	}

	unsigned long GetSide(void) const
	{
		return ulSide;
	}

	unsigned long GetArea(void) const
	{
		return ulArea;
	}

	void Rotate(long lRotate)//���ҷ���
	{
		ulRotation = (ulRotation + lRotate) % ulRotationForm;//�л���ת��̬
		bArrCurBlock = &bArrBlock[ulArea * ulRotation];//��ת����*��������Ӧ���λ���ַ
	}

	void SetRotation(unsigned long _ulRotation)
	{
		ulRotation = _ulRotation % ulRotationForm;
		bArrCurBlock = &bArrBlock[ulArea * ulRotation];//��ת����*��������Ӧ���λ���ַ
	}

	long GetRotation(void) const
	{
		return ulRotation;
	}

	const bool* operator[](long y) const
	{
		return &bArrCurBlock[y * ulSide];//��ǰ���λ���ַ+�߳���С*y�����Ӧ�л���ַ
	}

	void Draw(Draw &csDraw, const char *cBlock = "��") const
	{
		for (long y = 0; y < GetSide(); ++y)
		{
			csDraw.SetCursorPos({GetBlockX() * 2, GetBlockY() + y});//����
			if (GetBlockY() + y < 0)//�ж�Y�Ƿ��ڻ��Ʊ߽���
			{
				continue;
			}

			long x = 0;
			if (GetBlockX() < 0)//�ж�X�Ƿ��ڻ��Ʊ߽���
			{
				x = -GetBlockX();//���������x + GetBlockX() == 0��Ҳ����x = -GetBlockX()
				csDraw.SetCursorPos({0, GetBlockY() + y});//���ó�ʼλ��Ϊx == 0
			}

			for (; x < GetSide(); ++x)
			{
				if (bArrCurBlock[y * GetSide() + x])
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
		unsigned long ulSide;
		unsigned long ulRotationForm;
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

	unsigned long GetBlockCount(void)
	{
		return 7;
	}
};