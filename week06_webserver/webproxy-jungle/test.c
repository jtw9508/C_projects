#include <stdio.h>
#include <string.h>
// #include "proxy.c"

// int parse_uri(char *uri, char *hostname, char *path, char *port) {
//     char *hostbegin, *hostend, *pathbegin;
//     char defaultport[] = "80";

//     // 'http://' 부분을 제외하고 시작 부분 찾기
//     hostbegin = strstr(uri, "://");
//     if (hostbegin != NULL) {
//         hostbegin += 3;  // 'http://' 건너뛰기
//     } else {
//         hostbegin = uri;  // 만약 'http://'가 없으면 시작부터 처리
//     }

//     // 경로 시작 위치를 찾음
//     pathbegin = strchr(hostbegin, '/');
//     if (pathbegin != NULL) {
//         strcpy(path, pathbegin);  // 경로 복사
//         *pathbegin = '\0';        // 호스트와 경로 분리
//     } else {
//         strcpy(path, "/");  // 경로가 없으면 루트 경로 설정
//     }

//     // 호스트 이름과 포트 추출
//     hostend = strchr(hostbegin, ':');
//     if (hostend != NULL) {
//         *hostend = '\0';
//         sscanf(hostend + 1, "%s", port);  // 포트 번호 파싱
//     } else {
//         strcpy(port, defaultport);  // 포트 번호가 없으면 기본 포트 80 사용
//     }
//     strcpy(hostname, hostbegin);  // 호스트 이름 복사
//     return 0;
// }
// int parse_uri(char *uri, char *hostname, char *path, char *port) {
//     char *hostbegin, *hostend, *pathbegin;
//     char defaultport[] = "80";

//     // 'http://' 부분을 제외하고 시작 부분 찾기
//     hostbegin = strstr(uri, "://");
//     if (hostbegin != NULL) {
//         hostbegin += 3;  // 'http://' 건너뛰기
//     } else {
//         hostbegin = uri;  // 만약 'http://'가 없으면 시작부터 처리
//     }

//     // 경로 시작 위치를 찾음
//     pathbegin = strchr(hostbegin, '/');
//     printf("%p", pathbegin);
//     if (pathbegin != NULL) {
//         strcpy(path, pathbegin);  // 경로 복사
//         // *pathbegin = '\0';        // 호스트와 경로 분리
//     } else {
//         strcpy(path, "/");  // 경로가 없으면 루트 경로 설정
//         pathbegin = hostbegin + strlen(hostbegin);   // pathbegin이 NULL로 설정됨
//     }

//     // 호스트 이름과 포트 추출
//     hostend = strchr(hostbegin, ':');
//     if (hostend != NULL) {
//         // *hostend = '\0';
//         // sscanf(hostend + 1, "%s", port);  // 포트 번호 파싱
//         strncpy(port, hostend+1, pathbegin-hostend-1);
        
//         strncpy(hostname, hostbegin, hostend-hostbegin);  // 호스트 이름 복사
//         printf("%p,%ld",hostbegin, hostend-hostbegin);
        
//     } else {
//         strcpy(port, defaultport);  // 포트 번호가 없으면 기본 포트 80 사용
//         strncpy(hostname, hostbegin, pathbegin-hostbegin);  // 호스트 이름 복사
//     }
    
//     return 0;
// }
int parse_uri(char *uri, char *hostname, char *path, char *port) {
    char *hostbegin, *hostend, *pathbegin;
    char defaultport[] = "80";

    // 'http://' 부분을 제외하고 시작 부분 찾기
    hostbegin = strstr(uri, "://");
    if (hostbegin != NULL) {
        hostbegin += 3;  // 'http://' 건너뛰기
    } else {
        hostbegin = uri;  // 만약 'http://'가 없으면 시작부터 처리
    }

    // 경로 시작 위치를 찾음
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin != NULL) {
        strcpy(path, pathbegin);  // 경로 복사
    } else {
        strcpy(path, "/");  // 경로가 없으면 루트 경로 설정
        pathbegin = hostbegin + strlen(hostbegin);  // 경로가 없을 경우 처리
    }

    // 호스트 이름과 포트 추출
    hostend = strchr(hostbegin, ':');
    if (hostend != NULL) {
        // 포트 번호 추출
        strncpy(port, hostend + 1, pathbegin - hostend - 1);
        port[pathbegin - hostend - 1] = '\0';  // Null-terminator 추가
        
        // 호스트 이름 복사
        strncpy(hostname, hostbegin, hostend - hostbegin);
        hostname[hostend - hostbegin] = '\0';  // Null-terminator 추가
    } else {
        strcpy(port, defaultport);  // 포트 번호가 없으면 기본 포트 80 사용
        
        // 호스트 이름 복사
        strncpy(hostname, hostbegin, pathbegin - hostbegin);
        hostname[pathbegin - hostbegin] = '\0';  // Null-terminator 추가
    }

    return 0;
}

void test_parse_uri(char *uri) {
    char hostname[256];
    char path[256];
    char port[10];
    
    // Call the parse_uri function
    parse_uri(uri, hostname, path, port);
    
    // Print the results
    printf("URI: %s\n", uri);
    printf("Hostname: %s\n", hostname);
    printf("Path: %s\n", path);
    printf("Port: %s\n", port);
    printf("\n");
}

int main() {
    // Test cases
    test_parse_uri("http://example.com/path/to/resource");
    test_parse_uri("http://example.com:8080/path/to/resource");
    test_parse_uri("http://example.com/");
    test_parse_uri(" https://www.naver.com ");
    test_parse_uri("http://example.com:8080");
    
    return 0;
}