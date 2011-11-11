
#include <nut/gc/gc.hpp>
#include <conio.h>
#include <stdio.h>

using namespace nut;

class TestA
{
	DECLARE_GC_ENABLE
public:
	~TestA()
	{
		printf("~TestA\n");
	}
};


int main()
{
	{
		ref<TestA> a = gc_new<TestA>();
	}
	printf("press any key to continue...");
	getch();
}
