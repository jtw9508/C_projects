#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400


/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

typedef struct cache_node {
  char *url;
  char *content;
  int size;
  struct cache_node *next, *prev;  
} cache_node;

typedef struct cache_list{
  cache_node *head;
  cache_node *tail;
  int total_size;
  int max_cache_size;
  int max_block_size;
  pthread_mutex_t mutex;
} cache_list;

typedef struct {
    int *connfdp;       // 클라이언트 연결 파일 디스크립터 포인터
    cache_list *cache_list;  // 공유 캐시 리스트
} thread_args;

void doit(int fd, cache_list *cache);
int parse_uri(char *uri, char *hostname, char *path, char *port);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rp);
void send_get_request_to_server(int clientfd, char *hostname, char *path, char *method);
void *thread(void *vargp);
int find_cache(cache_node *head, char *full_url, char *cache_buf);
cache_list *new_cache_list(int max_cache_size, int max_block_size);
cache_node *create_cache_node(char *url, char *content);

cache_node *create_cache_node(char *url, char *content) {
  cache_node *p = (cache_node *)calloc(1, sizeof(cache_node));
  p -> url = url;
  p -> content = content;
  p -> size = strlen(content);
  p -> next = NULL;
  p -> prev = NULL;
  return p;
}

cache_list *new_cache_list(int max_cache_size, int max_block_size) {
  cache_list *p = (cache_list *)calloc(1, sizeof(cache_list));
  p -> head = NULL;
  p -> tail = NULL;
  p -> total_size = 0;
  p -> max_cache_size = max_cache_size;
  p -> max_block_size = max_block_size;
  pthread_mutex_init(&p->mutex, NULL);
  return p;
}

thread_args *create_thread_args(int *connfdp, cache_list *cache_list) {
  thread_args *args = (thread_args*) Malloc(sizeof(thread_args));
  args-> connfdp = connfdp;  
  args-> cache_list = cache_list;
  return args; 
}

int main(int argc, char **argv) {
  signal(SIGPIPE, SIG_IGN);
  signal(SIGCHLD, SIG_IGN);
  printf("%s", user_agent_hdr);
  int listenfd, connfd, *connfdp;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  pthread_t tid;
  cache_list *cache_list;
  thread_args *args;
  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  cache_list = new_cache_list(MAX_CACHE_SIZE, MAX_OBJECT_SIZE);
  listenfd = Open_listenfd(argv[1]);
  while (1) {    
    clientlen = sizeof(clientaddr);
    connfdp = Malloc(sizeof(int));
    *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // line:netp:tiny:accept
    // connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    args = create_thread_args(connfdp,cache_list);
    Pthread_create(&tid, NULL, thread, args);                   
    
    // printf("Accepted connection from (%s, %s)\n", hostname, port);
    // doit(connfd);
    //   // line:netp:tiny:close
    // if (Fork() == 0){
    //   Close(listenfd);
    //   doit(connfd);
    //   Close(connfd);
    //   exit(0);
    // }
    // Close(connfd);
  }
}

void *thread(void *vargp)
{
  thread_args *args = (thread_args *)vargp;
  int connfd = *(args->connfdp);
  cache_list *cache = args->cache_list;
  Pthread_detach(pthread_self());
  Free(args->connfdp);
  Free(args);
  doit(connfd, cache);
  Close(connfd);
  return NULL;
}

void doit(int fd, cache_list *cache)
{
  int is_static, client_proxy;
  struct stat sbuf; 
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], hostname[MAXLINE], path[MAXLINE], port[MAXLINE], full_url[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio, rio_proxy;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers:\n");
  printf("%s", buf);

  sscanf(buf, "%s %s %s", method, uri, version);
  // if (strcasecmp(method, "GET")) {
  //   clienterror(fd, method, "501", "Not implemented",
  //   "Proxy does not implement this method");
  //   return;
  // }

  // read_requesthdrs(&rio);

  /* Parse URI from GET request */
  is_static = parse_uri(uri, hostname, path, port);
  strcpy(full_url, hostname);
  strcat(full_url, path);
  cache_node *head;
  char cache_buf[MAX_OBJECT_SIZE];
  head = cache->head;
  if (find_cache(head,full_url,cache_buf) == 0) {
    
    Rio_writen(fd, cache_buf, head->size);
    return;
  }
  
  printf("host:%s path: %s method:%s port:%s\n", hostname, path, method, port);
  client_proxy = Open_clientfd(hostname, port);
  if (client_proxy < 3) {
    clienterror(fd, hostname, "404", "Not found",
    "Proxy can not connect this address");
    Close(client_proxy);
    return;
  }

  send_get_request_to_server(client_proxy, hostname, path, method);
  char response[MAXBUF];
  ssize_t n;
  size_t total_size = 0;
  
  Rio_readinitb(&rio_proxy, client_proxy);

  // function int find_cache(cache->head, full_url) return -1 if full_url not in cache, return 0 if full_url in cache
  // cache_node *head;
  // head = cache->head;
  // if (find_cache(head,full_url) == 0) {
  //   strcpy(cache_buf, head->content);
  //   Rio_writen(fd, cache_buf, head->size);
  //   Rio_writen(fd, "cache", 5);
  //   Close(client_proxy);
  //   return;
  // }
  //if find_cache return -1, then get from real server
  while ((n = Rio_readlineb(&rio_proxy, response, MAXLINE)) > 0) {
      Rio_writen(fd, response, n);  // 실제 읽어온 바이트 수만큼 전송
      if (total_size + n <= MAX_OBJECT_SIZE) {
        memcpy(cache_buf+total_size,response,n);
      }      
      total_size += n;
      // cache_check(cache_buf,response); 
  }
  // if total_size < MAX_OBJECT_SIZE, put cache_buf in cache
  if (total_size <= MAX_OBJECT_SIZE) {
    char *content = malloc(strlen(cache_buf)+1);
    strcpy(content, cache_buf);
    char *url = malloc(strlen(full_url)+1);
    strcpy(url, full_url);
    cache_node *new_cache_node = create_cache_node(url,content);
    int node_size = new_cache_node->size;

    if ((cache -> total_size) + node_size <= MAX_CACHE_SIZE) {
      if (cache->head == NULL) {
        cache->head = new_cache_node; // 기존 head의 prev를 새 노드로 설정
        cache->tail = new_cache_node;
        // cache->head->next = cache->tail;
        // cache->tail->prev = cache->head;
      }
      else {
        cache->head->prev = new_cache_node;
        new_cache_node->next = cache->head;
        cache->head = new_cache_node;        
      }
      cache->total_size += node_size;
    }
    //if cache_size is overed, remove lru cache until get enough space for new cache
    else {
      while ((cache->total_size) + node_size > MAX_CACHE_SIZE) {
        cache_node *old_block = cache->tail;
        cache->total_size -= old_block->size;
        cache->tail = cache->tail->prev;
        cache->tail->next = NULL;
        free(old_block->content);
        free(old_block->url);
        free(old_block);
      }
      cache->head->prev = new_cache_node;
      cache->head = new_cache_node;
      cache->total_size += node_size;
    }
  }
  // while ((n = Rio_readnb(&rio_proxy, re)))
  // if Rio_readn(rio_proxy->rio_fd,cache_buf, MAX_OBJECT_SIZE)
  //check if cache is in list
  // head = cache_list -> head;
  // while (head -> next != NULL) {

  // } 
  // 여러 줄의 응답을 읽어 클라이언트로 전송

  Close(client_proxy);
}

int find_cache(cache_node *head, char *full_url, char *cache_buf) {
  while (head != NULL){
    if (strcmp(head->url, full_url) == 0) {
      strcpy(cache_buf, head->content);
      return 0;
    } 
    head = head -> next;
  }
  return -1;
}

void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE);
  while(strcmp(buf, "\r\n")) { /* header and body is separated by \r\n\r\n */
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

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
//     // printf("%p", pathbegin);
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


void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];
  /* Build the HTTP response body */
  sprintf(body, "<html><title>Proxy Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

void send_get_request_to_server(int clientfd, char *hostname, char *path, char *method) {
    char buf[MAXLINE];

    // Prepare the GET request message
    sprintf(buf, "%s %s HTTP/1.0\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n"
             "Proxy-Connection: close\r\n"
             "%s\r\n"
             "\r\n", method, path,
             hostname, user_agent_hdr);

    // Send the GET request to the server
    Rio_writen(clientfd, buf, strlen(buf));
    // printf("%s", buf);
}

//1. checking if buf can be cached and apply LRU 
int cache_check(char *cache_buf, char *response){

}


// #include <stdio.h>
// #include "csapp.h"

// /* Recommended max cache and object sizes */
// #define MAX_CACHE_SIZE 1049000
// #define MAX_OBJECT_SIZE 102400


// /* You won't lose style points for including this long line in your code */
// static const char *user_agent_hdr =
//     "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
//     "Firefox/10.0.3\r\n";

// typedef struct cache_node {
//   char *url;
//   char *content;
//   int size;
//   struct cache_node *next, *prev;  
// } cache_node;

// typedef struct cache_list{
//   cache_node *head;
//   cache_node *tail;
//   int total_size;
//   int max_cache_size;
//   int max_block_size;
//   pthread_mutex_t mutex;
// } cache_list;

// typedef struct {
//     int *connfdp;       // 클라이언트 연결 파일 디스크립터 포인터
//     cache_list *cache_list;  // 공유 캐시 리스트
// } thread_args;

// void doit(int fd, cache_list *cache);
// int parse_uri(char *uri, char *hostname, char *path, char *port);
// void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
// void read_requesthdrs(rio_t *rp);
// void send_get_request_to_server(int clientfd, char *hostname, char *path, char *method);
// void *thread(void *vargp);
// cache_list *new_cache_list(int max_cache_size, int max_block_size);

// cache_list *new_cache_list(int max_cache_size, int max_block_size) {
//   cache_list *p = (cache_list *)calloc(1, sizeof(cache_list));
//   p -> head = NULL;
//   p -> tail = NULL;
//   p -> total_size = 0;
//   p -> max_cache_size = max_cache_size;
//   p -> max_block_size = max_block_size;
//   pthread_mutex_init(&p->mutex, NULL);
//   return p;
// }



// thread_args *create_thread_args(int *connfdp, cache_list *cache_list) {
//   thread_args *args = (thread_args*) Malloc(sizeof(thread_args));
//   args-> connfdp = connfdp;  
//   args-> cache_list = cache_list;
//   return args; 
// }

// int main(int argc, char **argv) {
//   signal(SIGPIPE, SIG_IGN);
//   signal(SIGCHLD, SIG_IGN);
//   printf("%s", user_agent_hdr);
//   int listenfd, connfd, *connfdp;
//   char hostname[MAXLINE], port[MAXLINE];
//   socklen_t clientlen;
//   struct sockaddr_storage clientaddr;
//   pthread_t tid;
//   cache_list *cache_list;
//   thread_args *args;
//   /* Check command line args */
//   if (argc != 2) {
//     fprintf(stderr, "usage: %s <port>\n", argv[0]);
//     exit(1);
//   }
//   cache_list = new_cache_list(MAX_CACHE_SIZE, MAX_OBJECT_SIZE);
//   listenfd = Open_listenfd(argv[1]);
//   while (1) {    
//     clientlen = sizeof(clientaddr);
//     connfdp = Malloc(sizeof(int));
//     *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // line:netp:tiny:accept
//     // connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
//     Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
//     args = create_thread_args(connfdp,cache_list);
//     Pthread_create(&tid, NULL, thread, args);                   
    
//     // printf("Accepted connection from (%s, %s)\n", hostname, port);
//     // doit(connfd);
//     //   // line:netp:tiny:close
//     // if (Fork() == 0){
//     //   Close(listenfd);
//     //   doit(connfd);
//     //   Close(connfd);
//     //   exit(0);
//     // }
//     // Close(connfd);
//   }
// }

// void *thread(void *vargp)
// {
//   thread_args *args = (thread_args *)vargp;
//   int connfd = *(args->connfdp);
//   cache_list *cache = args->cache_list;
//   Pthread_detach(pthread_self());
//   Free(args->connfdp);
//   Free(args);
//   doit(connfd, cache);
//   Close(connfd);
//   return NULL;
// }

// void doit(int fd, cache_list *cache)
// {
//   int is_static, client_proxy;
//   struct stat sbuf; 
//   char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], hostname[MAXLINE], path[MAXLINE], port[MAXLINE];
//   char filename[MAXLINE], cgiargs[MAXLINE];
//   rio_t rio, rio_proxy;


//   /* Read request line and headers */
//   Rio_readinitb(&rio, fd);
//   Rio_readlineb(&rio, buf, MAXLINE);
//   printf("Request headers:\n");
//   printf("%s", buf);

//   sscanf(buf, "%s %s %s", method, uri, version);
//   // if (strcasecmp(method, "GET")) {
//   //   clienterror(fd, method, "501", "Not implemented",
//   //   "Proxy does not implement this method");
//   //   return;
//   // }

//   // read_requesthdrs(&rio);

//   /* Parse URI from GET request */
//   is_static = parse_uri(uri, hostname, path, port);
//   printf("host:%s path: %s method:%s port:%s\n", hostname, path, method, port);
//   client_proxy = Open_clientfd(hostname, port);
//   if (client_proxy < 3) {
//     clienterror(fd, hostname, "404", "Not found",
//     "Proxy can not connect this address");
//     Close(client_proxy);
//     return;
//   }

//   send_get_request_to_server(client_proxy, hostname, path, method);
  
//   Rio_readinitb(&rio_proxy, client_proxy);
//   char response[MAXBUF];
//   ssize_t n;
//   char cache_buf[MAX_OBJECT_SIZE];
//   //check if cache is in list
//   // head = cache_list -> head;
//   // while (head -> next != NULL) {

//   // } 
//   // 여러 줄의 응답을 읽어 클라이언트로 전송
//   while ((n = Rio_readlineb(&rio_proxy, response, MAXLINE)) > 0) {
//       Rio_writen(fd, response, n);  // 실제 읽어온 바이트 수만큼 전송
//       cache_check(cache_buf,response); 

//   }

//   Close(client_proxy);
// }

// void read_requesthdrs(rio_t *rp)
// {
//   char buf[MAXLINE];

//   Rio_readlineb(rp, buf, MAXLINE);
//   while(strcmp(buf, "\r\n")) { /* header and body is separated by \r\n\r\n */
//     Rio_readlineb(rp, buf, MAXLINE);
//     printf("%s", buf);
//   }
//   return;
// }

// // int parse_uri(char *uri, char *hostname, char *path, char *port) {
// //     char *hostbegin, *hostend, *pathbegin;
// //     char defaultport[] = "80";

// //     // 'http://' 부분을 제외하고 시작 부분 찾기
// //     hostbegin = strstr(uri, "://");
// //     if (hostbegin != NULL) {
// //         hostbegin += 3;  // 'http://' 건너뛰기
// //     } else {
// //         hostbegin = uri;  // 만약 'http://'가 없으면 시작부터 처리
// //     }

// //     // 경로 시작 위치를 찾음
// //     pathbegin = strchr(hostbegin, '/');
// //     // printf("%p", pathbegin);
// //     if (pathbegin != NULL) {
// //         strcpy(path, pathbegin);  // 경로 복사
// //         // *pathbegin = '\0';        // 호스트와 경로 분리
// //     } else {
// //         strcpy(path, "/");  // 경로가 없으면 루트 경로 설정
// //         pathbegin = hostbegin + strlen(hostbegin);   // pathbegin이 NULL로 설정됨
// //     }

// //     // 호스트 이름과 포트 추출
// //     hostend = strchr(hostbegin, ':');
// //     if (hostend != NULL) {
// //         // *hostend = '\0';
// //         // sscanf(hostend + 1, "%s", port);  // 포트 번호 파싱
// //         strncpy(port, hostend+1, pathbegin-hostend-1);
// //         strncpy(hostname, hostbegin, hostend-hostbegin);  // 호스트 이름 복사
// //     } else {
// //         strcpy(port, defaultport);  // 포트 번호가 없으면 기본 포트 80 사용
// //         strncpy(hostname, hostbegin, pathbegin-hostbegin);  // 호스트 이름 복사
// //     }
    
// //     return 0;
// // }
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
//     } else {
//         strcpy(path, "/");  // 경로가 없으면 루트 경로 설정
//         pathbegin = hostbegin + strlen(hostbegin);  // 경로가 없을 경우 처리
//     }

//     // 호스트 이름과 포트 추출
//     hostend = strchr(hostbegin, ':');
//     if (hostend != NULL) {
//         // 포트 번호 추출
//         strncpy(port, hostend + 1, pathbegin - hostend - 1);
//         port[pathbegin - hostend - 1] = '\0';  // Null-terminator 추가
        
//         // 호스트 이름 복사
//         strncpy(hostname, hostbegin, hostend - hostbegin);
//         hostname[hostend - hostbegin] = '\0';  // Null-terminator 추가
//     } else {
//         strcpy(port, defaultport);  // 포트 번호가 없으면 기본 포트 80 사용
        
//         // 호스트 이름 복사
//         strncpy(hostname, hostbegin, pathbegin - hostbegin);
//         hostname[pathbegin - hostbegin] = '\0';  // Null-terminator 추가
//     }

//     return 0;
// }


// void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
// {
//   char buf[MAXLINE], body[MAXBUF];
//   /* Build the HTTP response body */
//   sprintf(body, "<html><title>Proxy Error</title>");
//   sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
//   sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
//   sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
//   sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

//   /* Print the HTTP response */
//   sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
//   Rio_writen(fd, buf, strlen(buf));
//   sprintf(buf, "Content-type: text/html\r\n");
//   Rio_writen(fd, buf, strlen(buf));
//   sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
//   Rio_writen(fd, buf, strlen(buf));
//   Rio_writen(fd, body, strlen(body));
// }

// void send_get_request_to_server(int clientfd, char *hostname, char *path, char *method) {
//     char buf[MAXLINE];

//     // Prepare the GET request message
//     sprintf(buf, "%s %s HTTP/1.0\r\n"
//              "Host: %s\r\n"
//              "Connection: close\r\n"
//              "Proxy-Connection: close\r\n"
//              "%s\r\n"
//              "\r\n", method, path,
//              hostname, user_agent_hdr);

//     // Send the GET request to the server
//     Rio_writen(clientfd, buf, strlen(buf));
//     // printf("%s", buf);
// }

// //1. checking if buf can be cached and apply LRU 
// int cache_check(char *cache_buf, char *response){

// }


int parser(char *urL, char *doM, char *urI, char* porT, char* htT)
{
  char *start, *end;
  char temp_url[200];
  start = strstr(urL, "://");
  if (start != NULL) {
      strncpy(htT, urL, start - urL);
      htT[start - urL] = '\0';
      start += 3;
  } else {
      strcpy(htT, "http");
      start = urL;
  }
  end = strchr(start, '/');
  if (end != NULL) {
      strncpy(temp_url, start, end - start);
      temp_url[end - start] = '\0';
      strcpy(urI, end);
  } else {
      strcpy(temp_url, start);
      strcpy(urI, "/");
  }
  start = strchr(temp_url, ':');
  if (start != NULL) {
      strncpy(doM, temp_url, start - temp_url);
      doM[start - temp_url] = '\0';
      strcpy(porT, start + 1);
  } else {
      strcpy(doM, temp_url);
      strcpy(porT, "80");
  }
}