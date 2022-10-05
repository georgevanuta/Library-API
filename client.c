
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <arpa/inet.h>
#include "libraries/helpers/helpers.h"
#include "libraries/requests/requests.h"
#include "libraries/jsons/parson.h"
#include "libraries/commands/commands.h"

int main(int argc, char *argv[])
{
    int sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

    // on login
    // off logout
    char *session_cookie = (char *)malloc(sizeof(char *));
    session_cookie = NULL;

    // on enter_library
    // off logout
    char *JWT_token = (char *)malloc(sizeof(char *));
    JWT_token = NULL;

    // response from server
    char *response = (char *)malloc(sizeof(char *));
    response = NULL;

    // read command from stdin
    char command[LINELEN];

    // keeps track whether client is logged
    // into an account or not 
    int is_logged = FALSE;

    printf("--------------- CONNECTION STARTED ---------------\n");

    while (TRUE) {
        // keep connection alive
        sockfd = open_connection(HOST, PORT, AF_INET, SOCK_STREAM, 0);

        scanf("%s", command);

        if (strcmp(command, "exit") == 0) {
            break;
        
        } else if (strcmp(command, "register") == 0) {
            if (is_logged == TRUE) {
                // can't register another account
                // if already logged in
                printf(LOGIN_ERROR);
                continue;
            }

            response = register_login_prompt(sockfd, HOST, REGISTER_ROUTE);

			if (response != NULL) {
	            puts(extract_http_nr(response));
			}
            
        } else if (strcmp(command, "login") == 0) {
            if (is_logged == TRUE) {
                // can't log into another account
                // if already logged in
                printf(LOGIN_ERROR);
                continue;
            }

            response = register_login_prompt(sockfd, HOST, LOGIN_ROUTE);

			if (response != NULL) {
	            puts(extract_http_nr(response));

	            if (!is_error(response)) {
                    // not an error, extract cookie
                    // and set is_logged flag to TRUE
    	            is_logged = TRUE;
        	        session_cookie = extract_cookie(response);
            	}
			}
        
        } else if (strcmp(command, "enter_library") == 0) {
            response = enter_library_command(sockfd, HOST, ENTER_LIBRARY_ROUTE, session_cookie);
            puts(extract_http_nr(response));

			if (!is_error(response)) {
                // not an error, extract JWT token
				JWT_token = extract_token(basic_extract_json_response(response));
			}

        } else if (strcmp(command, "get_books") == 0) {
			response = get_books_command(sockfd, HOST, GET_BOOKS_ROUTE, JWT_token, session_cookie);
			puts(extract_http_nr(response));

			if (!is_error(response)) {
                // not an error, print books
				puts(basic_extract_json_array(response));
			}

        } else if (strcmp(command, "get_book") == 0) {
            response = get_book_prompt(sockfd, HOST, GET_BOOK_ROUTE, JWT_token, session_cookie);
			puts(extract_http_nr(response));

			if (!is_error(response)) {
                // not an error, print book
				puts(basic_extract_json_response(response));
			}

        } else if (strcmp(command, "add_book") == 0) {
            response = add_book_prompt(sockfd, HOST, ADD_BOOK_ROUTE, JWT_token, session_cookie);

			if (response != NULL) {
                // no user input error, print response
				puts(extract_http_nr(response));
			}

        } else if (strcmp(command, "delete_book") == 0) {
			response = delete_book_prompt(sockfd, HOST, DELETE_BOOK_ROUTE, JWT_token, session_cookie);
			
            puts((extract_http_nr(response)));

        } else if (strcmp(command, "logout") == 0) {
			response = logout_command(sockfd, HOST, LOGOUT_ROUTE, session_cookie);
			
            puts(extract_http_nr(response));

			if (!is_error(response)) {
                // user was logged in
                // so logged out successfully
				is_logged = FALSE;
				JWT_token = NULL;
				session_cookie = NULL;  
			}

        } else if (strcmp(command, "help") == 0) {
            // print all commands
            printf(USAGE_COMMANDS);

        } else {
            // invalid command
            printf(INVALID_COMMAND);

        }
    }

    printf("---------------  CONNECTION ENDED  ---------------\n");

    // close socket
    close_connection(sockfd);

    return 0;
}
