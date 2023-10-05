#include <iostream>

#include <WinSock2.h>

#include "a.h"
using namespace std;

void test()
{
	cout << __FILE__ << endl;
	cout << __FUNCTION__ << endl;
	cout << __LINE__ << endl;
}

int main()
{
	//cout << (SOCKET)(~0) << endl;
	bea a;
	a.add();
	a.add();
	a.pri();
	//test();
	//cout << __FUNCTION__ << endl;

	return 0;
}