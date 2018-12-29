#include "socketwrapper.h"
#include <time.h>

int main(int argc, char **argv)
{
    int clientfd;
    time_t t_start, t_end;
    
    if(argc != 2)
    {
        fprintf(stderr, "Usage: ./fileclient IP_Address\n");
        exit(1);
    }
    struct sockaddr_in clientaddr;
    bzero(&clientaddr, sizeof(clientaddr));    

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htons(INADDR_ANY);
    clientaddr.sin_port = htons(0);
    
    /* create socket */
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(clientfd < 0)    
    {
        perror("socket failed");
        exit(1);
    }
    
    /* bind */
    if(bind(clientfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0)
    {
        perror("bind failed");
        exit(1);
    }

    struct sockaddr_in svraddr;
    bzero(&svraddr, sizeof(svraddr));   /* init svraddr */
    if(inet_aton(argv[1], &svraddr.sin_addr) == 0)
    {
        perror("inet_aton failed");
        exit(1);
    }
    svraddr.sin_family = AF_INET;
    svraddr.sin_port = htons(PORT);
    
    t_start = time(NULL);   /* start to record time at here */
    socklen_t svraddrlen = sizeof(svraddr);
    if(connect(clientfd, (struct sockaddr*)&svraddr, svraddrlen) < 0)    
    {
        perror("connect failed");
        exit(1);
    }
    
    /* receive file imformation */
    char buff[BUFFSIZE];
    char filename[FILE_NAME_MAX_SIZE];
    int count;
    bzero(buff, BUFFSIZE);

    count = recv(clientfd, buff, BUFFSIZE, 0);
    if(count < 0)
    {
        perror("recv failed");
        exit(1);
    }

    int buff_size = strlen(buff);
    strncpy(filename, buff, buff_size > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : buff_size);
    filename[buff_size] = '\0';
    printf("Preparing receive file: %s from %s \n", filename, argv[1]);    

    FILE *fd = fopen(filename, "wb+");
    if(NULL == fd)
    {
        perror("open file failed");
        exit(1);
    }
    bzero(buff, BUFFSIZE);
    
    int length = 0;
    
    /* start to receive file */
    while((length = recv(clientfd, buff, BUFFSIZE, 0)))
    {
        if(length < 0)
        {
            perror("recv failed");
            exit(1);
        }
        int writelen = fwrite(buff, sizeof(char), length, fd);
        if(writelen < length)
        {
            perror("write failed");
            exit(1);
        }
        bzero(buff, BUFFSIZE);
    }

    t_end = time(NULL);    /* end to record time */
    printf("Receieved file: %s from %s finished!\n", filename, argv[1]);
    printf("Cost time: %.0fs\n", difftime(t_end, t_start));
    
    fclose(fd);
    close(clientfd);  
    return 0;
}
