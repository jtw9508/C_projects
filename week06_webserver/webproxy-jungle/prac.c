#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_sockaddr_as_sockaddr_in(struct sockaddr *sa) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)sa;

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
    printf("IPv4 Address: %s\n", ipstr);
    printf("Port: %d\n", ntohs(ipv4->sin_port));
}

int main() {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8080);
    inet_pton(AF_INET, "192.168.1.1", &sa);

    struct sockaddr *sa_ptr = (struct sockaddr *)&sa;

    // 출력
    printf("Family: %d\n", sa_ptr->sa_family);

    // sa_data를 16진수로 출력
    printf("Data: ");
    for (int i = 0; i < sizeof(sa_ptr->sa_data); i++) {
        printf("%02x ", (unsigned char)sa_ptr->sa_data[i]);
    }
    printf("\n");

    // 상세 주소 정보 출력
    print_sockaddr_as_sockaddr_in(sa_ptr);

    return 0;
}
