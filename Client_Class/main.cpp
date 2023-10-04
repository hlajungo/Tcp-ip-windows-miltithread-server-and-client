#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include "ClientClass.h"



int main()
{
	Client c("192.168.1.73", "8080");
	c.ClientInit();
	std::thread serverRecva([&c]() {
		c.serverRecv(c); 
		});
	serverRecva.detach();
	c.clientControlPanel(c);
	
	return 0;
}

