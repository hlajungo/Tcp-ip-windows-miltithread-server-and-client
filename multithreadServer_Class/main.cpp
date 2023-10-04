#define _CRT_SECURE_NO_WARNINGS
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include "MultithreadServerCLass.h"

int main()
{
	Server s("192.168.1.73", "8080");
	s.ServerInit();
	std::thread clientThread([&s]() {
		s.acceptClient(s);
		});
	clientThread.detach();
	s.ControlPanel(s);

	return 0;
}

