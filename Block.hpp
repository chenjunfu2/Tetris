#pragma once
#include "Draw.hpp"

class Block
{
private:
	//块数据
	long lBlockX;//在用户界面坐标
	long lBlockY;//在用户界面坐标
	unsigned long ulRotation;//Rotation属性 Rotate动词
	unsigned long ulRotationForm;//旋转形态数
	unsigned long ulSide;//边长
	unsigned long ulArea;//面积
	const bool *bArrCurBlock;//当前块的基地址
	const bool *bArrBlock;//所有块基地址
	bool bNew;
public:
	template<size_t Size, size_t Side>
	Block(bool(&bArrBlock)[Size][Side][Side]) :Block((bool *)bArrBlock, Side, Size)//委托构造
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
		//进行赋值和清理（如果bArrBlock不为nullptr）
		SetBlock(_Copy.bArrBlock, _Copy.ulSide, _Copy.ulRotationForm, true);
		//拷贝剩余成员
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
		//清理成员
		_Move.lBlockX = 0;
		_Move.lBlockY = 0;
		_Move.ulRotation = 0;
		_Move.ulRotationForm = 0;
		_Move.ulSide = 0;
		_Move.ulArea = 0;
		_Move.bArrCurBlock = nullptr;
		_Move.bArrBlock = nullptr;//设置nullptr防止被后续析构函数调用清理
		_Move.bNew = false;

		return *this;
	}

	void FreeBlock(void)
	{
		if (bArrBlock != nullptr)//不为nullptr则清理
		{
			lBlockX = 0;
			lBlockY = 0;
			ulRotation = 0;
			ulRotationForm = 0;
			ulSide = 0;
			ulArea = 0;

			if (bNew)//如果是new出来的才释放，否则直接放弃该内存块
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
			unsigned long ulBlockArrLength = (_ulSide * _ulSide) * _ulRotationForm;//边长乘边长是一个方块大小，再乘以方块个数算出总内存
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

		//释放之前的内存（如果有）
		FreeBlock();
		//成员赋值
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

	void Rotate(long lRotate)//正右反左
	{
		ulRotation = (ulRotation + lRotate) % ulRotationForm;//切换旋转形态
		bArrCurBlock = &bArrBlock[ulArea * ulRotation];//旋转方向*面积求出对应矩形基地址
	}

	void SetRotation(unsigned long _ulRotation)
	{
		ulRotation = _ulRotation % ulRotationForm;
		bArrCurBlock = &bArrBlock[ulArea * ulRotation];//旋转方向*面积求出对应矩形基地址
	}

	long GetRotation(void) const
	{
		return ulRotation;
	}

	const bool* operator[](long y) const
	{
		return &bArrCurBlock[y * ulSide];//当前矩形基地址+边长大小*y求出对应行基地址
	}

	void Draw(Draw &csDraw, const char *cBlock = "□") const
	{
		for (long y = 0; y < GetSide(); ++y)
		{
			csDraw.SetCursorPos({GetBlockX() * 2, GetBlockY() + y});//换行
			if (GetBlockY() + y < 0)//判断Y是否在绘制边界外
			{
				continue;
			}

			long x = 0;
			if (GetBlockX() < 0)//判断X是否在绘制边界外
			{
				x = -GetBlockX();//如果是则让x + GetBlockX() == 0，也就是x = -GetBlockX()
				csDraw.SetCursorPos({0, GetBlockY() + y});//设置初始位置为x == 0
			}

			for (; x < GetSide(); ++x)
			{
				if (bArrCurBlock[y * GetSide() + x])
				{
					csDraw.WriteBuffer(cBlock, 2);//画方块
				}
				else
				{
					csDraw.MoveCursorPos({2, 0});//跳空白
				}
			}
		}
	}
};


template <typename T>
void RotateArr(const T *tArrIn, T *tArrOut, long lArrSide, long lRotation)
{
	unsigned long ulRotation = (unsigned long)lRotation % 4;//4->4个旋转方向，每个旋转是90度

	for (long y = 0; y < lArrSide; ++y)
	{
		for (long x = 0; x < lArrSide; ++x)
		{
			switch (ulRotation)
			{
			case 0://直接拷贝
				tArrOut[y * lArrSide + x] = tArrIn[y * lArrSide + x];
				break;
			case 1://右转90
				tArrOut[y * lArrSide + x] = tArrIn[(lArrSide - 1 - x) * lArrSide + y];
				break;
			case 2://翻转180
				tArrOut[y * lArrSide + x] = tArrIn[(lArrSide - 1 - y) * lArrSide + (lArrSide - 1 - x)];
				break;
			case 3://左转90
				tArrOut[y * lArrSide + x] = tArrIn[x * lArrSide + (lArrSide - 1 - y)];
				break;
			default:
				return;//意外结果
				break;
			}
		}
	}
}


//对二维数组进行旋转
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
	bool bBlockO[1][2][2] =//1种旋转形态，大小2*2
	{
		1,1,
		1,1,
	};

	bool bBlockI[2][4][4] =//2种旋转形态，大小4*4
	{
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
		0,0,1,0,
	};

	bool bBlockS[2][3][3] =//2种旋转形态，大小3*3
	{
		0,0,0,
		0,1,1,
		1,1,0,
	};

	bool bBlockZ[2][3][3] =//2种旋转形态，大小3*3
	{
		0,0,0,
		1,1,0,
		0,1,1,
	};

	bool bBlockL[4][3][3] =//4种旋转形态，大小3*3
	{
		0,1,0,
		0,1,0,
		0,1,1,
	};

	bool bBlockJ[4][3][3] =//4种旋转形态，大小3*3
	{
		0,1,0,
		0,1,0,
		1,1,0,
	};

	bool bBlockT[4][3][3] =//4种旋转形态 大小3*3
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