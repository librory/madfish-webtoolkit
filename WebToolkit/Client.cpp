#include "Common.h"
#include "Client.h"
#include "Server.h"

Client::Client(Socket* socket)
{
	this->socket=socket;
}

Client::~Client()
{
	delete socket;
}

void Client::Run()
{
	try
	{
		HttpRequest request;
		request.clientIP=socket->remoteIP;
		request.clientPort=socket->remotePort;
		string st;
		for(;;)
		{
			st=socket->ReadLine();
			if(st.empty())
				break;
			request.ParseLine(st);
		}
		if(request.postContentLength!=0)
		{
			request.postContent=socket->BufferedRead(request.postContentLength);
			request.ParseParameters(request.postContent);
		}
		ostringstream r;
		r<<socket->remoteIP<<(request.isPost?" POST ":" GET ")<<request.resource;
		Server::Instance().LogWrite(LogInfo,r.str());
		Server::Instance().Handle(&request,this);
		Send();
	}
	catch(exception& e)
	{
		Server::Instance().LogWrite(LogError,e.what());
	}
}

void Client::DirectSend(const char* buf,int len)
{
	socket->Write(buf,len);
}