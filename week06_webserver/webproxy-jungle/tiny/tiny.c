#include "csapp.h"
#include <signal.h>

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv) {
  signal(SIGPIPE, SIG_IGN);
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {    
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr,
                    &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
  // return 0;
}

void doit(int fd)
{
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  /* Read request line and headers */
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers:\n");
  printf("%s", buf);
  // /* echo 11-6 header */
  // Rio_writen(fd, buf, strlen(buf));

  sscanf(buf, "%s %s %s", method, uri, version);
  if (!(strcasecmp(method, "GET") ^ strcasecmp(method, "HHEAD"))) {
    clienterror(fd, method, "501", "Not implemented",
    "Tiny does not implement this method");
    return;
  }
  if (!strcasecmp(method, "HHEAD")) {
    setenv("HHEAD","head",1);
  }
  read_requesthdrs(&rio);

  /* Parse URI from GET request */
  is_static = parse_uri(uri, filename, cgiargs);
  if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found",
    "Tiny couldn't find this file");
    return;
  }

  if (is_static) { /* Serve static content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
      "Tiny couldn't find this file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size);
  }
  else { /* Serve dynamic content */
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden",
      "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
  char buf[MAXLINE], body[MAXBUF];
  /* Build the HTTP response body */
  sprintf(body, "<html><title>Tiny Error</title>");
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

int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin")) { /* Static content */
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/')
      strcat(filename, "home.html");
    return 1;
  }
  else { /* Dynamic content */
    ptr = index(uri, '?');
    if (ptr) {
      strcpy(cgiargs, ptr+1);
      *ptr = '\0';
    }
    else
      strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  /* Send response headers to client */
  get_filetype(filename, filetype);
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection: close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));
  if (getenv("HHEAD")) {
      return;
  }
  printf("Response headers:\n");
  printf("%s", buf);

  /* Send response body to client */
  srcfd = Open(filename, O_RDONLY, 0);
  // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  // Close(srcfd);
  // Rio_writen(fd, srcp, filesize);
  // Munmap(srcp, filesize);
  srcp = (char *)malloc(filesize);
  if (srcp ==NULL) {
    perror("malloc");
    close(srcfd);
    return;
  }
  if (rio_readn(srcfd, srcp, filesize) != filesize) {
    perror("rio_readn");
    free(srcp);
    close(srcfd);
    return;
  }
  close(srcfd);
  rio_writen(fd, srcp, filesize);
  free(srcp);

}
  /*
  * get_filetype - Derive file type from filename
  */
void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, "jpg"))
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mp4"))
    strcpy(filetype, "video/mp4");
  else
    strcpy(filetype, "text/plain");
}

void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL};

  /* Return first part of HTTP response */
  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));
  // if (strcasecmp(method, "HHEAD") == 0) {
  //     return;
  // }
  if (Fork() == 0) { /* Child */
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);
    Dup2(fd, STDOUT_FILENO); /* Redirect stdout to client */
    Execve(filename, emptylist, environ); /* Run CGI program */
  }
  Wait(NULL); /* Parent waits for and reaps child*/
}

// #include "csapp.h"
// #include <strings.h>
// ////
// #define PORTNUM "5555"
// #define SERVER_ADDR "13.125.233.186"
// typedef struct sockaddr SA;
// ////
// ////
// void doit(int fd);
// void c_error(int fd, char* m, char* ernum, char* smsg, char* lmsg);
// int p_uri(char* uri, char* fname, char* cgi);
// void serve_static(int fd, char* fname, int fsize);
// void r_rqhs(rio_t *rio);
// void serve_dynamic(int fd, char* fname, char* cgi);
// void get_filetype(char* fname, char* ftype);
// ////
// int main(void){
//     int l_fd, c_fd;
//     char hostname[MAXLINE], port[MAXLINE];
//     socklen_t clientlen;
//     struct sockaddr_storage clientaddr;
//     // rio_t rio; //일회성 에코
//     // char test[MAXLINE], test_buff[MAXLINE]; // 일회성 에코
//     l_fd = open_listenfd(PORTNUM);
//     while(1){
//         clientlen = sizeof(clientaddr);
//         c_fd = accept(l_fd, (SA*)&clientaddr, &clientlen);
//         getnameinfo((SA*)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
//         // 일회성 에코
//         // printf("..connect checking..\nhostname:%s\nconnect port: %s\n", hostname, port);
//         // Rio_readinitb(&rio, c_fd);                      // 클라 쪽 데이터 > rio구조체로
//         // sprintf(test,"..connect checking..\n");
//         // rio_writen(c_fd, test, strlen(test));           // test에 들어간 데이터 > 클라 쪽으로
//         // rio_readlineb(&rio, test_buff, MAXLINE);        // rio구조체 데이터 > test_buff 쪽으로
//         // rio_writen(c_fd, test_buff, strlen(test_buff)); // test_buff 데이터 > 클라 쪽으로
//         // rio_writen(c_fd, "----typing request----\n", 24);
//         // 서버 쪽 처리함수
//         doit(c_fd);
//         // 연결 끊기
//         close(c_fd);
//     }
//     return 0;
// }
// void doit(int fd){
//     int is_static;
//     struct stat sbuf;
//     char b[MAXLINE], m[MAXLINE], uri[MAXLINE], v[MAXLINE];
//     char fname[MAXLINE], cgi[MAXLINE];
//     rio_t doitrio;
//     rio_readinitb(&doitrio, fd);
//     printf("waiting for request\n\n");
//     rio_readlineb(&doitrio, b, MAXLINE);
//     printf("Requested Header :\n");
//     sscanf(b, "%s %s %s", m, uri, v);
//     printf("method: %s\nuri: %s\nversion: %s\n", m, uri, v);
//     if (strcasecmp(m, "GET")){// get일 경우 0 반환
//         c_error(fd, m, "501", "NOT implemented", "Tiny does not implement this method");
//         return;
//     }
//     r_rqhs(&doitrio);
//     is_static = p_uri(uri, fname, cgi);
//     if (stat(fname, &sbuf) < 0){
//         c_error(fd, fname, "404", "Not found","Tiny couldn't find this file");
//         return;
//     }
//     if (is_static){ // 정적
//         if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)){
//             c_error(fd, fname, "403", "Forbidden", "Tiny couldn't read this file");
//             return;
//         }
//         serve_static(fd, fname, sbuf.st_size);
//     }else{
//         if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)){
//             c_error(fd, fname, "403", "Forbidden", "Tiny couldn't run the CGI program");
//             return;
//         }
//         serve_dynamic(fd, fname, cgi);
//     }
// }
// void c_error(int fd, char* m, char* ernum, char* smsg, char* lmsg){
//     char head[MAXLINE], body[MAXLINE];
// // body build
//     sprintf(body, "<html><title>!error!</title>");
//     sprintf(body, "%s<body style=\"background-color: #FFA500;\">\r\n", body);
//     sprintf(body, "%s%s: %s\r\n", body, ernum, smsg);
//     sprintf(body, "%s<p>%s: %s\r\n", body, lmsg, m);
//     sprintf(body, "%s<hr><em>TThhee TTIINNYY WWEEBB SSEERRVVEERR</em>\r\n", body);
// // head
//     // head 입력 및 출력
//     sprintf(head, "HTTP/1.0 %s %s\r\n", ernum, smsg);
//     rio_writen(fd, head, strlen(head));
//     sprintf(head, "content-type: text/html\r\n");
//     rio_writen(fd, head, strlen(head));
//     sprintf(head, "content-length: %d\r\n\r\n", (int)strlen(body));
//     rio_writen(fd, head, strlen(head));
//     // body 출력
//     rio_writen(fd, body, strlen(body));
// }
// void r_rqhs(rio_t *rio){
//     char b[MAXLINE];
//     rio_readlineb(rio, b, MAXLINE);
//     while(strcmp(b, "\r\n")){
//         rio_readlineb(rio, b, MAXLINE);
//         printf("%s", b);
//     }
//     return;
// }
// int p_uri(char* uri, char* fname, char* cgi){
//     char *ptr;
//     if (!strstr(uri, "cgi-bin")){ // 정적 콘텐츠
//         strcpy(cgi, "");
//         strcpy(fname, ".");
//         strcat(fname, uri);
//         if (uri[strlen(uri)-1] == '/'){
//             strcat(fname, "home.html");
//         }
//         return 1;
//     }
//     else{ // 동적 콘텐츠
//         ptr = strchr(uri, '?');
//         if (ptr){
//             strcpy(cgi, ptr+1);
//             *ptr = '\0';
//         }
//         else{
//             strcpy(cgi, "");
//         }
//         strcpy(fname, ".");
//         strcat(fname, uri);
//         return 0;
//     }
// }
// void serve_static(int fd, char* fname, int fsize){
//     int src_fd;
//     char*src_p, ftype[MAXLINE], b[MAXLINE];
//     get_filetype(fname, ftype);
//     sprintf(b, "HTTP/1.0 200 OK\r\n");
//     sprintf(b, "%sServer: Tiny Web Server\r\n", b);
//     sprintf(b, "%sConnection: Close\r\n", b);
//     sprintf(b, "%sConnection-length: %d\r\n", b, fsize);
//     sprintf(b, "%sConnection-type: %s\r\n\r\n", b, ftype);
//     rio_writen(fd, b, strlen(b));
//     printf("Respinse headers:\n");
//     printf("%s", b);
//     src_fd = open(fname, O_RDONLY, 0);
//     // src_p = mmap(0, fsize, PROT_READ, MAP_PRIVATE, src_fd, 0);
//     src_p = (char *)malloc(fsize);
//     if (src_p ==NULL) {
//       perror("malloc");
//       close(src_fd);
//       return;
//     }
//     if (rio_readn(src_fd, src_p, fsize) != fsize) {
//       perror("rio_readn");
//       free(src_p);
//       close(src_fd);
//       return;
//     }
//     // *src_p = src_fd;

//     close(src_fd);
//     rio_writen(fd, src_p, fsize);
//     free(src_p);
//     // munmap(src_p, fsize);
// }
// void get_filetype(char* fname, char* ftype){
//     if (strstr(fname,".html")){
//         strcpy(ftype, "text/html");
//     }else if (strstr(fname,".gif")){
//         strcpy(ftype, "image/gif");
//     }else if (strstr(fname,".png")){
//         strcpy(ftype, "image/png");
//     }else if (strstr(fname,".jpg")){
//         strcpy(ftype, "image/jpeg");
//     }else if(strstr(fname,".mp4")){
//         strcpy(ftype, "video/mp4");
//     }else{
//         strcpy(ftype, "text/plain");
//     }
// }
// void serve_dynamic(int fd, char* fname, char* cgi){
//     char b[MAXLINE], *elist[] = { NULL };
//     sprintf(b, "HTTP/1.0 200 OK\r\n");
//     Rio_writen(fd, b, strlen(b));
//     sprintf(b, "Server: Tiny Web Server\r\n");
//     Rio_writen(fd, b, strlen(b));
//     // if (Fork() == 0) {
//     //     setenv("QUERY_STRING", cgi, 1);
//     //     Dup2(fd, STDOUT_FILENO);
//     //     Execve(fname, elist, environ);
//     // }
//     // wait(NULL);
//     if (Fork() == 0) {
//     // 자식 프로세스에서 CGI 실행
//     setenv("QUERY_STRING", cgi, 1);
//     Dup2(fd, STDOUT_FILENO);
//     Execve(fname, elist, environ);
//     // 만약 Execve가 실패했다면 로그 출력
//     perror("Execve failed");
//     }
//     // 부모 프로세스에서 자식 프로세스가 정상적으로 실행되는지 확인
//     wait(NULL);
// }