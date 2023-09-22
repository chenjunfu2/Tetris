#include "Block.hpp"
#include "Input.hpp"

int main(void)
{
	bool test[4][3][3] =
	{
		0,0,0,
		1,1,1,
		0,1,0,
	};

	for (long i = 1; i < 4; ++i)
	{
		RotateArr(test[0], test[i], i);
	}
	
	//俄罗斯方块对象
	Block tb((bool *)test, 3, 4, false);
	//多缓冲绘制
	DoubleBufferDraw dbd;
	//按键触发
	Input ipt;


	auto leftRota = [](void *p)->void
	{
		((Block*)p)->Rotate(-1);//旋转
	};

	auto rightRota = [](void *p)->void
	{
		((Block *)p)->Rotate(1);//旋转
	};

	ipt.RegisterKey('q', {leftRota,&tb});
	ipt.RegisterKey('Q', {leftRota,&tb});
	ipt.RegisterKey('e', {rightRota,&tb});
	ipt.RegisterKey('E', {rightRota,&tb});

	while(true)
	{
		dbd.BegPrint();//绘图区域开始
		Draw &d = dbd.GetDraw();
		{
			d.SetCursorShow(false);
			tb.Draw(d);
		}
		dbd.EndPrint();//绘图区域结束
	
		ipt.ProcessKey();
		Sleep(0);
	}

	return 0;
}