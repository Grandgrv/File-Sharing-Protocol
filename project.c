//Amit Kumar Jha 201201087
//Ajitesh Gupta 201201068
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
int fd[2];
void cFiledownload(char write_buffer[],int *sockfd);
void Filehash(char read_buffer[] , int *newsock_fd);
void Filedownload(char read_buffer[] , int *newsock_fd);
void FileUpload(char read_buffer[],int *newsockfd);
void cFileUpload(char write_buffer[],int *newsockfd);
int choice;
void server();
void client();
int serPort,clPort;
char clIp[100];
char recv_command[1000][1000];
char send_command[1000][1000];
pid_t pid;
typedef struct file
{
    char name[1024];
    char time[1024];
    int size;
    char type[200];
}file;

file ls[1000];

void cIndexget(char buff[],int *sock)
{
    int n;
    /*char l[8]="LongList";
    char r[5]="RegEx";
    char s[9]="ShortList";
    char read_buff[1024];
    int f=0,i=0;
    if(strcmp(send_command[1],l)==0)
        f=0;
    if(strcmp(send_command[1],r)==0)
        f=1;
    if(strcmp(send_command[1],s)==0)
        f=2;*/
    char read_buff[1024];
    bzero(read_buff, 1024);
    while((n=read(*sock,read_buff,1024))>0)
    {
        read_buff[n]='\0';
        if(strcmp(read_buff,"END")==0)
            break;
        printf("%s",read_buff);
    }
    printf("\n");
}

void sIndexget(char read_buffer[],int *sock)
{
    char str1[1024],str2[1024];
    char l[8]="LongList";
    char r[5]="RegEx";
    char s[9]="ShortList";
    char write_buff[1024];
    int f=0,i=0;
    if(strstr(read_buffer,l)!=NULL)
        f=0;
    if(strstr(read_buffer,r)!=NULL)
        f=1;
    if(strstr(read_buffer,s)!=NULL)
        f=2;
    FILE *fs;
    bzero(write_buff,1024);
    if(f==0)
    {
        bzero(write_buff,1024);
        system("touch Result");
        system("ls -l  > Result");
        fs = fopen("Result", "r");
    }
    if(f==1)
    {
        bzero(str1,1024);
        bzero(str2,1024);
        int k=0;
        for(i=15,k=0;read_buffer[i]!='\0';i++,k++)
            str1[k]=read_buffer[i];
        str1[k]='\0';
        system("touch Result");
        strcat(str2,"ls -l | grep ");
        strcat(str2,str1);
        strcat(str2," > Result");
        system(str2);
        //      printf("%s",str2);
        fs = fopen("Result", "r");
    }
    if(f==2)
    {
        bzero(str1,1024);
        bzero(str2,1024);
        int k=0;
        i=19;
        while(i<38)
            str1[k++]=read_buffer[i++];
        str1[k]='\0';
        i++;
        k=0;
        while(i<58)
            str2[k++]=read_buffer[i++];
        str2[k]='\0';
        system("touch Result");
        system("touch fd");
        system("ls --full-time > fd");
        FILE *file = fopen ( "fd", "r" );
        FILE *file1 = fopen("Result","w");
        if (file != NULL)
        {
            char line [1024];
            while ( fgets ( line, 1024, file ) != NULL )
            {
                char curr_time[1024];
                for(i=32;i<51;i++)
                    curr_time[i-32] = line[i];
                curr_time[i-32] = '\0';
                if( strcmp(str2,curr_time)>=0 && strcmp(curr_time,str1) >= 0 )
                {
                    printf("%s\n",line);
                    fputs(line,file1);
                }
            }
            fclose ( file );
            fclose ( file1 );
        }
        else
        {
            perror("Error in file opening");
            exit(0);
        }
        fs =fopen("Result","r");
    }
    if(fs==NULL)
        error("ERROR: File not found");
    int fi=0,flag=0;
    bzero(write_buff, 1024);
    while((fi= fread(write_buff, sizeof(char),1024, fs)) > 0)
    {
        //    printf("%s\n",write_buff);
        if(write(*sock, write_buff, 1024) < 0)
            error("\nERROR: Writing to socket");
        bzero(write_buff, 1024);
        flag=1;
    }
    if(flag==1)
    {
        bzero(write_buff, 1024);
        strcpy(write_buff,"END");
        if(write(*sock, write_buff, 1024) < 0)
            error("\nERROR: Writing to socket");
    }
    system("rm -rf Result");
    if(f==2)
        system("rm -rf fd");
}

int main(int argc,char *argv[])
{
    pipe(fd);
    printf("Enter Port Number on which you want to listen\n");
    scanf("%d",&serPort);
    printf("0->TCP Mode\n1->UDP Mode\n");
    scanf("%d",&choice);
    //printf("Enter IP address on which you want to send the data\n");
    //scanf("%s",clIp);
    printf("Enter Port Number on which you want to send the data\n");
    scanf("%d",&clPort);
    pid=fork();
    if(pid==-1)
    {
        printf("Error:Creating Fork\n");
        exit(0);
    }
    else if(pid==0)
    {
        close(fd[0]);
        client();
    }
    else
    {
        close(fd[1]);
        server();
    }
    return 0;
}

void server()
{
    int sockfd, newsockfd;
    socklen_t cli_len;
    char read_buffer[1024];
    struct sockaddr_in servi_addr, cli_addr;
    int n,i;

    if(choice==0)
    {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
    }
    else
    {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
    }
    bzero((char *) &servi_addr, sizeof(servi_addr));
    servi_addr.sin_family = AF_INET;
    servi_addr.sin_addr.s_addr = INADDR_ANY;
    servi_addr.sin_port = htons(serPort);


    if (bind(sockfd, (struct sockaddr *)&servi_addr, sizeof(servi_addr))
            <0) {
        error("Unable to bind");
    }
    else
        printf("[Server] Socket Binded to the Server Address\n");
    if(choice==0)
    {
        if (listen(sockfd, 5) == -1) {
            perror("Listen");
            exit(1);
        }
        printf("\nTCPServer Waiting for client on port %d",serPort);
        fflush(stdout);
    }
    else
    {
        printf("\nUDPServer Waiting for client on port %d",serPort);
        fflush(stdout);
    }
    //  while(1)
    // {
    cli_len = sizeof(cli_addr);

    if(choice==0)
    {
        newsockfd = accept(sockfd,  (struct sockaddr *) &cli_addr, &cli_len);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        printf("\nServer:I got a connection from (%s , %d)\n",inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
        fflush(stdout);
    }
    while (1)
    {
        bzero(read_buffer,1024);
        n= read(newsockfd, read_buffer , 1024);
        if (n < 0)
            error("ERROR writing to socket");
        //            read_buffer[n]='\0';
        int n;
        int i;
        int c=0;
        int j=0;
        for(i=0;i<strlen(read_buffer);i++)
        {
            if(read_buffer[i] == ' ')
            {
                recv_command[c][j]='\0';
                j=0;
                c++;
            }
            else
                recv_command[c][j++]=read_buffer[i];
        }
        recv_command[c][j]='\0';
        if(strcmp(read_buffer, "q")==0 || strcmp(read_buffer, "Q")==0)
        {
            printf("\nRecieved Data: %s\n",read_buffer);
            kill(pid,SIGTERM);
            break;
        }
        else if(strncmp(read_buffer , "IndexGet",8)==0){
            printf("\nRecieved Data: %s\n",read_buffer);
            sIndexget(read_buffer,&newsockfd);
        }
        else if(strncmp(read_buffer,"FileHash",8)==0)
        {
            printf("\nRecieved Data: %s\n",read_buffer);
            Filehash(read_buffer,&newsockfd);
        }
        else if(strncmp(read_buffer,"FileDownload",12)==0)
        {
            printf("\nRecieved Data: %s\n",read_buffer);
            Filedownload(read_buffer,&newsockfd);
        }
        else if(strncmp(read_buffer,"FileUpload",10)==0)
        {
            printf("\nRecieved Data: %s\n",read_buffer);
            FileUpload(read_buffer,&newsockfd);
        }
        else
        {
            printf("\nRecieved Data : %s\n" , read_buffer);
        }
        fflush(stdout);
        //      while(waitpid(-1, NULL, WNOHANG) > 0);
    }
    close(newsockfd);
    printf("\nConnection closed by peer\n");
    fflush(stdout);
    // }
    close(sockfd);
    exit(1);
}
void client()
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char read_buffer[1024],write_buffer[1024];

    if(choice==0)
    {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
            perror("Socket");
            exit(1);
        }
    }
    else
    {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("Socket");
            exit(1);
        }
    }
    printf("\n[Client] Socket created\n");

    server = gethostbyname("127.0.0.1");
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(clPort);
    serv_addr.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    if(choice==0)
    {
        while(connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
        {
            printf("Connecting\n");
            sleep(1);
        }
        printf("\nClient:Connected to Port No: %d\n",clPort);
    }
    while(1)
    {
        bzero(write_buffer , 1024);
        printf("\nMessage: ");
        fgets(write_buffer,1024,stdin);
        printf("\n");
        int len=strlen(write_buffer);
        if(len>0 && write_buffer[len-1]=='\n')
        {
            write_buffer[len-1]='\0';
            len--;
        }
        if(strcmp(write_buffer , "Q")==0 || strcmp(write_buffer , "q")==0)
        {
            n = write(sockfd,write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
            printf("[Client] Connection closed\n");
            kill(pid,SIGTERM);
            break;
        }
        else if(strncmp(write_buffer,"IndexGet",8)==0 || strncmp(write_buffer,"FileHash",8)==0)
        {
            n = write(sockfd,write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
            cIndexget(write_buffer,&sockfd);
        }
        else if(strncmp(write_buffer,"FileDownload",12)==0)
        {
            n = write(sockfd,write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
            cFiledownload(write_buffer,&sockfd);
        }
        else if(strncmp(write_buffer,"FileUpload",10)==0)
        {
            n = write(sockfd,write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
            cFileUpload(write_buffer,&sockfd);
        }
        /*else if(strcmp(write_buffer,"FileUploadAllow")==0)
        {
            n=write(fd[1],write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
        }
        else if(strcmp(write_buffer,"FileUploadDeny")==0)
        {
            n=write(fd[1],write_buffer,1024);
            if (n < 0)
                error("ERROR writing to socket");
        }*/
        else
        {
            if(write(sockfd,write_buffer,1024)<0)
                error("ERROR writing to socket");
        }
    }
    close(sockfd);
    exit(0);
}
void Filehash(char read_buffer[] , int *newsock_fd)
{
    int f=0;
    if(strstr(read_buffer,"Verify") != NULL )
        f = 1;
    else if(strstr(read_buffer,"CheckAll") != NULL )
        f = 2;
    if(f==1)
    {
        int i=0,k;
        int len=strlen(read_buffer);
        char file[1024];
        for(k=0,i=16;i!=len;i++,k++)
            file[k]=read_buffer[i];
        file[k]='\0';
        char md5sum[1024],query[1024],write_buffer[1024];
        bzero(write_buffer,1024);
        strcpy(md5sum,"md5sum ");
        strcat(md5sum,file);
	    system("touch tmp1");
	    system("touch tmp1");
        strcat(md5sum," > tmp1");
        system(md5sum);
        strcpy(query,"stat ");
        strcat(query,file);
        strcat(query," | grep Modify | awk '{print $3}' > tmp2");
        system(query);
        FILE *f1=fopen("tmp1","r");
        FILE *f2=fopen("tmp2","r");
        char t1[1024],t2[1024];
        fread(t1,sizeof(char),1024,f1);
        fread(t2,sizeof(char),1024,f2);
        i = 0;
        while( t1[i++] != ' ' );
        t1[i] = '\0';
        strcpy(write_buffer,file);
        strcat(write_buffer,"\t");
        strcat(write_buffer,t1);
        strcat(write_buffer,"\t");
        strcat(write_buffer,t2);
        system("rm tmp1 tmp2");
        if(write(*newsock_fd, write_buffer, 1024) < 0)
            error("\nERROR: Writing to socket");
        bzero(write_buffer,1024);
        strcpy(write_buffer,"END");
        if(write(*newsock_fd, write_buffer, 1024) < 0)
            error("\nERROR: Writing to socket");
    }
    if(f==2)
    {
        DIR *dp;
        struct dirent *ep;
        char tmp[1024];
        dp=opendir(".");
        if(dp!=NULL)
        {
            while ( (ep = readdir (dp)) != NULL)
            {
                if( ep->d_type == 8 )
                {
                    strcpy(tmp,ep->d_name);
                    char md5sum[1024],query[1024],write_buffer[1024];
                    bzero(write_buffer,1024);
                    strcpy(md5sum,"md5sum ");
                    strcat(md5sum,tmp);
                    strcat(md5sum," > tmp1");
                    system(md5sum);
                    strcpy(query,"stat ");
                    strcat(query,tmp);
                    strcat(query," | grep Modify | awk '{print $3}' > tmp2");
                    system(query);
		            system("touch tmp1");
		            system("touch tmp1");
                    FILE *f1=fopen("tmp1","r");
                    FILE *f2=fopen("tmp2","r");
                    char t1[1024],t2[1024];
                    fread(t1,sizeof(char),1024,f1);
                    fread(t2,sizeof(char),1024,f2);
                    int i = 0;
                    while( t1[i++] != ' ' );
                    t1[i] = '\0';
                    strcpy(write_buffer,tmp);
                    strcat(write_buffer,"\t");
                    strcat(write_buffer,t1);
                    strcat(write_buffer,"\t");
                    strcat(write_buffer,t2);
                    system("rm tmp1 tmp2");
                    if(write(*newsock_fd, write_buffer, 1024) < 0)
                        error("\nERROR: Writing to socket");
                    bzero(write_buffer,1024);
                    strcpy(write_buffer,"END");
                    if(write(*newsock_fd, write_buffer, 1024) < 0)
                        error("\nERROR: Writing to socket");
                }
            }
            (void )closedir(dp);
        }
        else
            error ("Couldn't open the directory");
    }
}

/*int CalcFileMD5(char *file_name, char *md5_sum)
  {
#define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
sprintf(cmd, MD5SUM_CMD_FMT, file_name);
#undef MD5SUM_CMD_FMT

FILE *p = popen(cmd, "r");
if (p == NULL) return 0;

int i, ch;
for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
 *md5_sum++ = ch;
 }

 *md5_sum = '\0';
 pclose(p);
 return i == MD5_LEN;
 }*/

void Filedownload(char read_buffer[] , int *newsock_fd)
{
    char write_buffer[1024];
    int i,n;
    char file[1024];
    for(i=13;read_buffer[i]!='\0';i++)
        file[i-13]=read_buffer[i];
    file[i-13]='\0';
    FILE *fp;
    if(fp==NULL)
    {
        if( write(*newsock_fd,"file doesn't exist",1024)<0)
            error("\nERROR: Writing to socket");
        return;
    }
    fp=fopen(file,"rb");
    bzero(write_buffer,1024);
    while((n=fread(write_buffer,1,1024,fp))>0)
    {
        if( write(*newsock_fd,write_buffer,1024)<0)
            error("\nERROR: Writing to socket");
        bzero(write_buffer,1024);
    }
    strcpy(write_buffer,"END");
    if( write(*newsock_fd,write_buffer,1024)<0)
        error("\nERROR: Writing to socket");
    fclose(fp);
}
void cFiledownload(char write_buffer[],int *sockfd)
{
    int n,i;
    char read_buff[1024];
    char file[1024];
    for(i=13;write_buffer[i]!='\0';i++)
        file[i-13]=write_buffer[i];
    file[i-13]='\0';
    FILE *fp=fopen(file,"w");
    printf("Downloading\n");
    fflush(stdout);
    bzero(read_buff, 1024);
    while((n=read(*sockfd,read_buff,1024))>0)
    {
        read_buff[n]='\0';
        if(strcmp(read_buff,"END")==0)
            break;
        fprintf(fp,"%s",read_buff);
        printf(".");
        fflush(stdout);
        bzero(read_buff, 1024);
    }
    printf("\nDownload Complete\n");
    fflush(stdout);
    fclose(fp);
}
void FileUpload(char read_buffer[],int *newsockfd)
{
    char write_buffer[1024];
    int i,n;
    char file[1024];
    for(i=11;read_buffer[i]!='\0';i++)
        file[i-11]=read_buffer[i];
    file[i-11]='\0';
    printf("Received Upload request for %s\n",file);
    printf("Allow/Deny the client to upload:\n");
    char inp[100];
    fflush(stdout);
    kill (pid,SIGSTOP);
    scanf("%s",inp);
    if(strlen(inp)>0 && inp[strlen(inp)]=='\n')
        inp[strlen(inp)]='\0';
    kill (pid,SIGCONT);
    //char inp[1024];
    //if((n=read(fd[0],&inp,1024))<=0)
      //  error("Pipe Error\n");
    if(n=write(*newsockfd,inp,1024)<0)
        error("Error writing to socket\n");
    if(strcmp(inp,"FileUploadAllow")==0)
    {
        bzero(write_buffer, 1024);
        FILE * fp=fopen(file,"wb");
        if(fp==NULL)
            error("File Cannot be opened file on peer.\n");
        printf("Downloading\n");
        fflush(stdout);
        bzero(write_buffer, 1024);
        while(1)
        {
            if((n=read(*newsockfd,write_buffer,1024))<=0)
                error("\nERROR: Writing to socket");
            if(strcmp(write_buffer,"END")==0)
                break;
            fprintf(fp,"%s",write_buffer);
            bzero(write_buffer, 1024);
            printf(".");
            fflush(stdout);
        }
        printf("\nDownload Complete\n");
        fflush(stdout);
        fclose(fp);
    }
}
void cFileUpload(char write_buffer[],int *sockfd)
{
    int n,i;
    char read_buffer[1024];
    char file[1024];
    for(i=11;write_buffer[i]!='\0';i++)
        file[i-11]=write_buffer[i];
    file[i-11]='\0';
    bzero(read_buffer,1024);
    if(n=read(*sockfd,read_buffer,1024)<0)
        error("Error in reading from socket");
    if(strcmp(read_buffer,"FileUploadAllow")==0)
    {
        printf("Uploading\n");
        fflush(stdout);
        bzero(read_buffer,1024);
        FILE *fp=fopen(file,"rb");
        if(fp==NULL)
            error("ERROR: File not found on server.\n");
        while((n=fread(read_buffer,sizeof(char),1024,fp))>0)
        {
            if(write(*sockfd,read_buffer,1024)<=0)
                error("Error in writing in socket");
            bzero(read_buffer,1024);
            printf(".");
            fflush(stdout);
        }
        fclose(fp);
        if(write(*sockfd,"END",1024)<=0)
            error("Error in writing in socket");
        printf("\nUpload Complete\n");
        fflush(stdout);
    }
}
