#include <iostream>
#include <winsock.h>
#include <fstream>
#include <thread>

using std::string;

using namespace std;
struct sockaddr_in server;


fd_set fr,fw,ferr; // file read,file write,file error
int port=901;
int socketnum;
int arrclient[5];
string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}
void processnewmessgs(int i) {
    char buffer[20000]={0,};

    int nRet=  ( recv(i,buffer,20000,0));
    if(nRet<0)
    {
        cout<<"ERROR";
        closesocket(i);
        for(int j=0;j<5;j++)
        {
            if(arrclient[j]==i)
            {
                arrclient[j]=0;
                break;
            }
        }
    } else{

//parsing the request from the client and put output in array
        string array[300];
        int j=0;
        for(int i=0;i<lstrlenA(buffer);i++) {

            while ((buffer[i] != ' ') && i < lstrlenA(buffer) - 1) {
                if (buffer[i] == '_') {
                    i++;
                    j++;
                }
                array[j] = array[j] + buffer[i];
                i++;

            }
            j++;
        }

        // check if coming request get or post

        // if get it will take the data from file in server nd send it to client

        if(array[0].compare("GET")==0)
        {

            char a[1000];
            strcpy(a, array[1].c_str());
for(int i=0;i<lstrlenA(a);i++)
{
    a[i]=a[i+1];
}

            string q = convertToString(a, lstrlenA(a));
            ifstream ifile;
            if(ifile) {
                std::ifstream file(q+".txt");
                std::string str;
                std::string file_contents;
                while (std::getline(file, str))
                {
                    file_contents += str;
                    file_contents.push_back('\n');
                }
                strcpy(buffer, file_contents.c_str());
            }
            // if the file not found in server
            else {

                strcpy(buffer, "HTTP/1.1 404 Not Found\n");
            }

            send(i,buffer,lstrlenA(buffer)+1,0);
        }
        // if post it take data from client and put it in file in server
        if(array[0].compare("POST")==0)
        {

            char a[1000];
            strcpy(a, "HTTP/1.1 200 OK\nwait the file to be uploaded to the server\n");
            send(i,a,lstrlenA(a)+1,0);
            char b[1000];
            char m[1000];
            strcpy(b, array[1].c_str());

        int i=1;
        while(b[i]!='/'&&i<lstrlenA(b)-1)
        {
            m[i-1]=b[i];
            i++;
        }


            string q = convertToString(m, lstrlenA(m));

            ofstream myfile;
            myfile.open (q+".txt");

            myfile.write((char *)buffer,lstrlenA(buffer)+1);
            myfile.close();


        }




    }
}
void newrequests()
{
    //new coneection request
    if(FD_ISSET(socketnum,&fr))
    {
        int a=sizeof (struct sockaddr);
        int clientsocketnum= accept(socketnum,NULL,&a);

        if(clientsocketnum>0)
        {
            int i;
            for( i=0;i<5;i++)
            {
                if(arrclient[i]==0)
                {
                    arrclient[i]=clientsocketnum;
                    send(clientsocketnum,"Got the connection successfully",255,0);
                    break;
                }
            }
            if(i==5)
            {


            }

        }

    } else{
        for(int i=0;i<5;i++)
        {
            if(FD_ISSET(arrclient[i],&fr))
            {
                // get new messages from client
                thread t1(processnewmessgs,arrclient[i]);
                t1.join();
            }
        }
    }

}



int main() {
    WSADATA windows;
    int nRet;
    if(WSAStartup(MAKEWORD(2,2),&windows)<0)
    {
        cout<<"failed\n";

    }
    else{
        cout<<"windows initialized\n";
    }
    socketnum= socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(socketnum<0)
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
    server.sin_addr.s_addr=INADDR_ANY;
    memset(&(server.sin_zero),0,8);
    //set sockopt
    int optval=0;


    nRet= setsockopt(socketnum,SOL_SOCKET,SO_REUSEADDR,(const char *)&optval,sizeof (optval));
    if(nRet==0)
    {
        cout<<"set system call successful\n";
    } else{
        cout<<"failed\n";
        WSACleanup();
        exit(EXIT_FAILURE);

    }
//bind socket to local num
    int nmax=socketnum;

    nRet= bind(socketnum,(sockaddr*)&server,sizeof (sockaddr));

    if(nRet<0)
    {
        cout<<"Not Binded to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout<<"successfully Binded \n";

    }
    // start take connections
    nRet= listen(socketnum,5); //5 requests in the queue
    if(nRet<0)
    {
        cout<<"no listen to local port";
        WSACleanup();
        exit(EXIT_FAILURE);

    } else
    {
        cout<<"start listening\n";
    }
    struct  timeval time;
    time.tv_sec=2;
    time.tv_usec=0;
    while (true) {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&ferr);


        FD_SET (socketnum, &fr);
        FD_SET (socketnum, &ferr);
        for(int i=0;i<5;i++)
        {
            if(arrclient[i]!=0)
            {
                FD_SET (arrclient[i],&fr);
                FD_SET (arrclient[i],&ferr);



            }
        }






// taking new requests  and proceed it
//time =time to wait wheter fr,fw,ferr ready or not

        nRet = select(nmax + 1, &fr, &fw, &ferr, &time);


        if (nRet > 0) {
            cout<<"data is processing\n";
            thread t(newrequests);
            t.join();


        } else if (nRet == 0) {

        } else {

            WSACleanup();
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
