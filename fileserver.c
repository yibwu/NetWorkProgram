#include "socketwrapper.h"

int main(int argc, char **argv)
{
    /* input the file name */
    char filename[FILE_NAME_MAX_SIZE];
    bzero(filename,FILE_NAME_MAX_SIZE);
    printf("Please input the file name you want to send: ");
    fgets(filename, FILE_NAME_MAX_SIZE, stdin);
    int file_name_size = strlen(filename) - 1;
    filename[file_name_size] = '\0';

    /* create socket */
    int sockfd, connfd;
    struct sockaddr_in svraddr, clientaddr;
    bzero(&svraddr, sizeof(svraddr));
    
    svraddr.sin_family = AF_INET;
    svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svraddr.sin_port = htons(PORT);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("socket failed");
        exit(1);
    }

    /* bind */    
    if(bind(sockfd,(struct sockaddr*)&svraddr,sizeof(svraddr)) < 0)
    {
        perror("bind failed");
        exit(1);
    }

    /* listen */
    if(listen(sockfd, LISTENQ) < 0)
    {
        perror("listen failed");
        exit(1);
    }

    while(1)
    {
        socklen_t length = sizeof(clientaddr);

        /* accept */
        connfd = accept(sockfd,(struct sockaddr*)&clientaddr,&length);
        if(connfd < 0)
        {
            perror("connect failed");
            exit(1);
        }
    
        /* send file imformation */
        char buff[BUFFSIZE];
        int count;
        bzero(buff, BUFFSIZE);
        strncpy(buff, filename, file_name_size > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : file_name_size);

        count = send(connfd, buff, strlen(buff), 0);
        if(count < 0)
        {
            perror("Send file information");
            exit(1);
        }
        
        /* read file */ 
        FILE *fd = fopen(filename, "rb");
        if(fd == NULL)
        {
            printf("File :%s not found!\n",filename);
        }
        else 
        {
            bzero(buff,BUFFSIZE);
            int file_block_length = 0;
            while((file_block_length = fread(buff, sizeof(char), BUFFSIZE,fd)) > 0)
            {
                if(send(connfd, buff,file_block_length, 0) < 0)
                {
                    perror("Send failed");
                    exit(1);
                }
                bzero(buff, BUFFSIZE);    
            }
            fclose(fd);
            printf("Transfer file finished!\n");
        }
        close(connfd);
    }
    close(sockfd);
    
    return 0;
}

