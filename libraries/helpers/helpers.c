#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <ctype.h>      /* isdigit, isspace */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "../buffer/buffer.h"
#include "../jsons/parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

#define INVALID_ID          "[ERROR] Invalid type: Book id must be a positive integer.\n"

int is_error(char* message) {
    return strstr(message, "error") != NULL; 
}

void error(const char *msg)
{
    printf("%s\n", msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting.\nMake sure you have an internet connection.");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            printf("No connection.\n");
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

int contains_spaces(char *string) {
    for (int i = 0; i < sizeof(string); i++) {
        if (isspace(string[i])) {
            return 1;
        }
    }
    return 0;
}

char *extract_cookie(char *message) {
    return strtok(strtok(strstr(message, "Set-Cookie:"), "\n") + sizeof("Set-Cokie: "), ";");
}

char *extract_http_nr(char *message) {
    char *copy = (char *)malloc(LINELEN);
    strcpy(copy, message);
    return strtok(copy, "\n");
}

char *extract_token(char *json_token) {
    JSON_Value *root_value = json_value_init_object();
    char *token = NULL;

    root_value = json_parse_string(json_token);
    token = (char *)json_object_get_string(json_object(root_value), "token");

    return token;
}

char *basic_extract_json_array(char *str) {
    return strstr(str, "[");
}

int isnumber(char *str) {
    int length = strlen(str);

    if (length == 0) {
        return 0;
    }

    for (int i = 0; i < length; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }

    return 1;
}

char **process_cookies(char *session_cookie) {
    if (session_cookie == NULL) {
        return NULL;
    }

    char **cookies = (char **)malloc(sizeof(char **));
    cookies[0] = (char *)malloc(LINELEN);
    strcpy(cookies[0], session_cookie);

    return cookies;
}

char *read_and_validate_id(void) {
    char *id = (char *)malloc(LINELEN);

    printf("id=");
    fflush(stdout);
    scanf(" %[^\n]s", id);

    if (!isnumber(id)) {
        printf(INVALID_ID);
    }

    return id;
}

char *process_route(char *url, char *id) {
    char *route_line = (char *)malloc(LINELEN);
    memset(route_line, 0, LINELEN);
    strcpy(route_line, url);
    strcat(route_line, id);

    return route_line;
}