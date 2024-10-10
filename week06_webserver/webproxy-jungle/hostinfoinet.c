#include "csapp.h"

int main(int argc, char **argv)
{
    struct addrinfo *p, *listp, hints;
    struct sockaddr_in *sockp;
    char buf[MAXLINE];
    int rc, flags;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    /* Get a list of addrinfo records */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* Connections only */
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* Walk the list and display each IP address */
    // flags = NI_NUMERICHOST; /* Display address string instead of domain name */
    for (p = listp; p; p = p->ai_next) {
        // getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE,NULL, 0, flags);
        sockp = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(AF_INET,&(sockp->sin_addr),buf,MAXBUF);
        printf("%s\n",buf);
    }

    /* Clean up */
    Freeaddrinfo(listp);

    exit(0);
}


typedef struct {
    int rio_fd;               //읽어야할 파일 디스크립터
    int rio_cnt;              //현재까지 읽은 바이트 수
    char *rio_bufptr;         //버퍼 포인터로 해당 포인터 위치부터 읽기 시작
    char rio_buf[RIO_BUFSIZE] //파일을 읽어와서 임시로 저장할 버퍼
}

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd; //rio_t 구조체에 읽어야할 파일 디스크립터 배정
    rp->rio_cnt = 0; //현재 읽은 바이트는 0
    rp->rio_bufptr = rp->rio_buf; //읽기 시작할 위치는 버퍼의 시작부분
}