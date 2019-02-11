#include "MainServerSocket.h"
#include<thread>

#define DEFAULT_BUFLEN 512

MainServer* MainServer::m_MainServer = NULL;

/*--------------------------------------------------------------------
** ���� : DetectSubServerHeart
**--------------------------------------------------------------------
** ���� : ����ӷ���������
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.07		��ΰ
**-------------------------------------------------------------------*/
void MainServer::DetectSubServerHeart()
{
	while (1)
	{
		Sleep(5000);
		DetectSubServer();
	}
}

/*--------------------------------------------------------------------
** ���� : MainServer
**--------------------------------------------------------------------
** ���� : ��ʼ��
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.06		��ΰ
**-------------------------------------------------------------------*/
MainServer::MainServer()
{
	m_MainServer = this;
}
/*--------------------------------------------------------------------
** ���� : ~MainServer
**--------------------------------------------------------------------
** ���� : �ͷ�
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.06		��ΰ
**-------------------------------------------------------------------*/
MainServer::~MainServer()
{

}
/*--------------------------------------------------------------------
** ���� : StartNetService
**--------------------------------------------------------------------
** ���� : �����������
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.06		��ΰ
**-------------------------------------------------------------------*/
bool MainServer::StartNetService()
{
	// �����׽���
	int iListenSocket = CreateSocket();

	if(INVALID_SOCKET == iListenSocket)
	{
		return false;
	}

	// ���׽���
	bool bResult = BindSocket(iListenSocket);
	if(!bResult)
	{
		return false;
	}

	// ���ü���
	bResult = ListenSocket(iListenSocket);
	if(!bResult)
	{
		return false;
	}

	/*�����߳��д�������*/
	std::thread t_detectSubServer(DetectSubServerHeart);
	//tSocket.join();
	while(1)
	{
		AcceptSocket(iListenSocket);
		Sleep(5000);
		std::cout << "good Accept"<<std::endl;
	}

	t_detectSubServer.join();
	return true;
}
/*--------------------------------------------------------------------
** ���� : CreateSocket
**--------------------------------------------------------------------
** ���� : �����׽���
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.01.31		��ΰ
**-------------------------------------------------------------------*/
int MainServer::CreateSocket()
{
	//��ʼ��DLL
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if(NO_ERROR != iResult)
	{
		return 0;
	}

	//�����׽���
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == ListenSocket)
	{
		std::cout << "Create Socket Error!" << std::endl;

		WSACleanup();

		return 0;
	}
	else
	{
		std::cout << "Create Socket Success" << std::endl;
	}
	
	return ListenSocket;
}
/*--------------------------------------------------------------------
** ���� : BindSocket
**--------------------------------------------------------------------
** ���� : ���׽���
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.01.31		��ΰ
**-------------------------------------------------------------------*/
bool MainServer::BindSocket(int iListenSocket)
{
	//----------------------
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	// The socket address to be passed to bind
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(27015);

	//----------------------
	// Bind the socket.
	int iResult = bind(iListenSocket, (SOCKADDR *)&service, sizeof(service));
	if(INVALID_SOCKET == iResult)
	{
		WSACleanup();

		return false;
	}
	else
	{
		std::cout << "Bind Socket Success"<< std::endl;
	}
	return true;

}
/*--------------------------------------------------------------------
** ���� : ListenSocket
**--------------------------------------------------------------------
** ���� : �����׽���
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.01.31		��ΰ
**-------------------------------------------------------------------*/
bool MainServer::ListenSocket(int iListenSocket)
{
	if(INVALID_SOCKET == iListenSocket)
	{
		return false;
	}

	int iRet = listen(iListenSocket, SOMAXCONN);
	if(0 != iRet)
	{
		return false;
	}
	else
	{
		std::cout << "Listen Socket Success" << std::endl;
	}
	return true;
}
/*--------------------------------------------------------------------
** ���� : AcceptSocket
**--------------------------------------------------------------------
** ���� : ���ܿͻ�������
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.01.31		��ΰ
**-------------------------------------------------------------------*/
bool MainServer::AcceptSocket(int iListenSocket)
{
	if(INVALID_SOCKET == iListenSocket)
	{
		return false;
	}

	SOCKET iAcceptSocket;

	iAcceptSocket = accept(iListenSocket, NULL, NULL);
	if(INVALID_SOCKET != iAcceptSocket)
	{
		m_vecSubServerSocket.push_back(iAcceptSocket);

		std::cout <<"Accept SubServer Socket" << iAcceptSocket << std::endl;
	}

	return true;
}
/*--------------------------------------------------------------------
** ���� : DetectSubServer
**--------------------------------------------------------------------
** ���� : ������߳�����
**--------------------------------------------------------------------
** ���� : NULL
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.07		��ΰ
**-------------------------------------------------------------------*/
void MainServer::DetectSubServer()
{
	VecSubServerSocket::iterator itSubServer = m_MainServer->m_vecSubServerSocket.begin();
	if(itSubServer == m_MainServer->m_vecSubServerSocket.end())
	{
		return;
	}
	for(;itSubServer != m_MainServer->m_vecSubServerSocket.end();itSubServer++)
	{
		//*itSubServer
		int iSubSocket = *itSubServer;
		DetectSubServerConnect(iSubSocket);
	}
}
/*--------------------------------------------------------------------
** ���� : DetectSubServerConnect
**--------------------------------------------------------------------
** ���� : ������߳���������
**--------------------------------------------------------------------
** ���� : iAcceptSocket ���߳��׽���
** ��ֵ : NULL
**--------------------------------------------------------------------
** Date:		Name
** 19.02.07		��ΰ
**-------------------------------------------------------------------*/
void MainServer::DetectSubServerConnect(int iAcceptSocket)
{
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN] = "";
	int iResult = recv(iAcceptSocket, recvbuf, recvbuflen, 0);
	if(iResult > 0)
	{
		wprintf(L"Bytes received: %d\n", iResult);
		std::cout <<"[AcceptSocket]"<< iAcceptSocket << "Reiceived Value is " << recvbuf << " Length is  " << recvbuflen << std::endl;
	}
	else if(0 == iResult)
	{
		wprintf(L"Connection closed\n");
		closesocket(iAcceptSocket);
	}
	else
	{
		wprintf(L"recv failed with error: %d\n", WSAGetLastError());
		closesocket(iAcceptSocket);
	}
	std::cout << "Accept Success" << std::endl;
}