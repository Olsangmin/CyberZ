#include "Protocol.h"
#include "Server.h"

int main()
{
	Server& server = Server::GetInstance();
	
	server.Network();

}
