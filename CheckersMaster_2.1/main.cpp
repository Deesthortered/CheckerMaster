#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <string>
#include <WinSock2.h>
#include "GameParameters.h"
#pragma comment(lib, "WS2_32.lib")
using namespace sf;

WSADATA wsData;
SOCKET listen_socket;
SOCKET work_socket;

sockaddr_in name;
hostent* hn;
sockaddr_in adr;
sockaddr_in new_ca;


void StartThread(GameParameters &g)
{
	g.ThreadFunc();
}
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	RenderWindow window(VideoMode(1200, 700), "CheckerMaster v2.0");
	GameParameters game = GameParameters();
	WSAStartup(MAKEWORD(1, 1), &wsData);
	std::thread SecondThread(StartThread, std::ref(game));
	while (window.isOpen())
	{
		Event eve;
		while (window.pollEvent(eve))
		{
			if (eve.type == Event::Closed)
				window.close();
		}
		game.AllUpdate(eve);
		window.clear();
		game.AllDraw(window);
		window.display();
	}
	if (SecondThread.joinable()) SecondThread.join();
	WSACleanup();
	return 0;
}