#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <winsock.h>
#include <stdlib.h>
#include <Windows.h>
#include <process.h>

#pragma comment(lib,"ws2_32.lib")



int isspaceline(char* test)
{
	int i = 1;
	while (*test != '\n')
	{
		if (*test != ' ')
		{
			i = 0;
			break;
		}
		test++;
	}
	return i;
}

UINT ClientThread(LPVOID param)
{
	SOCKET* sock = (SOCKET*)param;
	FILE* fp;
	char rev[1024] = { 0 };
	char sel[1024] = { 0 };
	char num[1024] = { 0 };
	char* str;
	int len;
	while ((len = recv(*sock, rev, 1024,0)) >= 0)
	{
		if (strcmp(rev, "sign") == 0)
		{
			recv(*sock, rev, 1024, 0);
			int flag = 0;
			fp = fopen("database.txt", "r+");
			while (fgets(sel, 1024, fp) &&(strcmp(sel,"")!=0))
			{
				str = strtok(sel, "\t\n");
				if (strcmp(rev, str) == 0)
				{
					flag = 1;
					break;
				}
			}
			if (flag)
			{
				send(*sock, "用户名已被注册", 1024, 0);
				continue;
			}
			else
			{
				send(*sock, "1", 1024, 0);
				fclose(fp);
				recv(*sock, num, 1024, 0);
				char num2[1024];
				recv(*sock, num2, 1024, 0);
				if (strcmp(num, num2) == 0)
				{
					fp = fopen("database.txt", "a+");
					fprintf(fp, "%s\t%s\n", rev, num);
					send(*sock, "注册成功", 1024, 0);
					fclose(fp);
				}
				else
				{
					send(*sock, "密码错误", 1024, 0);
				}
			}
		}
		else if (strcmp(rev, "login") == 0)
		{
			recv(*sock, rev, 1024, 0);
			int flag = 0;
			fp = fopen("database.txt", "r+");
			while (fgets(sel, 1024, fp) || (strcmp(sel, "") != 0))
			{
				str = strtok(sel, "\t\n");
				if (strcmp(rev, str) == 0)
				{
					flag = 1;
					break;
				}
			}
			if (!flag)
			{
				send(*sock, "用户名错误", 1024, 0);
				fclose(fp);
				continue;
			}
			else
			{
				send(*sock, "1", 1024, 0);
				str = strtok(NULL, "\t\n");
				recv(*sock, rev, 1024, 0);
				if (strcmp(rev, str) == 0)
				{
					send(*sock, "登陆成功", 1024, 0);
					fclose(fp);
					closesocket(*sock);
					break;
				}
				else
				{
					send(*sock, "密码错误", 1024, 0);
					fclose(fp);
					continue;
				}
			}
		}
	}
	printf("%d", WSAGetLastError());
	return 0;
}

int main()
{
	WSADATA wsa;
	SOCKET serversock;
	SOCKET clientsock;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	serversock = socket(PF_INET, SOCK_STREAM, 0);
	if (serversock == INVALID_SOCKET)
	{
		printf("套接字创建失败");
		system("pause");
		return 1;
	}
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = PF_INET;
	serveraddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serveraddr.sin_port = htons(1234);
	int flag = bind(serversock, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr));
	if (flag < 0)
	{
		printf("绑定失败");
		system("pause");
		return 1;
	}
	flag = listen(serversock, SOMAXCONN);
	if (flag < 0)
	{
		printf("监听失败");
		system("pause");
		return 1;
	}
	while (1)
	{
		memset(&clientaddr, 0, sizeof(clientaddr));
		int i = sizeof(clientaddr);
		clientsock = accept(serversock, (struct sockaddr *)&clientaddr, &i);
		if (clientsock == INVALID_SOCKET)
		{
			printf("接受失败！");
			system("pause");
			return 0;
		}
		_beginthreadex(0, 0, (unsigned int(__stdcall *)(void *))ClientThread, &clientsock, 0, 0);
	}
	closesocket(serversock);
}
