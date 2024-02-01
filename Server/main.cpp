#include "Protocol.h"
#include "Network.h"

int main()
{
	
	Network& server = Network::GetInstance();
	
	server.run();

}
