#ifndef _COMMANDS_
#define _COMMANDS_

#define USAGE_COMMANDS      "---Commands---\n-register\n-login\n-enter_library\n-get_books\n-get_book\n-add_book\n-delete_book\n-logout\n-exit\n"
#define INVALID_COMMAND     "[Error] Invalid command: type help to list all possible commands.\n"
#define LOGIN_ERROR         "[Error] Login error: already logged in.\n"

// opens a login/register prompt and sends a POST request to server
char *register_login_prompt(int sockfd, char *host, char *route);

// sends a GET request for library entrance to server
char *enter_library_command(int sockfd, char *host, char *route, char *session_cookie);

// sends a GET request for all books to server
char *get_books_command(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie);

// opens an id prompt and sends a GET request for that book to server
char *get_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie);

// opens a book details prompt and sends a POST request for that book to the server
char *add_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie);

// opens an id prompt and sends a DELETE request for that book to the server
char *delete_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie);

// sends a GET request to log out user to the server
char *logout_command(int sockfd, char *host, char *route, char *session_cookie);

#endif