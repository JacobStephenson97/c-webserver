//Created following https://bruinsslot.jp/post/simple-http-webserver-in-c/


#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
int main() {
  char buffer[BUFFER_SIZE];

  char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world</html>\r\n";

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("webserver socket");
    return errno;
  }

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(PORT),
    .sin_addr = { .s_addr = htonl(INADDR_ANY) },
  };
  int addr_size = sizeof(addr);

  struct sockaddr_in client_addr;
  int client_addrlen = sizeof(client_addr);

  if (bind(sockfd, (struct sockaddr *)&addr, addr_size) == -1)
  {
    perror("webserver bind");
    return errno;
  }
  printf("webserver bind: success\n");
  if (listen(sockfd, SOMAXCONN) == -1)
  {
    perror("webserver listen");
    return errno;
  }
  printf("Listen: success\n");

  for (;;) {
    int newsockfd = accept(sockfd, (struct sockaddr *)&addr, (socklen_t *)&addr_size);

    if (newsockfd < 0) {
            perror("webserver (accept)");
            continue;
        }
        printf("connection accepted\n");

        // Get client address
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr,
                                (socklen_t *)&client_addrlen);
        if (sockn < 0) {
            perror("webserver (getsockname)");
            continue;
        }

        int valread = read(newsockfd, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("webserver (read)");
            continue;
        }
            // Read the request
        char method[BUFFER_SIZE], uri[BUFFER_SIZE], version[BUFFER_SIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), method, version, uri);

        
        // Send response
        int valwrite = write(newsockfd, resp, strlen(resp));
        if (valwrite < 0) {
            perror("webserver (write)");
            continue;
        }
        close(newsockfd);
    }
    return 0;
}
