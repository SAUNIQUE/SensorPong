#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main() {
	
	//initialis� winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0) {
		cerr << "Can't Initialize winsock ! Quitting" << endl;
		return;
	}

	//cr�e une socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket ! Quitting" << endl;
		return;
	}

	//Bind une adresse ip et un numero de port � une socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // Could also use inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Dire a winsock que la socket est faite pour �couter
	listen(listening, SOMAXCONN);

	//Attendre une connection
	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];		//Client's remote name
	char service[NI_MAXSERV];	//Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); //Same as a memset(host,0,NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host <<  " connect on port " << service << endl;
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connect on port" << ntohs(client.sin_port) << endl;
	}

	//Fermer la socket d'ecoute
	closesocket(listening);

	//boucle while : accepter et echo le message au client
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		//Attendre que le client envoie des donn�es
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
				cerr << "Error in recv(). Quitting" << endl;
		break;
	}

		if(bytesReceived == 0)
		{ 
			cout << "Client disconnected " << endl;
			break;
		}

		// Echo le message au client
		send(clientSocket, buf, bytesReceived + 1, 0);

	}

	//supprimer une socket
	closesocket(clientSocket);

	//fermer winsock
	WSACleanup();
}