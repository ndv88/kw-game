#include <hgeguictrls.h>

#include "Client.h"
#include "Network.h"
#include "World.h"
#include "Initiate.h"

extern Entity *player;

SOCKET ConnectToServer(void *world, std::string srv_ip)
{
	World *w = (World *)world;
	std::vector<Entity *> *entitys = w->GetWorldEntitys();

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(2008);
	addr.sin_addr.S_un.S_addr = inet_addr(srv_ip.c_str());
	
	//hge->System_Log("connect %s to ", srv_ip.c_str());

	int con = connect(sock,(SOCKADDR*)&addr,sizeof(addr));

	if (con != SOCKET_ERROR)
	{
		int result = 0;
		int id = -1;
		int size = -1;
		ENTITY_NET_DATA data;


		int cmd = CONNECT;
		result = send(sock, (char *)&cmd, sizeof(int), 0);

		result = recv(sock, (char *)&id, sizeof(int), 0);

		InitiatePlayer(id);
		//RefreshPlayer();

		player->GetEntityData(data);
		send(sock, (char *)&data, sizeof(ENTITY_NET_DATA), 0);

		result = recv(sock, (char *)&size, sizeof(int), 0);

		entitys->resize(size);

		for (int i = 0; i < size; i++)
		{
			result = recv(sock, (char *)&data, sizeof(ENTITY_NET_DATA), 0);


			Entity *e = new Entity(data.pos, GENERAL_MAP_LAYER);
			e->SetEntityData(data);

			int index = data.id - (data.id - i);

			(*entitys)[index] = e;
		}

		return sock;
	}
}



DWORD WINAPI FindServer(void *data)
{
	hgeGUIListbox *lb = (hgeGUIListbox *)data;

	WSAData wsad;
	if(!WSAStartup(MAKEWORD(2,0),&wsad))
	{
	    SOCKET s1;
	    s1 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	    SOCKADDR_IN sock_addr;
	    sock_addr.sin_family = AF_INET;
	    sock_addr.sin_port = htons(2000);
	    sock_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);
	    bind(s1, (SOCKADDR*)&sock_addr, sizeof(sock_addr));
	    listen(s1, 10);

	    SOCKET s2;
	    s2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	    if(s2)
	    {
		    SOCKADDR_IN addr;
		    addr.sin_family = AF_INET;
		    addr.sin_port = htons(3000);
			addr.sin_addr.S_un.S_addr = inet_addr("10.0.0.255");
		    sendto(s2, FIND_SRV, strlen(FIND_SRV), 0, (SOCKADDR*)&addr, sizeof(addr));
	    }
	
	    while(true)
	    {
		    int size = sizeof(sock_addr);
		    SOCKET s2 = accept(s1, (SOCKADDR*)&sock_addr, &size);

		    char buf[101];
		    int read = recv(s2, buf, 100, 0);
		    buf[read] = '\0';

			std::string str = buf;
			std::string str1, str2;
			int find = 0;
			find = str.find_first_of("|");
			str1 = str.substr(0, find);
			str2 = str.substr(find+1);

			if(strcmp(str1.c_str(), SRV_STR) == 0)
		    {
				HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX1);
				if (mutex)
				{
					WaitForSingleObject(mutex, INFINITE);
					
					std::string srv_ip;
					srv_ip = inet_ntoa(sock_addr.sin_addr);

					std::string item = srv_ip + "|" + str2;

					lb->AddItem((char *)item.c_str());

					ReleaseMutex(mutex);
				}
		    }
	    }
    }
	return 0;
}


DWORD WINAPI Client(void *param)
{
	NET_DATA *nd = (NET_DATA *)param;

	SOCKET sock = nd->sock;
	std::vector<Entity *> *entitys = (std::vector<Entity *>*)(nd->data);

	delete nd;

	int cmd = 0;
	int result = 0;
	ENTITY_NET_DATA_SMALL data = {0};
	int size1 = 0, size2 = 0;
	int id = 0;

	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);

	if (mutex)
	{
		do
		{
			WaitForSingleObject(mutex, INFINITE);

			player->GetEntityData(data);
			size1 = entitys->size();

			ReleaseMutex(mutex);

			cmd = CLIENT_UPDATE;

			result = send(sock, (char *)&cmd, sizeof(int), 0);
			result = send(sock, (char *)&data, sizeof(ENTITY_NET_DATA_SMALL), 0);
			result = send(sock, (char *)&size1, sizeof(int), 0);
			result = recv(sock, (char *)&size2, sizeof(int), 0);

			id = data.id;

			for (int i = 0; i < size1; i++)
			{
				if (id == i)
					continue;
				else
				{
					result = recv(sock, (char *)&data, sizeof(ENTITY_NET_DATA_SMALL), 0);

					if (result == sizeof(ENTITY_NET_DATA_SMALL) && result != 0)
					{
						WaitForSingleObject(mutex, INFINITE);

						int index = data.id - (data.id - i);
						(*entitys)[index]->SetEntityData(data);

						ReleaseMutex(mutex);
					}
				}
			}

			if (size1 < size2)
			{
				ENTITY_NET_DATA ed = {0};
				Entity *e;

				WaitForSingleObject(mutex, INFINITE);

				(*entitys).resize(size2);

				ReleaseMutex(mutex);

				for (int i = size1; i < size2; i++)
				{
					result = recv(sock, (char *)&ed, sizeof(ENTITY_NET_DATA), 0);

					e = new Entity(ed.pos, GENERAL_MAP_LAYER);
					e->SetEntityData(ed);

					WaitForSingleObject(mutex, INFINITE);

					int index = ed.id - (ed.id - i);
					(*entitys)[index] = e;

					ReleaseMutex(mutex);

					e = NULL;
				}
			}

		} while (cmd != SRV_DISCONNECT && result != SOCKET_ERROR && result != 0);
	}
	return 0;
}