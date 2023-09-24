#include "Block.hpp"
#include "Input.hpp"

int mainj(void)
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
	//按键输入
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
		{
			Draw &d = dbd.GetDraw();
			d.SetCursorShow(false);
			tb.Draw(d);
		}
		dbd.EndPrint();//绘图区域结束
	
		ipt.ProcessKey();
		Sleep(0);
	}

	return 0;
}


#include "Logic.hpp"
#include <time.h>

int main(void)
{
	srand((unsigned)time(NULL));//固定种子方便调试
	//游戏主逻辑
	Logic lg(16 * 20, 1, 1);
	//多缓冲绘制
	DoubleBufferDraw dbd;
	//按键输入
	Input ipt;

	//注册函数
	ipt.RegisterKey('Q', {Logic::RotateLeft,&lg});
	ipt.RegisterKey('q', {Logic::RotateLeft,&lg});

	ipt.RegisterKey('E', {Logic::RotateRight,&lg});
	ipt.RegisterKey('e', {Logic::RotateRight,&lg});

	ipt.RegisterKey('W', {Logic::MoveUp,&lg});
	ipt.RegisterKey('w', {Logic::MoveUp,&lg});

	ipt.RegisterKey('A', {Logic::MoveLeft,&lg});
	ipt.RegisterKey('a', {Logic::MoveLeft,&lg});

	ipt.RegisterKey('S', {Logic::MoveDown,&lg});
	ipt.RegisterKey('s', {Logic::MoveDown,&lg});

	ipt.RegisterKey('D', {Logic::MoveRight,&lg});
	ipt.RegisterKey('d', {Logic::MoveRight,&lg});

	ipt.RegisterKey('P', {Logic::Pause,&lg});
	ipt.RegisterKey('p', {Logic::Pause,&lg});

	do
	{
		//Sleep(0);
		dbd.BegPrint();//绘图区域开始
		{
			Draw &d = dbd.GetDraw();
			d.SetCursorShow(false);
			lg.Draw(d);
		}
		dbd.EndPrint();//绘图区域结束

		ipt.ProcessKey();//获取用户操作
	} while (lg.ProcessMove());//自动下落


	while (true)
	{
		dbd.BegPrint();
		dbd.GetDraw().SetCursorPos({0,0});
		dbd.GetDraw().SetCursorShow(false);
		dbd.GetDraw().WriteBuffer("你失败了！\n", sizeof("你失败了！\n") - 1);
		dbd.EndPrint();

		system("pause");
	}
	
	return 0;
}