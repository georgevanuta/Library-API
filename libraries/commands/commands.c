#include "commands.h"
#include "../jsons/parson.h"
#include "../buffer/buffer.h"
#include "../helpers/helpers.h"
#include "../requests/requests.h"

#define APPLICATION_JSON    "application/json"

#define INVALID_SPACES      "[ERROR] Invalid username/password: Can't use whitespaces.\n"
#define INVALID_PAGECOUNT   "[ERROR] Invalid type: Pagecount must be a positive integer.\n"

// Used for register/login requests
char *register_login_prompt(int sockfd, char *host, char *route) {

    char *username = (char *)malloc(LINELEN);
    char *password = (char *)malloc(LINELEN);

    // prompt
    printf("username=");
    fflush(stdout);
    scanf(" %[^\n]s", username);

    printf("password=");
    fflush(stdout);
    scanf(" %[^\n]s", password);

    // no whitespaces allowed
    if (contains_spaces(username) || contains_spaces(password)) {
        printf(INVALID_SPACES);
        return NULL;
    }

    // construct json
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;
    
    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char **form_data = (char **)calloc(1, sizeof(char *));
    form_data[0] = (char *)calloc(LINELEN, sizeof(char));
    strcpy(form_data[0], serialized_string);

    // send POST request
    char *message = compute_post_request(host, route, APPLICATION_JSON, form_data, 1, NULL, NULL, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}

// Used to gain acces to the library
char *enter_library_command(int sockfd, char *host, char *route, char *session_cookie) {
    // must have session_cookie
    char **cookies = process_cookies(session_cookie);
    
    // send GET request
    char *message = compute_get_request(host, route, NULL, NULL, cookies, 1);
    send_to_server(sockfd, message);
    
    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}

// Needs session cookie and JWT token, returns all books for user
char *get_books_command(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie) {
    // process session cookie
    char **cookies = process_cookies(session_cookie);

    // send GET request
    char *message = compute_get_request(host, route, NULL, JWT_token, cookies, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;

}

// Needs session cookie and JWT token, return book for provided id
char *get_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie) {
    // process session cookie
    char **cookies = process_cookies(session_cookie);

    // prompt
    char *id = read_and_validate_id();

    // process url
    char *route_line = process_route(route, id);

    // send GET request
    char *message = compute_get_request(host, route_line, NULL, JWT_token, cookies, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}

// Needs session cookie and JWT token, adds book for that user
char *add_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie) {
    // processs session cookie
    char **cookies = process_cookies(session_cookie);

    char *title = (char *)malloc(LINELEN);
    char *author = (char *)malloc(LINELEN);
    char *genre = (char *)malloc(LINELEN);
    char *publisher = (char *)malloc(LINELEN);
    char *pagecount = (char *)malloc(LINELEN);

    // prompt
    printf("title=");
    fflush(stdout);
    scanf(" %[^\n]s", title);

    printf("author=");
    fflush(stdout);
    scanf(" %[^\n]s", author);

    printf("genre=");
    fflush(stdout);
    scanf(" %[^\n]s", genre);

    printf("publisher=");
    fflush(stdout);
    scanf(" %[^\n]s", publisher);

    printf("pagecount=");
    fflush(stdout);
    scanf(" %[^\n]s", pagecount);

    // check wheter provided pagecount is valid
    if (!isnumber(pagecount)) {
        printf(INVALID_PAGECOUNT);
        return NULL;
    }

    // construct json
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "genre", genre);
    json_object_set_number(root_object, "page_count", atoi(pagecount));
    json_object_set_string(root_object, "publisher", publisher);
    serialized_string = json_serialize_to_string_pretty(root_value);

    char **form_data = (char **)calloc(1, sizeof(char *));
    form_data[0] = (char *)calloc(LINELEN, sizeof(char));
    strcpy(form_data[0], serialized_string);

    // send POST request
    char *message = compute_post_request(host, route, APPLICATION_JSON, form_data, 1, JWT_token, cookies, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}

// Needs session cookie and JWT token, deletes book for provided id
char *delete_book_prompt(int sockfd, char *host, char *route, char *JWT_token, char *session_cookie) {
    // must have session cookie
    char **cookies = process_cookies(session_cookie);
    
    // prompt
    char *id = read_and_validate_id();

    // process url
    char *route_line = process_route(route, id);

    // send DELETE request
    char *message = compute_delete_request(host, route_line, JWT_token, cookies, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}

// Needs session cookie, logs out user
char *logout_command(int sockfd, char *host, char *route, char *session_cookie) {
    // must have session cookie
    char **cookies = process_cookies(session_cookie);

    // send GET request
    char *message = compute_get_request(host, route, NULL, NULL, cookies, 1);
    send_to_server(sockfd, message);

    // receive response from server
    char *response = receive_from_server(sockfd);

    return response;
}