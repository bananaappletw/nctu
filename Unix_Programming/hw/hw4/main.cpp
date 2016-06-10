#include<iostream>
#include<string>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include "simple.h"

using namespace std;

int main(int argc, char* argv[])
{
    string dir_root;
    int listen_fd, client_fd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    long int port = 8888;
    int flag = 1;
    int flag_len = sizeof(flag);
    if (argc != 3)
    {
        cout << "Usage: ./hw4 [port] [directory]" << endl;
        return 1;
    }
    if (!(port = strtol(argv[1], NULL, 10)))
    {
        cout << "[Error]: wrong port number" << endl;
        return 1;
    }
    if (!is_dir(string(argv[2])))
    {
        cout << "[Error]: Directory not exist" << endl;
        return 1;
    }
    dir_root = string(argv[2]);
    if (*dir_root.rbegin() == '/')
        dir_root = dir_root.substr(0, dir_root.length() - 1);
    // Initialize server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "[Error]: Create socket error" << endl;
        return 1;
    }
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &flag, flag_len) == -1)
    {
        cout << "[Error]: setsockopt error" << endl;
    }
    if (bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        cout << "[Error]: Bind error" << endl;
        return 0;
    }
    if (listen(listen_fd, 10) == -1)
    {
        cout << "[Error]: Bind error" << endl;
        return 0;
    }
    for (;;)
    {
        cliaddr_len = sizeof(cliaddr);
        client_fd = accept(listen_fd, (struct sockaddr*)&cliaddr, &cliaddr_len);
        pid_t pid;
        if ((pid = fork()) < 0)
        {
            cout << "[Error]: Fork error." << endl;
        }
        else if (pid == 0)
        {
            close(listen_fd);
            serve(client_fd, dir_root);
            return 1;
        }
        else
        {
            close(client_fd);
            continue;
        }
    }
    return 0;
}
