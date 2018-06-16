#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "GameParameters.h"
#include <WinSock2.h>
using namespace sf;

bool GameParameters::invert_desk;
unsigned long long GameParameters::operations;

#define INF 1000000
#define MAX_PACKET_SIZE 30
char buff[MAX_PACKET_SIZE]; int len = 0;

extern WSADATA wsData; //ok
extern SOCKET listen_socket;
extern SOCKET work_socket;

extern sockaddr_in name; // ls
extern hostent* hn; // ls
extern sockaddr_in adr; // ls + ws
extern sockaddr_in new_ca; // ws

bool GetBit(bit_type field, int number)
{
	register bit_type bit = (number == 0 ? 1 : 2);
	for (register char i = 1; i < number; i++) bit = bit << 1;
	register bit_type res = (field & bit);
	if (res == bit) return true;
	return false;
}
bit_type SetBit(bit_type field, int number, bool value)
{
	register bit_type bit = (number == 0 ? 1 : 2);
	for (register char i = 1; i < number; i++) bit = bit << 1;
	if (value)
	{
		field = field | bit;
	}
	else
	{
		bit = ~bit;
		field = field & bit;
	}
	return field;
}

inline void AddMove(MoveList * &list, Move move)
{
	list = new MoveList(move, list);
}
void DeleteMoveList(MoveList * &list)
{
	while (list != nullptr)
	{
		MoveList * p = list;
		list = list->next;
		delete p;
	}
}

////// Public functions
GameParameters::GameParameters()
{
	//Content
	tex_main_menu.loadFromFile("content/images/mainmenu.png");
	tex_multiplayer.loadFromFile("content/images/multiplayer.png");
	tex_mlt_one.loadFromFile("content/images/mlt_one.png");
	tex_mlt_tcp.loadFromFile("content/images/mlt_tcp.png");
	tex_singleplayer.loadFromFile("content/images/singleplayer.png");
	tex_galka.loadFromFile("content/images/galka.png");
	tex_snake.loadFromFile("content/images/snake.png");

	tex_white_shape.loadFromFile("content/images/white_shape.png");
	tex_white_queen.loadFromFile("content/images/white_queen.png");
	tex_black_shape.loadFromFile("content/images/black_shape.png");
	tex_black_queen.loadFromFile("content/images/black_queen.png");

	tex_gameground.loadFromFile("content/images/gameground.png");
	tex_selected.loadFromFile("content/images/selected.png");
	tex_light.loadFromFile("content/images/light.png");

	font.loadFromFile("content/font.TTF");

	b_Game = true;
	focus = true;
	gameState = st_Main_Menu;

	sp_main_menu.setTexture(tex_main_menu);
	sp_multiplayer.setTexture(tex_multiplayer);
	sp_mlt_one.setTexture(tex_mlt_one);
	sp_mlt_tcp.setTexture(tex_mlt_tcp);
	sp_singleplayer.setTexture(tex_singleplayer);
	sp_galka.setTexture(tex_galka);
	sp_snake.setTexture(tex_snake);

	sp_gameground.setTexture(tex_gameground);
	sp_selected.setTexture(tex_selected);

	sp_galka.setPosition(Vector2f(522, 211));
	sp_snake.setPosition(Vector2f(169, 221));

	tx_diff.setFont(font);
	tx_diff.setFillColor(Color::Black);
	tx_diff.setPosition(Vector2f(370, 144));
	tx_diff.setString("6");

	tx_name1.setFont(font);
	tx_name1.setFillColor(Color::Black);
	tx_name1.setPosition(Vector2f(180, 287));
	tx_name1.setStyle(Text::Italic);
	tx_name1.setString("");

	tx_name2.setFont(font);
	tx_name2.setFillColor(Color::Black);
	tx_name2.setPosition(Vector2f(180, 380));
	tx_name2.setStyle(Text::Italic);
	tx_name2.setString("");

	tx_status.setFont(font);
	tx_status.setFillColor(Color::Black);
	tx_status.setPosition(Vector2f(781, 329));
	tx_status.setString("");

	tx_name1ingame.setFont(font);
	tx_name1ingame.setFillColor(Color::Black);
	tx_name1ingame.setPosition(Vector2f(814, 198));
	tx_name1ingame.setString("");

	tx_name2ingame.setFont(font);
	tx_name2ingame.setFillColor(Color::Black);
	tx_name2ingame.setPosition(Vector2f(814, 238));
	tx_name2ingame.setString("");

	name1 = "";
	name2 = "";
	ready = false;
	input = false;
	once = true;

	difficult = 6;
	player_color = true;
	winer = true;

	main_desk = Desk();
	selected = false;
	turn = t_Player1;
	memset(light, 0, 32);
	selected_cell = -1;
	player_list = nullptr;
	must_beat = false;
	game_over = false;
	invert_desk = false;

	thread_state = Waiting;
	thread_mode = AI;

	// TCP
	sp_mlt_tcp_ready.setTexture(tex_galka);
	sp_mlt_tcp_ready.setPosition(Vector2f(1081, 287));

	server = true;
	sp_galka_server_client.setTexture(tex_galka);
	sp_galka_server_client.setPosition(Vector2f(32, 181));

	tex_mlt_tcp_server_bind.loadFromFile("content/images/mlt_tcp_server_bind.png");
	sp_mlt_tcp_server_bind.setTexture(tex_mlt_tcp_server_bind);
	sp_mlt_tcp_server_bind.setPosition(Vector2f(388, 164));
	server_auto_bind = true;
	sp_galka_tcp_bind.setTexture(tex_galka);
	sp_galka_tcp_bind.setPosition(Vector2f(388 + 297, 164 + 7));

	tx_IP.setFont(font);
	tx_Port.setFont(font);
	tx_IP.setFillColor(Color::Black);
	tx_Port.setFillColor(Color::Black);
	tx_IP.setCharacterSize(48);
	tx_Port.setCharacterSize(48);
	tx_IP.setPosition(Vector2f(459, 272));
	tx_Port.setPosition(Vector2f(505, 335));
	tx_name.setFont(font);
	tx_name.setFillColor(Color::Black);
	tx_name.setCharacterSize(48);
	tx_name.setPosition(Vector2f(525, 390));
	tx_IP.setString("auto");
	tx_Port.setString("auto");
	tx_name.setString("");
	ip = "";
	port = "";

	tex_mlt_tcp_color.loadFromFile("content/images/mlt_tcp_color.png");
	sp_mlt_tcp_color.setTexture(tex_mlt_tcp_color);
	sp_mlt_tcp_color.setPosition(Vector2f(27, 446));
	sp_galka_mlt_tcp_color.setTexture(tex_galka);
	sp_galka_mlt_tcp_color.setPosition(Vector2f(27 + 107, 446 + 55));

	input_ip = false;
	input_port = false;
	input_name = true;

	tx_start_button.setFont(font);
	tx_start_button.setFillColor(Color::Black);
	tx_start_button.setCharacterSize(70);
	tx_start_button.setPosition(Vector2f(550, 510));
	tx_start_button.setString("    Create room");
	go = false;
}
void GameParameters::AllUpdate(Event &eve)
{
	if (eve.type == Event::GainedFocus) focus = true;
	else if (eve.type == Event::LostFocus) focus = false;

	if (eve.type == Event::Closed) { thread_state = Terminate; return; }
	switch (gameState)
	{
	case st_Main_Menu: { UpdateMainMenu(); } break;
	case st_Multiplayer: { UpdateMultiplayer(); } break;
	case st_Mlt_One: { UpdateMltOne(eve); } break;
	case st_Mlt_TCP: { UpdateMltTcp(eve); } break;
	case st_Single_Player: { UpdateSinglePlayer(); } break;
	case st_Game_Ai: { UpdateGameAI(); } break;
	case st_Game_One: { UpdateGameOne(); } break;
	case st_Game_TCP: { UpdateGameTcp(); } break;
	}
}
void GameParameters::AllDraw(RenderWindow &window)
{
	if (!b_Game) { window.close(); thread_state = Terminate; return; }
	Vector2f real_mouse_position_in_game = window.mapPixelToCoords(Vector2i(mouse.getPosition(window)));
	MouseX = int(real_mouse_position_in_game.x);
	MouseY = int(real_mouse_position_in_game.y);
	switch (gameState)
	{
	case st_Main_Menu: { DrawMainMenu(window); } break;
	case st_Multiplayer: { DrawMultiplayer(window); } break;
	case st_Mlt_One: { DrawMltOne(window); } break;
	case st_Mlt_TCP: { DrawMltTcp(window); } break;
	case st_Single_Player: { DrawSinglePlayer(window); } break;
	default: { DrawGame(window); }
	}
}

////// Functions ///
void GameParameters::DrawMainMenu(RenderWindow &window)
{
	window.draw(sp_main_menu);
}
void GameParameters::DrawMultiplayer(RenderWindow &window)
{
	window.draw(sp_multiplayer);
}
void GameParameters::DrawMltOne(RenderWindow &window)
{
	window.draw(sp_mlt_one);
	window.draw(tx_name1);
	window.draw(tx_name2);
}
void GameParameters::DrawMltTcp(RenderWindow &window)
{
	window.draw(sp_mlt_tcp);
	window.draw(sp_galka_server_client);
	if (server)
	{
		window.draw(sp_mlt_tcp_server_bind);
		window.draw(sp_galka_tcp_bind);
		window.draw(sp_mlt_tcp_color);
		window.draw(sp_galka_mlt_tcp_color);
	}
	if (ready) window.draw(sp_mlt_tcp_ready);
	window.draw(tx_IP);
	window.draw(tx_Port);
	window.draw(tx_name);
	window.draw(tx_start_button);
}
void GameParameters::DrawSinglePlayer(RenderWindow &window)
{
	window.draw(sp_singleplayer);
	window.draw(sp_galka);
	window.draw(sp_snake);
	window.draw(tx_diff);
}
void GameParameters::DrawGame(RenderWindow &window)
{
	window.draw(sp_gameground);
	for (register int i = 0; i < (int)shapes.size(); ++i) window.draw(shapes[i]);
	for (register int i = 0; i < (int)lights.size(); ++i) window.draw(lights[i]);
	if (selected) window.draw(sp_selected);
	window.draw(tx_status);
	window.draw(tx_name1ingame);
	window.draw(tx_name2ingame);
}

void GameParameters::UpdateMainMenu()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(364, 200, 861, 300)) { gameState = st_Single_Player; thread_mode = AI; return; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(364, 316, 861, 416)) { gameState = st_Multiplayer; return; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(364, 432, 861, 532)) { b_Game = false; return; }
}
void GameParameters::UpdateMultiplayer()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(20, 20, 250, 129)) { gameState = st_Main_Menu; return; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(349, 131, 885, 264)) { gameState = st_Mlt_One; input = true; return; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(349, 423, 885, 556))
	{
		gameState = st_Mlt_TCP;
		thread_mode = Net;
		go = false;
		ready = false;
		thread_state = Waiting;
		tx_start_button.setString("    Create room");
		return;
	}
}
void GameParameters::UpdateMltOne(Event &eve)
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(20, 20, 250, 129))
	{
		input = false;
		once = true;
		ready = false;
		tx_name1.setString("");
		tx_name2.setString("");
		name1 = "";
		name2 = "";
		turn = t_Player1;
		gameState = st_Main_Menu;
		return;
	}
	if (input && turn == t_Player2)
	{
		if (Keyboard::isKeyPressed(Keyboard::Return) && name2 != ""&& name2 != "AI")
		{
			turn = t_Player1;
			once = false;
			ready = true;
			tx_name2.setString(name2 + "   OK");
		}
		else if (eve.type == Event::TextEntered && once)
		{
			if (eve.key.code == 8)
			{
				if (name2 != "") name2.erase(name2.size() - 1);
			}
			else if (!Keyboard::isKeyPressed(Keyboard::Return))
			{
				name2 += char(eve.key.code);
			}
			tx_name2.setString(name2);
			once = false;
		}
		if (eve.type == sf::Event::KeyReleased) once = true;
	}
	if (input && turn == t_Player1)
	{
		if (Keyboard::isKeyPressed(Keyboard::Return) && name1 != "" && name1 != "AI")
		{
			turn = t_Player2;
			once = false;
			tx_name1.setString(name1 + "   OK");
		}
		else if (eve.type == Event::TextEntered && once)
		{
			if (eve.key.code == 8)
			{
				if (name1 != "") name1.erase(name1.size() - 1);
			}
			else if (!Keyboard::isKeyPressed(Keyboard::Return))
			{
				name1 += char(eve.key.code);
			}
			tx_name1.setString(name1);
			once = false;
		}
		if (eve.type == sf::Event::KeyReleased) once = true;
	}
	if (ready && mouse.isButtonPressed(mouse.Left) && MouseInside(578, 508, 1158, 662))
	{
		tx_name1.setString("");
		tx_name2.setString("");
		ready = false;
		input = true;
		once = true;
		main_desk = Desk(true);
		turn = t_Player1;
		selected = false;
		memset(light, 0, 32);
		tx_name1ingame.setString(name1);
		tx_name2ingame.setString(name2);
		gameState = st_Game_One;
		invert_desk = false;
	}
}
void GameParameters::UpdateMltTcp(Event &eve)
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(20, 20, 250, 129))
	{
		thread_mode = AI;
		server = true;
		sp_galka_server_client.setPosition(Vector2f(32, 181));
		server_auto_bind = true;
		sp_galka_tcp_bind.setPosition(Vector2f(388 + 297, 164 + 7));
		tx_IP.setString("auto");
		tx_Port.setString("auto");
		tx_name.setString("");
		ip = "";
		port = "";
		name1 = "";
		name2 = "";
		input_ip = false;
		input_port = false;
		input_name = true;
		go = false;
		ready = false;
		tx_start_button.setString("    Create room");
		player_color = true;
		gameState = st_Main_Menu;
		thread_state = Waiting;
		tx_start_button.setCharacterSize(70);
		closesocket(listen_socket);
		closesocket(work_socket);
		return;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(1081, 287, 1180, 387))
	{
		if (name1 != "" && !input_name) ready = true;
	}
	if (mouse.isButtonPressed(mouse.Right))
	{
		if (!go || thread_state == Error)
		{
			go = false;
			ready = false;
			thread_state = Waiting;
			if (server) tx_start_button.setString("    Create room"); else tx_start_button.setString("        Connect");
			name2 = "";
			if (server && server_auto_bind)
			{
				ip = "";
				port = "";
				name1 = "";
				tx_IP.setString("auto");
				tx_Port.setString("auto");
				tx_name.setString("");
				input_ip = false;
				input_port = false;
				input_name = true;
			}
			else
			{
				ip = "";
				port = "";
				name1 = "";
				tx_IP.setString("");
				tx_Port.setString("");
				tx_name.setString("");
				input_ip = true;
				input_port = false;
				input_name = false;
			}
		}
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(32, 181, 131, 281) && !ready)
	{
		server = true;
		sp_galka_server_client.setPosition(Vector2f(32, 181));
		if (server_auto_bind)
		{
			tx_IP.setString("auto");
			tx_Port.setString("auto");
			ip = "";
			port = "";
			input_ip = false;
			input_port = false;
			input_name = true;
		}
		else
		{
			tx_IP.setString("");
			tx_Port.setString("");
			ip = "";
			port = "";
			input_ip = true;
			input_port = false;
			input_name = false;
		}
		name1 = "";
		tx_name.setString("");
		tx_start_button.setString("    Create room");
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(32, 311, 131, 411) && !ready)
	{
		server = false;
		sp_galka_server_client.setPosition(Vector2f(32, 311));
		tx_IP.setString("");
		tx_Port.setString("");
		tx_name.setString("");
		ip = "";
		port = "";
		name1 = "";
		input_ip = true;
		input_port = false;
		input_name = false;
		tx_start_button.setString("        Connect");
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(388 + 297, 164 + 7, 388 + 396, 164 + 107) && server && !ready)
	{
		server_auto_bind = true;
		sp_galka_tcp_bind.setPosition(Vector2f(388 + 297, 164 + 7));
		tx_IP.setString("auto");
		tx_Port.setString("auto");
		tx_name.setString("");
		ip = "";
		port = "";
		name1 = "";
		input_ip = false;
		input_port = false;
		input_name = true;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(388 + 607, 164 + 7, 388 + 706, 164 + 107) && server && !ready)
	{
		server_auto_bind = false;
		sp_galka_tcp_bind.setPosition(Vector2f(388 + 607, 164 + 7));
		tx_IP.setString("");
		tx_Port.setString("");
		tx_name.setString("");
		ip = "";
		port = "";
		name1 = "";
		input_ip = true;
		input_port = false;
		input_name = false;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(27 + 107, 466 + 55, 27 + 206, 466 + 155) && server && !ready)
	{
		player_color = true;
		sp_galka_mlt_tcp_color.setPosition(Vector2f(27 + 107, 446 + 55));
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(27 + 331, 466 + 59, 27 + 430, 466 + 159) && server && !ready)
	{
		player_color = false;
		sp_galka_mlt_tcp_color.setPosition(Vector2f(27 + 331, 446 + 59));
	}
	if (input_name)
	{
		if (Keyboard::isKeyPressed(Keyboard::Return) && name1 != "")
		{
			input_name = false;
			once = false;
			tx_name.setString(name1 + "   OK");
			name1 += "\0";
		}
		else if (eve.type == Event::TextEntered && once)
		{
			if (eve.key.code == 8)
			{
				if (name1 != "") name1.erase(name1.size() - 1);
			}
			else if (!Keyboard::isKeyPressed(Keyboard::Return))
			{
				name1 += char(eve.key.code);
			}
			tx_name.setString(name1);
			once = false;
		}
		if (eve.type == sf::Event::KeyReleased) once = true;
	}
	if (input_port)
	{
		if (Keyboard::isKeyPressed(Keyboard::Return) && port != "")
		{
			input_port = false;
			input_name = true;
			once = false;
			tx_Port.setString(port + "   OK");
		}
		else if (eve.type == Event::TextEntered && once)
		{
			if (eve.key.code == 8)
			{
				if (port != "") port.erase(port.size() - 1);
			}
			else if (!Keyboard::isKeyPressed(Keyboard::Return))
			{
				port += char(eve.key.code);
			}
			tx_Port.setString(port);
			once = false;
		}
		if (eve.type == sf::Event::KeyReleased) once = true;
	}
	if (input_ip)
	{
		if (Keyboard::isKeyPressed(Keyboard::Return) && ip != "")
		{
			input_ip = false;
			input_port = true;
			once = false;
			tx_IP.setString(ip + "   OK");
		}
		else if (eve.type == Event::TextEntered && once)
		{
			if (eve.key.code == 8)
			{
				if (ip != "") ip.erase(ip.size() - 1);
			}
			else if (!Keyboard::isKeyPressed(Keyboard::Return))
			{
				ip += char(eve.key.code);
			}
			tx_IP.setString(ip);
			once = false;
		}
		if (eve.type == sf::Event::KeyReleased) once = true;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(520, 481, 1177, 635) && ready && !go)
	{
		go = true;
		if (thread_state == Ready)
		{

		}
	}
}
void GameParameters::UpdateSinglePlayer()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(20, 20, 250, 129))
	{
		gameState = st_Main_Menu;
		difficult = 6;
		player_color = true;
		sp_snake.setPosition(Vector2f(169, 221));
		tx_diff.setString("6");
		name1 = "";
		name2 = "";
		return;
	}
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(514, 202, 613, 302)) { player_color = true; }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(515, 328, 614, 427)) { player_color = false; }
	if (player_color) { sp_galka.setPosition(Vector2f(514, 202)); }
	else { sp_galka.setPosition(Vector2f(515, 328)); }
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(160, 236, 209, 599))
	{
		sp_snake.setPosition(Vector2f(169, (float)MouseY - 15));
		for (int i = 0; i < 11; i++) if ((sp_snake.getPosition().y + 15 > 220 + 34 * i) && (sp_snake.getPosition().y + 15 < 220 + 34 * (i + 1))) difficult = i + 6;
	}
	tx_diff.setString(std::to_string(difficult));
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(520, 481, 1100, 635))
	{
		if (player_color)
		{
			name1 = "Player";
			name2 = "AI";
		}
		else
		{
			name1 = "AI";
			name2 = "Player";
		}
		main_desk = Desk(player_color);
		memset(light, 0, 32);
		once = true;
		selected = false;
		gameState = st_Game_Ai;
		tx_name1ingame.setString(name1);
		tx_name2ingame.setString(name2);
		thread_mode = AI;
		invert_desk = !player_color;
		turn = (player_color ? t_Player1 : t_Player2);
	}
}

void GameParameters::UpdateGameAI()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(784, 24, 1170, 136))
	{
		gameState = st_Main_Menu;
		difficult = 6;
		player_color = true;
		sp_snake.setPosition(Vector2f(169, 221));
		tx_diff.setString("3");
		name1 = "";
		name2 = "";
		shapes.clear();
		lights.clear();
		once = false;
		turn = t_Player1;
		player_list = nullptr;
		tx_name1ingame.setString("");
		tx_name2ingame.setString("");
		tx_status.setString("");
		game_over = false;
		must_beat = false;
		search_time = 0;
		return;
	}
	if (!game_over)
	{
		std::string s = std::to_string(search_time);
		s.erase(4, s.length());
		if (turn == t_Player1)
		{
			if (player_color) tx_status.setString(name1 + "'s turn " + s + "s. " + std::to_string(operations));
			else tx_status.setString(name2 + "'s turn " + s + "s. " + std::to_string(operations));
			PlayerTurn(player_color, main_desk);
		}
		else
		{
			if (player_color) tx_status.setString(name2 + "'s turn... ");
			else  tx_status.setString(name1 + "'s turn... ");
			if (thread_state == Waiting) thread_state = Working;
			if (thread_state == Ready)
			{
				thread_state = Waiting;
				NextTurn();
			}
		}
	}
	else
	{
		thread_state = Waiting;
		if (turn == t_Player1)
		{
			tx_status.setString(name1 + " wins!!!");
		}
		else
		{
			tx_status.setString(name2 + " wins!!!");
		}
	}
	if (once)
	{
		if (turn == t_Player2) thread_state = Working;
		game_over = GameOver((turn == t_Player1 ? true : false));
		must_beat = MustBeat(main_desk, (turn == t_Player1 ? player_color : !player_color));
		ResetShapes();
		once = false;
	}
}
void GameParameters::UpdateGameOne()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(784, 24, 1170, 136))
	{
		gameState = st_Main_Menu;
		name1 = "";
		name2 = "";
		shapes.clear();
		lights.clear();
		once = false;
		turn = t_Player1;
		player_list = nullptr;
		tx_name1ingame.setString("");
		tx_name2ingame.setString("");
		tx_status.setString("");
		game_over = false;
		must_beat = false;
		return;
	}
	if (!game_over)
	{
		if (turn == t_Player1)
		{
			tx_status.setString(name1 + " is turning");
			PlayerTurn(true, main_desk);
		}
		else
		{
			tx_status.setString(name2 + " is turning");
			PlayerTurn(false, main_desk);
		}
	}
	else
	{
		if (turn == t_Player1)
		{
			tx_status.setString(name2 + " wins!!!");
		}
		else
		{
			tx_status.setString(name1 + " wins!!!");
		}
	}
	if (once)
	{
		game_over = GameOver((turn == t_Player1 ? true : false));
		must_beat = MustBeat(main_desk, (turn == t_Player1 ? true : false));
		ResetShapes();
		once = false;
	}
}
void GameParameters::UpdateGameTcp()
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(784, 24, 1170, 136))
	{
		gameState = st_Main_Menu;
		shapes.clear();
		lights.clear();
		once = false;
		turn = t_Player1;
		DeleteMoveList(player_list);
		player_list = nullptr;
		tx_name1ingame.setString("");
		tx_name2ingame.setString("");
		tx_status.setString("");
		game_over = false;
		must_beat = false;
		thread_mode = AI;
		server = true;
		sp_galka_server_client.setPosition(Vector2f(32, 181));
		server_auto_bind = true;
		sp_galka_tcp_bind.setPosition(Vector2f(388 + 297, 164 + 7));
		tx_IP.setString("auto");
		tx_Port.setString("auto");
		tx_name.setString("");
		ip = "";
		port = "";
		name1 = "";
		name2 = "";
		input_ip = false;
		input_port = false;
		input_name = true;
		go = false;
		ready = false;
		tx_start_button.setString("    Create room");
		player_color = true;
		gameState = st_Main_Menu;
		thread_state = Waiting;
		tx_start_button.setCharacterSize(70);
		closesocket(listen_socket);
		closesocket(work_socket);
		return;
	}
	if (!game_over)
	{
		if (turn == t_Player1)
		{
			tx_status.setString(name1 + "'s (yours) turn.");
			PlayerTurn(player_color, main_desk);
			if (turn == t_Player2) thread_state = Sending;
		}
		else
		{
			tx_status.setString(name2 + "'s turn...");
			if (thread_state == Working) NextTurn();
		}
	}
	else
	{
		if (turn == t_Player1)
		{
			tx_status.setString(name2 + " wins!!!");
		}
		else
		{
			tx_status.setString(name1 + " (You) wins!!!");
		}
	}
	if (once)
	{
		game_over = GameOver((turn == t_Player1 ? true : false));
		must_beat = MustBeat(main_desk, (turn == t_Player1 ? player_color : !player_color));
		ResetShapes();
		once = false;
	}
}

void GameParameters::ResetShapes()
{
	shapes.clear();
	lights.clear();
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i))
		{
			if (GetBit(main_desk.color, i))
			{
				if (GetBit(main_desk.queen, i))
				{
					Sprite sp;
					sp.setTexture(tex_white_queen);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
				else
				{
					Sprite sp;
					sp.setTexture(tex_white_shape);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
			}
			else
			{
				if (GetBit(main_desk.queen, i))
				{
					Sprite sp;
					sp.setTexture(tex_black_queen);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
				else
				{
					Sprite sp;
					sp.setTexture(tex_black_shape);
					sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
					shapes.push_back(sp);
				}
			}
		}
		if (light[i])
		{
			Sprite sp;
			sp.setTexture(tex_light);
			sp.setPosition((Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64))));
			lights.push_back(sp);
		}
	}
}
bool GameParameters::GameOver(bool color)
{
	bool res = true;
	MoveList * temp = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i) && (GetBit(main_desk.color, i) == color))
		{
			FindBeats(temp, main_desk, true, i);
			if (!temp) FindMoves(temp, main_desk, true, i);
			if (temp)
			{
				res = false;
				break;
			}
		}
	}
	if (res) { turn = t_Player2; DeleteMoveList(temp); return true; }
	res = true;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(main_desk.exist, i) && (GetBit(main_desk.color, i) != color))
		{
			res = false;
			break;
		}
	}
	if (res) { turn = t_Player1; DeleteMoveList(temp); return true; }
	return false;
}
void GameParameters::PlayerTurn(bool color, Desk &current_desk)
{
	if (mouse.isButtonPressed(mouse.Left) && MouseInside(100, 100, 611, 611))
	{
		char cell = ClickCell();
		if (cell != -1)
		{
			if (selected)
			{
				if (GetBit(current_desk.exist, cell) && GetBit(current_desk.color, cell) == color)
				{
					ResetData();
					player_list = nullptr;
					FindBeats(player_list, current_desk, color, cell);
					if (!player_list && !must_beat) FindMoves(player_list, current_desk, color, cell);
					if (player_list) SelectCell(cell);
				}
				else if (light[cell])
				{
					Move m = MoveFromCell(player_list, cell);
					MakeMove(current_desk, m);
					DeleteMoveList(player_list);
					BecameQueen(current_desk);
					FindBeats(player_list, current_desk, color, m.next);
					if (!(m.must_beat) || !(player_list != nullptr)) NextTurn();
					ResetData();
				}
				else ResetData();
			}
			else
			{
				if (GetBit(current_desk.exist, cell) && GetBit(current_desk.color, cell) == color)
				{
					FindBeats(player_list, current_desk, color, cell);
					if (!player_list && !must_beat) FindMoves(player_list, current_desk, color, cell);
					if (player_list) SelectCell(cell);
				}
			}
		}
		else ResetData();
		once = true;
	}
	if (mouse.isButtonPressed(mouse.Right))
	{
		ResetData();
		once = true;
	}
}

////// Subfunctions ///
inline bool GameParameters::MouseInside(int first_x, int first_y, int second_x, int second_y)
{
	return ((MouseX >= first_x) && (MouseX <= second_x) && (MouseY >= first_y) && (MouseY <= second_y) && focus);
}
inline void GameParameters::ResetData()
{
	selected = false;
	DeleteMoveList(player_list);
	memset(light, 0, 32);
	selected_cell = -1;
}
inline bool GameParameters::Celltype(char n)
{
	return ((n / 4) % 2 == 0);
}
char GameParameters::ClickCell()
{
	for (register char i = 0; i < 32; i++)
	{
		Vector2f temp = (Celltype(i) ? Vector2f((float)(100 + (i % 4) * 2 * 64), (float)(548 - (i / 8) * 2 * 64)) : Vector2f((float)(100 + (i % 4) * 2 * 64 + 64), (float)(548 - (i / 8) * 2 * 64 - 64)));
		if (MouseInside((int)temp.x, (int)temp.y, (int)temp.x + 64, (int)temp.y + 64)) return i;
	}
	return -1;
}
inline void GameParameters::SelectCell(char cell)
{
	selected = true;
	selected_cell = cell;
	sp_selected.setPosition((Celltype(cell) ? Vector2f((float)(100 + (cell % 4) * 2 * 64), (float)(548 - (cell / 8) * 2 * 64)) : Vector2f((float)(100 + (cell % 4) * 2 * 64 + 64), (float)(548 - (cell / 8) * 2 * 64 - 64))));
	MoveList * p = player_list;
	while (p)
	{
		light[p->move.next] = true;
		p = p->next;
	}
}
inline void GameParameters::NextTurn()
{
	if (thread_mode == AI && name1 != "AI" && name2 != "AI") InvertDesk(main_desk);
	if (turn == t_Player1) turn = t_Player2;
	else turn = t_Player1;
}
Move GameParameters::MoveFromCell(MoveList * list, char cell)
{
	while (list)
	{
		if (list->move.next == cell) break;
		list = list->next;
	}
	return list->move;
}
void GameParameters::FindMoves(MoveList * &list, Desk current_desk, bool color, char cell)
{
	if (GetBit(current_desk.queen, cell)) // Если дамка
	{
		bool b = Celltype(cell);
		char k = cell;
		if ((k % 8) != 0 && (k < 28)) // Влево-вверх
			while (true)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k > 27) || ((k % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if (((k + 1) % 8) != 0 && (k < 28)) // Вправо-вверх
			while (true)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k > 27) || (((k + 1) % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if (((k + 1) % 8) != 0 && (k > 3)) // Вправо-вниз
			while (true)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k < 4) || (((k + 1) % 8) == 0)) break;
			}

		b = Celltype(cell);
		k = cell;
		if ((k % 8) != 0 && (k > 3)) // Влево-вниз
			while (true)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				else break;
				if ((k < 4) || ((k % 8) == 0)) break;
			}
	}
	else // Если шашка
	{
		if (!invert_desk)
			if (color) // Если белая шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell + 3 : cell + 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell + 4 : cell + 5);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
			else // Если черная шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell - 5 : cell - 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell - 4 : cell - 3);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
		if (invert_desk)
			if (!color) // Если белая шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell + 3 : cell + 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell + 4 : cell + 5);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
			else // Если черная шашка
			{
				if ((cell % 8) != 0) // Налево
				{
					char k = (Celltype(cell) ? cell - 5 : cell - 4);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
				if (((cell + 1) % 8) != 0) // Направо
				{
					char k = (Celltype(cell) ? cell - 4 : cell - 3);
					if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, 0, 0));
				}
			}
	}
}
void GameParameters::FindBeats(MoveList * &list, Desk current_desk, bool color, char cell)
{
	if (GetBit(current_desk.queen, cell)) // Дамка
	{
		// Влево-вверх
		bool b = Celltype(cell);
		bool found = false;
		char k = cell;
		char kill = -1;
		if ((k % 8) != 0 && (k < 28))
			while (true)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k > 23) || ((k % 4) == 0)) break;
			}
		if ((k % 8) != 0 && (k < 28))
			while (found)
			{
				k += (b ? 3 : 4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k > 27) || ((k % 8) == 0)) break;
			}

		// Вправо-вверх
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if (((k + 1) % 8) != 0 && (k < 28))
			while (true)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k > 23) || (((k + 1) % 4) == 0)) break;
			}
		if (((k + 1) % 8) != 0 && (k < 28))
			while (found)
			{
				k += (b ? 4 : 5);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k > 27) || (((k + 1) % 8) == 0)) break;
			}

		// Вправо-вниз
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if (((k + 1) % 8) != 0 && (k > 3))
			while (true)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k < 8) || (((k + 1) % 4) == 0)) break;
			}
		if (((k + 1) % 8) != 0 && (k > 3)) // Вправо-вниз
			while (found)
			{
				k += (b ? -4 : -3);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k < 4) || (((k + 1) % 8) == 0)) break;
			}

		// Влево-вниз
		b = Celltype(cell);
		found = false;
		k = cell;
		kill = -1;
		if ((k % 8) != 0 && (k > 3))
			while (true)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (GetBit(current_desk.exist, k))
				{
					if (!GetBit(current_desk.color, k) == color)
					{
						found = true;
						kill = k;
						break;
					}
					else break;
				}
				if ((k < 8) || ((k % 4) == 0)) break;
			}
		if ((k % 8) != 0 && (k > 3)) // Влево-вниз
			while (found)
			{
				k += (b ? -5 : -4);
				b = !b;
				if (!GetBit(current_desk.exist, k)) AddMove(list, Move(cell, k, kill, 1));
				else break;
				if ((k < 4) || ((k % 8) == 0)) break;
			}
	}
	else // Шашка
	{
		if (cell < 24) // верхняя граница
		{
			if ((cell % 4) != 0) // Налево
			{
				char k = (Celltype(cell) ? cell + 3 : cell + 4);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell + 7)) AddMove(list, Move(cell, cell + 7, k, 1));
			}
			if (((cell + 1) % 4) != 0) // Направо
			{
				char k = (Celltype(cell) ? cell + 4 : cell + 5);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell + 9)) AddMove(list, Move(cell, cell + 9, k, 1));
			}
		}
		if (cell > 7) // нижняя граница
		{
			if ((cell % 4) != 0) // Налево
			{
				char k = (Celltype(cell) ? cell - 5 : cell - 4);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell - 9)) AddMove(list, Move(cell, cell - 9, k, 1));
			}
			if (((cell + 1) % 4) != 0) // Направо
			{
				char k = (Celltype(cell) ? cell - 4 : cell - 3);
				if (GetBit(current_desk.exist, k) && GetBit(current_desk.color, k) != color && !GetBit(current_desk.exist, cell - 7)) AddMove(list, Move(cell, cell - 7, k, 1));
			}
		}
	}
}
void GameParameters::MakeMove(Desk &current_desk, Move move)
{
	current_desk.exist = SetBit(current_desk.exist, move.next, 1);
	current_desk.color = SetBit(current_desk.color, move.next, GetBit(current_desk.color, move.prev));
	current_desk.queen = SetBit(current_desk.queen, move.next, GetBit(current_desk.queen, move.prev));

	current_desk.exist = SetBit(current_desk.exist, move.prev, 0);
	current_desk.color = SetBit(current_desk.color, move.prev, 0);
	current_desk.queen = SetBit(current_desk.queen, move.prev, 0);

	if (move.must_beat)
	{
		current_desk.exist = SetBit(current_desk.exist, move.beat, 0);
		current_desk.color = SetBit(current_desk.color, move.beat, 0);
		current_desk.queen = SetBit(current_desk.queen, move.beat, 0);
	}
}
void GameParameters::BecameQueen(Desk &current_desk)
{
	if (invert_desk)
	{
		for (register char i = 0; i < 4; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i))   current_desk.queen = SetBit(current_desk.queen, i, 1);
			if (GetBit(current_desk.exist, i + 28) && !GetBit(current_desk.color, i + 28))   current_desk.queen = SetBit(current_desk.queen, i + 28, 1);
		}
	}
	else
	{
		for (register char i = 0; i < 4; i++)
		{
			if (GetBit(current_desk.exist, i) && !GetBit(current_desk.color, i))   current_desk.queen = SetBit(current_desk.queen, i, 1);
			if (GetBit(current_desk.exist, i + 28) && GetBit(current_desk.color, i + 28))   current_desk.queen = SetBit(current_desk.queen, i + 28, 1);
		}
	}
}
bool GameParameters::MustBeat(Desk &current_desk, bool color)
{
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && (GetBit(current_desk.color, i)) == color)
		{
			FindBeats(list, current_desk, color, i);
			if (list) { DeleteMoveList(list); return true; }
		}
	}
	return false;
}
void GameParameters::InvertDesk(Desk &current_desk)
{
	Desk desk;
	for (register char i = 0; i < 32; i++)
	{
		desk.exist = SetBit(desk.exist, i, GetBit(main_desk.exist, 31 - i));
		desk.color = SetBit(desk.color, i, GetBit(main_desk.color, 31 - i));
		desk.queen = SetBit(desk.queen, i, GetBit(main_desk.queen, 31 - i));
	}
	current_desk = desk;
	invert_desk = !invert_desk;
}

void GameParameters::ThreadFunc()
{
	while (true)
	{
		if (thread_state == Terminate) return; ///////////
		if (thread_mode == AI)
		{
			if (thread_state == Working) ///////////
			{
				start_time = clock();
				AITurn(!player_color, main_desk);
				once = true;
				end_time = clock();
				search_time = end_time - start_time;
				search_time /= (double)CLK_TCK;
			}
		}
		else if (thread_mode == Net)
		{
			if (thread_state == Waiting && go) ///////////
			{
				if (StartNetwork())
				{
					go = false;
					thread_state = Ready;
				}
				else thread_state = Error;
			}
			if (thread_state == Ready) ///////////
			{
				if (go && server)
				{
					strcpy(buff, (player_color ? "Start 0\0" : "Start 1\0"));
					send(work_socket, buff, sizeof(buff), 0);
					main_desk = Desk(player_color);
					invert_desk = !player_color;
					memset(light, 0, 32);
					once = true;
					selected = false;
					tx_name1ingame.setString(name1);
					tx_name2ingame.setString(name2);
					thread_state = (player_color ? Working : Listening);
					gameState = st_Game_TCP;
					turn = (player_color ? t_Player1 : t_Player2);
				}
				if (!server)
				{
					len = recv(work_socket, (char *)&buff, MAX_PACKET_SIZE, 0);
					std::string s = "";
					for (register char i = 0; i < 5; i++) s += buff[i];
					if (s == "Start")
					{
						player_color = (buff[6] == '1' ? true : false);
						main_desk = Desk(player_color);
						invert_desk = !player_color;
						memset(light, 0, 32);
						once = true;
						selected = false;
						tx_name1ingame.setString(name1);
						tx_name2ingame.setString(name2);
						thread_state = (player_color ? Working : Listening);
						gameState = st_Game_TCP;
						turn = (player_color ? t_Player1 : t_Player2);
					}
					else
					{
						thread_state = Error;
						tx_start_button.setString("Bad request!");
					}
				}
			}
			if (thread_state == Listening) ///////////
			{
				len = recv(work_socket, buff, MAX_PACKET_SIZE, 0);
				main_desk.exist = atoll(buff);
				len = recv(work_socket, buff, MAX_PACKET_SIZE, 0);
				main_desk.color = atoll(buff);
				len = recv(work_socket, buff, MAX_PACKET_SIZE, 0);
				main_desk.queen = atoll(buff);
				invert_desk = !player_color;
				BecameQueen(main_desk);
				once = true;
				thread_state = Working;
			}
			if (thread_state == Sending) ///////////
			{
				Desk temp = main_desk;
				InvertDesk(temp);
				strcpy(buff, std::to_string(temp.exist).c_str());
				send(work_socket, buff, MAX_PACKET_SIZE, 0);
				strcpy(buff, std::to_string(temp.color).c_str());
				send(work_socket, buff, MAX_PACKET_SIZE, 0);
				strcpy(buff, std::to_string(temp.queen).c_str());
				send(work_socket, buff, MAX_PACKET_SIZE, 0);
				thread_state = Listening;
			}
		}
		sleep(milliseconds(100));
	}
}
void GameParameters::AITurn(bool color, Desk &current_desk)
{
	////// Обнулили счетчик
	operations = 1;
	////// 

	////// Создаем все ходы
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && (GetBit(current_desk.color, i) == color))
			FindBeats(list, current_desk, color, i);
	}
	if (list == nullptr)
	{
		for (register char i = 0; i < 32; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
				FindMoves(list, current_desk, color, i);
		}
	}
	if (list == nullptr) return;
	//////

	////// Если только один ход
	if (list->next == nullptr)
	{
		MakeMove(main_desk, list->move);
		BecameQueen(main_desk);
		MoveList * p = nullptr;
		if (list->move.must_beat) FindBeats(p, current_desk, color, list->move.next);
		if (p)
		{
			DeleteMoveList(p);
		}
		else thread_state = Ready;
		DeleteMoveList(list);
		sleep(milliseconds(100));
		return;
	}
	//////

	////// Объявляем рабочие переменные
	MoveList * iter = list;
	Move best_move;
	int alpha = -INF;
	int beta = INF;
	////// 
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	auto func = [](int &val, bool color, Desk &current_desk, char depht, int alpha, int beta, bool FLAG)
	{
		if (FLAG) val = AIRecurrent(color, current_desk, depht, alpha, beta);
		else val = -AIRecurrent(!color, current_desk, depht, -beta, -alpha);
	};

	size_t move_cnt = 0;
	while (iter) { move_cnt++; iter = iter->next; }
	iter = list;
	Desk *all_variant = new Desk[move_cnt];
	int *all_values = new int[move_cnt];

	std::thread **threads = new std::thread*[move_cnt];
	for (size_t i = 0; i < move_cnt; i++)
	{
		threads[i] = nullptr;
	}

	for (size_t i = 0; i < move_cnt; i++)
	{
		all_variant[i] = current_desk;
		all_values[i] = -INF;
		MakeMove(all_variant[i], iter->move);
		BecameQueen(all_variant[i]);

		MoveList * p = nullptr;
		if (iter->move.must_beat) FindBeats(p, all_variant[i], color, iter->move.next);

		if (p)
		{
			threads[i] = new std::thread(func, std::ref(all_values[i]), color, std::ref(all_variant[i]), difficult - 1, alpha, beta, true);
			DeleteMoveList(p);
		}
		else threads[i] = new std::thread(func, std::ref(all_values[i]), color, std::ref(all_variant[i]), difficult - 1, alpha, beta, false);

		iter = iter->next;
	}

	for (size_t i = 0; i < move_cnt; i++)
	{
		if (threads[i] != nullptr && threads[i]->joinable()) 
			threads[i]->join();
	}

	for (size_t i = 0; i < move_cnt; i++)
		if (threads[i]) delete threads[i];
	delete[] threads;

	size_t ind = 0;
	for (size_t i = 0; i < move_cnt; i++)
		if (all_values[i] > all_values[ind]) ind = i;

	current_desk = all_variant[ind];

	iter = list;
	for (size_t i = 0; i < ind; i++) iter = iter->next;
	best_move = iter->move;

	delete[] all_variant;
	delete[] all_values;
	DeleteMoveList(list);
	//////////////////////////////////////////////////////////////////////////////////////////////

	/*
	////// Начинаем цикл перебора
	while (iter && (alpha < beta))
	{
		////// Создаем доску с новым ходом
		Desk temp_desk = current_desk;
		MakeMove(temp_desk, iter->move);
		BecameQueen(temp_desk);
		//////

		////// Проверяем наличие продолжение хода
		int temp = -INF;
		MoveList * p = nullptr;
		if (iter->move.must_beat) FindBeats(p, temp_desk, color, iter->move.next);
		////// 

		////// Проверка хода рекурсивной функцией
		if (p)
		{
			temp = AIRecurrent(color, temp_desk, difficult - 1, alpha, beta);
			DeleteMoveList(p);
		}
		else temp = -AIRecurrent(!color, temp_desk, difficult - 1, -beta, -alpha);
		//////

		////// Сравнивание хода
		if (temp > alpha)
		{
			alpha = temp;
			best_move = iter->move;
		}
		//////

		////// Итерация
		iter = iter->next;
	}
	DeleteMoveList(list);
	////// 

	////// Производим лучший ход
	MakeMove(current_desk, best_move);
	BecameQueen(current_desk);
	//////
	*/

	////// Проверяем продолжение хода
	if (best_move.must_beat) FindBeats(list, current_desk, color, best_move.next);
	if (list) DeleteMoveList(list);
	else thread_state = Ready;
	sleep(milliseconds(100));
	//////
}
int GameParameters::AIRecurrent(bool color, Desk current_desk, char depht, int alpha, int beta)
{
	////// Счетчик
	operations++;
	//////

	////// Оценить последний ход
	if (depht <= 0) return Evaluate(current_desk, color);
	//////

	////// Создать все ходы
	MoveList * list = nullptr;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
			FindBeats(list, current_desk, color, i);
	}
	if (list == nullptr)
	{
		for (register char i = 0; i < 32; i++)
		{
			if (GetBit(current_desk.exist, i) && GetBit(current_desk.color, i) == color)
				FindMoves(list, current_desk, color, i);
		}
	}
	if (list == nullptr) return Evaluate(current_desk, color) - depht;
	//////

	////// Объявить рабочие переменные
	MoveList * iter = list;
	//////

	////// Запуск цикла перебора
	while (iter && (alpha < beta))
	{
		////// Создать доску с новым ходом
		Desk temp_desk = current_desk;
		MakeMove(temp_desk, iter->move);
		BecameQueen(temp_desk);
		//////

		////// Проверка наличия продолжения хода
		int temp = -INF;
		MoveList * p = nullptr;
		if (iter->move.must_beat) FindBeats(p, temp_desk, color, iter->move.next);
		//////

		////// Проверка хода рекурсивной функцией
		if (p)
		{
			temp = AIRecurrent(color, temp_desk, depht - 1, alpha, beta);
			DeleteMoveList(p);
		}
		else temp = -AIRecurrent(!color, temp_desk, depht - 1, -beta, -alpha);
		//////

		////// Сравнивание хода
		if (temp > alpha) alpha = temp;
		//////

		////// Итерация
		iter = iter->next;
		//////
	}
	DeleteMoveList(list);

	////// Возврат оценки
	return alpha;
}
int GameParameters::Evaluate(Desk &current_desk, bool color)
{
	int score1 = 0;
	int score2 = 0;
	for (register char i = 0; i < 32; i++)
	{
		if (GetBit(current_desk.exist, i))
		{
			if (GetBit(current_desk.color, i) == color)
			{
				score1 += (GetBit(current_desk.queen, i) ? 30 : 10);

			}
			else
			{
				score2 -= (GetBit(current_desk.queen, i) ? 30 : 10);
			}
		}
	}
	if (score1 == 0) return -INF;
	if (score2 == 0) return INF;
	return score1 + score2;
}

bool GameParameters::StartNetwork()
{
	int test;
	if (server)
	{
		tx_start_button.setString("Init socket...");
		listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listen_socket == INVALID_SOCKET) { tx_start_button.setString("Invalid socket!"); return false; }

		tx_start_button.setString("Bindind...");
		ZeroMemory(&adr, sizeof(adr));
		if (server_auto_bind)
		{
			hn = gethostbyname(INADDR_ANY);
			if (hn == NULL) { tx_start_button.setString("Error adr!"); return false; }
			adr.sin_port = htons(0);
		}
		else
		{
			hn = gethostbyname(ip.c_str());
			if (hn == NULL) { tx_start_button.setString("Error adr!"); return false; }
			adr.sin_port = htons(atoi(port.c_str()));
		}
		adr.sin_family = AF_INET;
		adr.sin_addr.S_un.S_addr = *(DWORD*)hn->h_addr_list[0];
		test = bind(listen_socket, (sockaddr*)&adr, sizeof(adr));
		if (test == SOCKET_ERROR) { tx_start_button.setString("Error bind!"); return false; }

		if (server_auto_bind)
		{
			tx_start_button.setString("getsockname...");
			int n_l = sizeof(name);
			ZeroMemory(&name, sizeof(name));
			test = getsockname(listen_socket, (sockaddr*)&name, &n_l);
			if (test == SOCKET_ERROR) { tx_start_button.setString("Error getsockname!"); return false; }
			tx_IP.setString(inet_ntoa((in_addr)name.sin_addr));
			tx_Port.setString(std::to_string(ntohs(name.sin_port)));
		}

		tx_start_button.setString("Listening...");
		test = listen(listen_socket, 1);
		if (FAILED(test)) { tx_start_button.setString("Error listen!"); return false; }

		tx_start_button.setString("Accepting...");
		int new_len = sizeof(new_ca);
		ZeroMemory(&new_ca, sizeof(new_ca));
		work_socket = accept(listen_socket, (sockaddr*)&new_ca, &new_len);
		if (FAILED(work_socket)) { tx_start_button.setString("Error accepting!"); return false; }

		tx_start_button.setString("Accepted!!!");
		len = recv(work_socket, (char *)&buff, MAX_PACKET_SIZE, 0);
		for (register char i = 0; i < len; i++) { if (buff[i] == '\0') break; name2 += buff[i]; }
		std::strcpy(buff, name1.c_str());
		test = send(work_socket, buff, sizeof(buff), 0);
		tx_start_button.setCharacterSize(50);
		tx_start_button.setString("Client: " + name2 + ". Go?");
		return true;
	}
	else
	{
		tx_start_button.setString("Init socket...");
		work_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (work_socket == INVALID_SOCKET) { tx_start_button.setString("Invalid socket!"); return false; }

		tx_start_button.setString("Set adr & port...");
		ZeroMemory(&adr, sizeof(adr));
		adr.sin_family = AF_INET;
		adr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		adr.sin_port = htons(std::atoi(port.c_str()));

		tx_start_button.setString("Connecting...");
		test = connect(work_socket, (sockaddr*)&adr, sizeof(adr));
		if (SOCKET_ERROR == test) { tx_start_button.setString("Connection error!"); return false; }

		tx_start_button.setString("Connected!!!");
		std::strcpy(buff, name1.c_str());
		test = send(work_socket, buff, sizeof(buff), 0);

		len = recv(work_socket, (char *)&buff, MAX_PACKET_SIZE, 0);
		for (register char i = 0; i < len; i++) { if (buff[i] == '\0') break; name2 += buff[i]; }
		tx_start_button.setCharacterSize(50);
		tx_start_button.setString("Server: " + name2);
		return true;
	}
}