#include <iostream>
#include <winsock.h>
#include <bits/stdc++.h>
using namespace std;
#include <fstream>

int port=901;
int clientsocketnum;
struct sockaddr_in server;

int main() {
    WSADATA windows;
    int nRet=0;

    if(WSAStartup(MAKEWORD(2,2),&windows)<0)
    {
        cout<<"failed\n";

    }
    else{
        cout<<"windows initialized\n";
    }
    clientsocketnum= socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(clientsocketnum<0)
    {
        cout<<"socket not open\n";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else{
        cout <<"socket opened\n";
    }
    server.sin_family=AF_INET;
    server.sin_port= htons(port);

//    my system ip addr
    server.sin_addr.s_addr= inet_addr("192.168.1.6");

    memset(&(server.sin_zero),0,8);
    nRet= connect(clientsocketnum,(struct sockaddr*) & server ,sizeof (server));

    if(nRet<0)
    {
        cout<<"failed to connect";
        WSACleanup();
        return (EXIT_FAILURE);
    }
    else
    {
        cout<<"successfully connected to Server  \n";
        char buffer[20000];

        recv(clientsocketnum,buffer,20000,0);
        cout <<"send message to the server\n";

// send requests to server
        while (1)
        {

// know if you want to send get or post  if get read from file the file pth in server and send the request
// to server

cout<<"if you want get request press 1 else press 2";
int count;
cout<<"\n";
            scanf("%d",&count);


            if(count==1)
            {

                std::ifstream file("input2.txt");
                std::string str;
                std::string file_contents;
                while (std::getline(file, str)) {
                    file_contents += str;
                    file_contents.push_back('\n');
                }
                strcpy(buffer, file_contents.c_str());
            }
            if(count==2) {
                std::ifstream file("input.txt");
                std::string str;
                std::string file_contents2;

                while (std::getline(file, str)) {
                    file_contents2 += str;
                    file_contents2.push_back('\n');
                }
                strcpy(buffer, file_contents2.c_str());



            }
            send(clientsocketnum,buffer,20000,0);




// get request from server


            cout<<"press key to get response from server\n";
            getchar();
            recv(clientsocketnum,buffer,20000,0);
            cout<<buffer<<"now send the new message\n";

            char hostname[32]={0,};
            nRet= gethostname(hostname,32);






        }


    }
    return 0;
}
