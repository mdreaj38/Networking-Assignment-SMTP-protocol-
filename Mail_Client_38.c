#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

int port;
char File_[5000],dust[5000],exact[4096],Sub[5000],adrs[5000],rcpt[5000];

int main()
{
    scanf("%[^\n]s",exact);
    int l = strlen(exact),id = 0, id_temp = 0;
    while(exact[id] != ' ' && id<l)
        dust[id_temp++] = exact[id++];

    if(id == l)
        printf("Invalid Number of parameters\n"),exit(0);
    else id++;
    dust[id_temp] = '\0';
    id_temp = 0;
    while(exact[id] != ' ' && id<l)
        adrs[id_temp++] = exact[id++];//store the address
    if(id == l)
        printf("Invalid Number of parameters\n"),exit(0);
    else id++;
    adrs[id_temp] = '\0';
    id_temp = 0;
    if(exact[id]!='"')
        printf("Invalid format of parameter\n"),exit(0);

    else id++;

    while(exact[id] != '"' && id<l)
        Sub[id_temp++] = exact[id++];
    if(id == l)
        printf("Invalid format of parameters\n"),exit(0);
    else id+=2;
    Sub[id_temp] = '\0';

    id_temp = 0;

    while(id<l)
        File_[id_temp++] = exact[id++];
    if(access(File_,F_OK)==-1)
    {
        printf("Invalid Filename\n");
        return 0;
    }
    char portChar[10];
    int adrsLen = strlen(adrs);
    adrsLen--;
    id_temp=0;
    while(adrs[adrsLen]!=':')
        portChar[id_temp++] = adrs[adrsLen--];//store the port
    portChar[id_temp]='\0';
    for(int i=0; i<id_temp/2; i++)
    {
        char temp = portChar[i];
        portChar[i] = portChar[id_temp -i -1];
        portChar[id_temp -i -1] = temp;
    }
    port = 0;
    for(int i=0; i<id_temp; i++)
        port = port*10 + (portChar[i]-'0');
    int i;
    for(i=0; adrs[i]!='@' ; i++)
        rcpt[i] = adrs[i];
    rcpt[i] = '\0'; /****Reciepient to */
    struct sockaddr_in address;
    int sock = 0, VAL;
    struct sockaddr_in serv_addr;
    char buffer[10024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return 0;
    }
    FILE *file2 = fopen("port.txt","r");
    fscanf(file2,"%d",&port);
    fclose(file2);
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return 0;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return 0;
    }
    // --- Connection Established
    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
    {
        printf("Error establishing connection.\n");
        return 0;
    }
    printf("200\n");
    char HELLO[100];
    scanf("%s",HELLO);
    send(sock, HELLO, strlen(HELLO), 0 );/**** HELLO*/
    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
        printf("%s\n",buffer),exit(0);
    printf("200\n");
    fflush(stdout);
    char Mail_from_USER[100];
    scanf(" %[^\n]",Mail_from_USER);
    send(sock, Mail_from_USER, strlen(Mail_from_USER), 0);

    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
        printf("%s\n",buffer),exit(0);
    printf("200\n");
    fflush(stdout);
    char toBeSent[100]; /****RCPT TO*/
    scanf(" %[^\n]",toBeSent);
    strcat(toBeSent,rcpt);
    send(sock, toBeSent, strlen(toBeSent), 0 );////
    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
        printf("%s\n",buffer),exit(0);
    printf("200\n");
    fflush(stdout);
    char DATA[100];
    scanf(" %[^\n]",DATA);/** DATA*/
    send(sock, DATA, strlen(DATA), 0 );
    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
        printf("%s\n",buffer),exit(0);
    else printf("200\n");
    fflush(stdout);
    FILE *rFile = fopen(File_,"r");
    char Final[10024],temp[5000];
    char ch = fgetc(rFile);
    i=0;
    while(ch!='.')
        temp[i++]=ch,ch=fgetc(rFile);
    temp[i]='\0';
    strcat(Final,"Sub: "); //Ready the mail with body and subject to write/send to server-> next 8 lines
    strcat(Final,Sub);
    strcat(Final,"\n\n");
    strcat(Final,temp);
    strcat(Final,"\n");
    strcat(Final,"\n");
    strcat(Final,"                              <__END__>                  \n");
    strcat(Final,"\n");
    send(sock, Final, strlen(Final), 0 );
    VAL = read(sock, buffer, 10024);
    if(buffer[0]=='2') fclose(rFile);
    else if(buffer[0] !='2' && buffer[0]!='3')
    {
        printf("%s\n",buffer);
        return 0;
    }
    fflush(stdout);
    char QUIT[100];
    scanf(" %[^\n]",QUIT);/** QUIT*/
    send(sock, QUIT, strlen(QUIT), 0 );
    VAL = read(sock, buffer, 10024);
    if(buffer[0]!='2')
    {
        printf("%s\n",buffer);
        return 0;
    }
    fflush(stdout);
}
//nothng reaj@host:port "subject" store.txt
//"HELLO"
//MAIL FROM"
//"RCPT TO "
//"DATA"
//"QUIT"

