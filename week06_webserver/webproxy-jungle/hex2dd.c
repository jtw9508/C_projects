// #include "csapp.h"

// int main(int argc, char **argv) {
//     struct in_addr inaddr; /* Address in network byte order */
//     uint16_t addr; /* Address in host byte order */
//     char buf[MAXBUF]; /* Buffer for dotted-decimal string */

//     if (argc != 2) {
//         fprintf(stderr, "usage: %s <hex number>\n", argv[0]);
//         exit(0);
//     }
//     sscanf(argv[1], "%hx", &addr);
//     inaddr.s_addr = htons(addr);

//     if (!inet_ntop(AF_INET, &inaddr, buf, MAXBUF))
//         unix_error("inet_ntop");
//     printf("%s\n", buf);
//     exit(0);
// }

// int main(int argc, char **argv) {
//     struct in_addr inaddr; /* Address in network byte order */
//     int rc;

//     if (argc != 2) {
//         fprintf(stderr, "usage: %s <hex number>\n", argv[0]);
//         exit(0);
//     }

//     rc = inet_pton(AF_INET, argv[1], &inaddr);
//     if (rc == 0)
//         app_error("inet_pton error: invalid dotted-decimal address");
//     else if (rc < 0)
//         unix_error("inet_pton error");
//     printf("0x%x\n", ntohs(inaddr.s_addr));
//     exit(0);
// }

/* IP socket address structure */
// struct sockaddr_in 
// {
//     uint16_t		sin_family; /* Protocol family (always AF_INET) */
//     uint16_t		sin_port;   /* Port number in network byte order */
//     struct in_addr  sin_addr;   /* IP address in network byte order */
//     unsigned char   sin_zero[8] /* Pad to sizeof(struct sockaddr) */
// };

// /* Generic socket address structure (for connect, bind, and accept) */
// struct sockaddr 
// {
//     uint16_t  sa_family;    /* Protocol family */
//     char      sa_data[14];  /* Address data */
// };
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 5123  // 서버와 동일한 포트 번호로 설정
int main(int argc, char const * argv[])
{
  int sock, inet, conn;
  int valread;
  struct sockaddr_in address;
  char *hello = "mal jom no cha";
  char buffer[1024] = {0};
  // 1. 소켓 생성
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    printf("socket creation error\n");
    return -1;
  }
  printf("socket %d\n", sock);
  // 서버 주소 설정
  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  // IP 주소 변환
  inet = inet_pton(AF_INET, "43.203.26.16", &address.sin_addr);  // 서버 IP 주소 사용
  if(inet <= 0)
  {
    printf("inet_pton error\n");
    return -1;
  }
  // 2. 서버와 연결 시도
  conn = connect(sock, (struct sockaddr *)&address, sizeof(address));
  if(conn < 0)
  {
    
    printf("connect error\n");
    return -1;
  }
  printf("conn %d\n", conn);
  // 3. 서버와 통신
  send(sock, hello, strlen(hello), 0);
  printf("hello message sent\n");
  valread = read(sock, buffer, 1024);
  printf("from server: %s\n", buffer);
  // 소켓 종료
  close(sock);
  return 0;
}