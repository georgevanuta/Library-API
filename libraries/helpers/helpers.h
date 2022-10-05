#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN  4096
#define LINELEN 1000

#define TRUE 	1
#define FALSE 	0

#define HOST "34.241.4.235"
#define PORT 8080

// ----- ROUTES ----- //
#define REGISTER_ROUTE      "/api/v1/tema/auth/register"
#define LOGIN_ROUTE         "/api/v1/tema/auth/login"
#define ENTER_LIBRARY_ROUTE "/api/v1/tema/library/access"
#define GET_BOOKS_ROUTE		"/api/v1/tema/library/books"
#define GET_BOOK_ROUTE		"/api/v1/tema/library/books/"
#define ADD_BOOK_ROUTE		"/api/v1/tema/library/books"
#define LOGOUT_ROUTE		"/api/v1/tema/auth/logout"
#define DELETE_BOOK_ROUTE	"/api/v1/tema/library/books/"

#define TODO printf("Not yet implemented.\n")

// checks whether response from server is an error
int is_error(char* message);

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char *receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

// checks whether string contains any whitespaces
int contains_spaces(char *string);

// extracts cookie from http response
char *extract_cookie(char *message);

// extracts the response number
char *extract_http_nr(char *message);

// extracts the token from a json
char *extract_token(char *json_token);

// extracts the array from a json str
char *basic_extract_json_array(char *str);

// checks whether string is a positive whole number
int isnumber(char *str);

// processes the session cookie
char **process_cookies(char *session_cookie);

// read id and check that it is a valid one
char *read_and_validate_id(void);

// adds id to route
char *process_route(char *url, char *id);

#endif
