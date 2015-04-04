#include "Server.h"
#include "Network.h"
#include "World.h"
#include "Initiate.h"


extern Entity *player;
extern std::string level_name;


typedef std::vector<Entity *> ENTITYS;


int ClientUpdate(SOCKET sock, ENTITYS *entitys);
int Connect(SOCKET sock, int &client_id, ENTITYS *entitys);
int ClientDisconnect(SOCKET sock, ENTITYS *entitys);


DWORD WINAPI WaitClients(void *param)
{
	WSAData wsad;
	if(!WSAStartup(MAKEWORD(2,0),&wsad))
	{
		SOCKET sock1;//на нём будем принимать
		SOCKET sock2;//на него будем отсылать
		int read = 0;

		sock1 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		
		SOCKADDR_IN sock_addr;		
		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(3000);
		sock_addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

		bind(sock1, (SOCKADDR*)&sock_addr, sizeof(sock_addr));

		
		while(true)
		{
			char buf[101];
			int size = sizeof(sock_addr);
			
			read = recvfrom(sock1, buf, 100, 0, (SOCKADDR*)&sock_addr, &size);

			if(read != SOCKET_ERROR)
			{
				buf[read] = '\0';
				if(strcmp(buf, FIND_SRV) == 0)
				{
					sock2 = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
					sock_addr.sin_port = htons(2000);
					sock_addr.sin_family = AF_INET;

					

					int con = 0;
					con = connect(sock2, (SOCKADDR*)&sock_addr, sizeof(sock_addr));

                    if(con != SOCKET_ERROR)
                    {
						//send server ;
						std::string send_str = SRV_STR;
						send_str += "|";
						send_str += level_name;

						send(sock2, send_str.c_str(), send_str.length(), 0);
                    }
                    closesocket(sock2);
				}
			}
		}
	}
	return 0;
}

DWORD WINAPI Accept(void *param)
{
	WSAData wsad;
	if(!WSAStartup(MAKEWORD(2,0),&wsad))
	{
		SOCKET sock;
		sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(2008);
		addr.sin_addr.S_un.S_addr = htons(INADDR_ANY);

		bind(sock,(SOCKADDR*)&addr,sizeof(addr));
		listen(sock, 20);

		NET_DATA *nd;

		while(1)
		{
			int size = sizeof(addr);
			SOCKET s = accept(sock,(SOCKADDR*)&addr,&size);

			nd = new NET_DATA;
			nd->data = param;
			nd->sock = s;

			CreateThread(NULL, 0, &Server, nd, 0, NULL);
		}
	}
	return 0;
}

DWORD WINAPI Server(void *param)
{
	NET_DATA *nd = (NET_DATA *)param;

	SOCKET sock = nd->sock;
	std::vector<Entity *> *entitys = (std::vector<Entity *>*)(nd->data);

	delete nd;

	int cmd = -1;
	int result = -1;

	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);

	int client_id = 0;

	if (mutex)
	{
		do
		{
			int cmd = -1;
			result = recv(sock, (char *)&cmd, sizeof(int), 0);

			if (result == sizeof(int))
			{
				switch(cmd)
				{
				case CONNECT:
						result = Connect(sock, client_id, entitys);
					break;
				case CLIENT_UPDATE:
					result = ClientUpdate(sock, entitys);
					break;
				case CLIENT_DISCONNECT:
					result = ClientDisconnect(sock, entitys);
					break;
				}
			}

		} while(cmd != CLIENT_DISCONNECT && result != SOCKET_ERROR && result != 0);

		closesocket(sock);

		WaitForSingleObject(mutex, INFINITE);

		if (entitys->size() >= (client_id-1))
		{
			if ((*entitys)[(client_id-1)] != NULL)
				(*entitys)[(client_id-1)]->is_disconnect = true;
		}

		ReleaseMutex(mutex);
	}

	return 0;
}

int Connect(SOCKET sock, int &client_id, ENTITYS *entitys)
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);

	ENTITY_NET_DATA data;
	int result = -1;
	int size = 0;
	int end_size = sizeof(ENTITY_NET_DATA);

	
	WaitForSingleObject(mutex, INFINITE);

	size = entitys->size()+1;
	client_id = entitys->size()+1;

	entitys->resize(size);

	ReleaseMutex(mutex);


	result = send(sock, (char *)&client_id, sizeof(int), 0);
	result = recv(sock, (char *)&data, end_size, 0);

	if (result != 0 && result == end_size)
	{
		WaitForSingleObject(mutex, INFINITE);

		Entity *e = new Entity(data.pos, GENERAL_MAP_LAYER);
		e->SetEntityData(data);
		(*entitys)[(data.id-1)] = e;

		player->GetEntityData(data);

		ReleaseMutex(mutex);
	}

	result = send(sock, (char *)&size, sizeof(int), 0);
	result = send(sock, (char *)&data, end_size, 0);

	for (int i = 0; i < (size-1); i++)
	{
		WaitForSingleObject(mutex, INFINITE);

		(*entitys)[i]->GetEntityData(data);

		ReleaseMutex(mutex);

		result = send(sock, (char *)&data, end_size, 0);
	}

	return result;
}

int ClientUpdate(SOCKET sock, ENTITYS *entitys)
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);

	int client_id = -1;
	ENTITY_NET_DATA_SMALL data = {0};
	int size1 = 0, size2 = 0;
	int result = -1;

	int end_size = sizeof(ENTITY_NET_DATA);
	int ends_size = sizeof(ENTITY_NET_DATA_SMALL);

	result = recv(sock, (char *)&data, ends_size, 0);


	if (result != 0 && result == ends_size)
	{
		WaitForSingleObject(mutex, INFINITE);

		client_id = data.id;
		size2 = entitys->size();
		
		(*entitys)[(data.id-1)]->SetEntityData(data);
		
		ReleaseMutex(mutex);
	}

	result = recv(sock, (char *)&size1, sizeof(int), 0);

	if (result == 0 && result != sizeof(int))
	{
		return result;
	}

	result = send(sock, (char *)&size2, sizeof(int), 0);


	WaitForSingleObject(mutex, INFINITE);
	player->GetEntityData(data);
	ReleaseMutex(mutex);

	send(sock, (char *)&data, ends_size, 0);

	for (int i = 0; i < size1; i++)
	{
		WaitForSingleObject(mutex, INFINITE);

		(*entitys)[i]->GetEntityData(data);

		ReleaseMutex(mutex);

		if (client_id != data.id)
			result = send(sock, (char *)&data, sizeof(ENTITY_NET_DATA_SMALL), 0);
	}

	if (size1 < size2)
	{
		ENTITY_NET_DATA ed = {0};

		std::vector<ENTITY_NET_DATA> temp;
		temp.resize(size2-size1);

		WaitForSingleObject(mutex, INFINITE);
		for (int i = size1; i < size2; i++)
		{
			(*entitys)[i]->GetEntityData(ed);
			temp.push_back(ed);
		}
		ReleaseMutex(mutex);

		for (int i = 0; i < temp.size(); i++)
		{
			result = send(sock, (char *)&temp[i], sizeof(ENTITY_NET_DATA), 0);
		}
	}
}

int ClientDisconnect(SOCKET sock, ENTITYS *entitys)
{
	HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, NAME_MUTEX2);
	int id = 0;
	int result = -1;

	result = recv(sock, (char *)&id, sizeof(int), 0);

	WaitForSingleObject(mutex, INFINITE);

	(*entitys)[(id-1)]->is_disconnect = true;

	ReleaseMutex(mutex);

	return result;
}