/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void) {
  char *buf, *p, *arg1_num, *arg2_num;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE], temp1[MAXLINE], temp2[MAXLINE];
  int n1=0, n2=0;

  /* Extract the two arguments */
  if ((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&');
    *p = '\0';
    strcpy(arg1, buf);
    strcpy(arg2, p+1);
    arg1_num = strchr(arg1,'=');
    arg2_num = strchr(arg2,'=');
    strcpy(temp1,arg1_num+1);
    strcpy(temp2,arg2_num+1);
    n1 = atoi(temp1);
    n2 = atoi(temp2);
    printf("%d, %d",n1,n2);
  }

  /* Make the response body */

  sprintf(content, "QUERY_STRING=%s", buf);
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is: %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  /* Generate the HTTP renponse */
  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  if (getenv("HHEAD") == NULL) {
    printf("%s", content);
  }

  fflush(stdout); 
  exit(0);
}
/* $end adder */
