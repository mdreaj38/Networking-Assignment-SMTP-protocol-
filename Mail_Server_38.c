#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<time.h>
#include<arpa/inet.h>
int main()
{
    FILE *file;
    int port=0;
    while(1)
    {
        struct sockaddr_in address;
        int server_fd, new_socket, val,addrlen = sizeof(address);
        char buffer[5000] = {0};
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)//Creating socket file descriptor
        {
            perror("socket failed");
            return 0;
        }
        port=1000;
        int opt=1;
        while(1)
        {
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) //Forcefully attaching socket to the port
            {
                perror("setsockopt");
                return 0;
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons( port );
            if(bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) port++;
            else
            {
                printf("Free Port: %d\n", port);
                FILE *tfile = fopen("port.txt","w");
                if(tfile!=NULL)
                {
                    fprintf(tfile,"%d",port);
                    fclose(tfile);
                }
                break;
            }
        }
        if (listen(server_fd, 3) < 0) puts("not listened"),exit(0);

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) puts("not accepted"),exit(0);
        send(new_socket, "200", 3, 0);

        val = read( new_socket, buffer, 5000);
        int i;
        if(strcmp(buffer,"HELLO") != 0)
        {
            send(new_socket, "400 not HELO", 12, 0);
            return 0;
        }
        else send(new_socket, "200", 3, 0);
        fflush(stdout);
        val = read( new_socket, buffer, 5000);
        for(i=0; i<9 && buffer[i]!='\0'; i++);
        buffer[i]='\0';
        if(strcmp(buffer,"MAIL FROM") !=0)
        {
            send(new_socket, "400 not MAIL FROM", 17, 0);
            return 0;
        }

        send(new_socket, "200", 3, 0);
        fflush(stdout);
        char rcpt[5000];
        val = read( new_socket, buffer, 5000);
        for(i=0; i<7 && buffer[i]!='\0'; i++) rcpt[i] = buffer[i];//store the "RCPT TO " into rcpt array
        rcpt[i] = '\0';
        if(strcmp(rcpt,"RCPT TO") != 0)
        {
            send(new_socket, "400 not RCPT TO", 15, 0);
            return 0;
        }
        for(i=0; buffer[8+i]!='\0' ; i++) rcpt[i] = buffer[8+i]; // store the recpt name to write his/her file
        rcpt[i] = '\0';
        strcat(rcpt,".txt");
        if(access(rcpt,F_OK) == -1)
        {
            send(new_socket, "400 no recipient", 17, 0);
            return 0;
        }

        file = fopen(rcpt,"a");
        send(new_socket, "200", 3, 0);

        fflush(stdout);
        val = read( new_socket, buffer, 5000);
        for(i=0; buffer[i]!=' ' && buffer[i]!='\0'; i++);
        buffer[i]='\0';
        printf("%s %d\n",buffer,(int)strlen(buffer));
        if(strcmp(buffer,"DATA") != 0)
        {
            send(new_socket, "400 not DATA", 12, 0);
            return 0;
        }

        send(new_socket, "200", 3, 0);
        fflush(stdout);
        val = read( new_socket, buffer, 5000);
        fputs(buffer,file);
        send(new_socket, "200", 3, 0);
        fputs(" ",file);
        fclose(file);
        val = read(new_socket, buffer, 5000);
        for(i=0; i<4; i++);
        buffer[i]='\0';
        if(strcmp(buffer,"QUIT") != 0)
        {
            send(new_socket, "400 not QUIT", 12, 0);
            return 0;
        }
        else send(new_socket, "200", 3, 0);
        fflush(stdout);
    }
}

