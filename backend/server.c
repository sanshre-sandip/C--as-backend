#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <mysql.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "8080"
#define BUFFER_SIZE 4096

// Database configuration
const char *db_host = "localhost";
const char *db_user = "root"; // Update if different
const char *db_pass = "";     // Update if different
const char *db_name = "portfolio_db";

void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int recv_size = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    if (recv_size <= 0) {
        closesocket(client_socket);
        return;
    }
    buffer[recv_size] = '\0';

    printf("Received request: %s\n", buffer);

    // Initial CORS headers to allow React frontend (running on localhost:5173 usually)
    char response_header[1024] = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "Connection: close\r\n\r\n";

    // Simple routing
    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");

    if (method && strcmp(method, "OPTIONS") == 0) {
        send(client_socket, response_header, (int)strlen(response_header), 0);
        closesocket(client_socket);
        return;
    }

    if (path && strcmp(path, "/api/posts") == 0) {
        MYSQL *conn = mysql_init(NULL);
        if (mysql_real_connect(conn, db_host, db_user, db_pass, db_name, 0, NULL, 0)) {
            if (mysql_query(conn, "SELECT id, title, content, created_at FROM posts")) {
                sprintf(response_header, "HTTP/1.1 500 Internal Server Error\r\n\r\n");
                send(client_socket, response_header, (int)strlen(response_header), 0);
            } else {
                MYSQL_RES *result = mysql_store_result(conn);
                int num_fields = mysql_num_fields(result);
                MYSQL_ROW row;
                
                // Manually building JSON (very basic)
                char body[8192] = "[";
                int first = 1;

                while ((row = mysql_fetch_row(result))) {
                    if (!first) strcat(body, ",");
                    char entry[1024];
                    // Escape basic characters if needed, skipping for MVP
                    sprintf(entry, "{\"id\": %s, \"title\": \"%s\", \"content\": \"%s\", \"created_at\": \"%s\"}", 
                        row[0], row[1], row[2], row[3]);
                    strcat(body, entry);
                    first = 0;
                }
                strcat(body, "]");
                
                send(client_socket, response_header, (int)strlen(response_header), 0);
                send(client_socket, body, (int)strlen(body), 0);
                mysql_free_result(result);
            }
            mysql_close(conn);
        } else {
            printf("DB Connection failed: %s\n", mysql_error(conn));
             char *err_resp = "HTTP/1.1 500 DB Error\r\n\r\n";
             send(client_socket, err_resp, (int)strlen(err_resp), 0);
        }
    } else if (path && strcmp(path, "/api/projects") == 0) {
         MYSQL *conn = mysql_init(NULL);
        if (mysql_real_connect(conn, db_host, db_user, db_pass, db_name, 0, NULL, 0)) {
            if (mysql_query(conn, "SELECT id, title, description, url, image_url FROM projects")) {
                 char *err_resp = "HTTP/1.1 500 DB Query Error\r\n\r\n";
                 send(client_socket, err_resp, (int)strlen(err_resp), 0);
            } else {
                MYSQL_RES *result = mysql_store_result(conn);
                MYSQL_ROW row;
                char body[8192] = "[";
                int first = 1;
                while ((row = mysql_fetch_row(result))) {
                    if (!first) strcat(body, ",");
                    char entry[1024];
                    sprintf(entry, "{\"id\": %s, \"title\": \"%s\", \"description\": \"%s\", \"url\": \"%s\", \"image_url\": \"%s\"}", 
                        row[0], row[1], row[2], row[3], row[4]);
                    strcat(body, entry);
                    first = 0;
                }
                strcat(body, "]");
                send(client_socket, response_header, (int)strlen(response_header), 0);
                send(client_socket, body, (int)strlen(body), 0);
                mysql_free_result(result);
            }
             mysql_close(conn);
        } else {
             char *err_resp = "HTTP/1.1 500 DB Connect Error\r\n\r\n";
             send(client_socket, err_resp, (int)strlen(err_resp), 0);
        }
    } else {
        char *not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_socket, not_found, (int)strlen(not_found), 0);
    }
    
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, hints;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) return 1;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(NULL, PORT, &hints, &result);
    if (iResult != 0) {
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %s...\n", PORT);

    while(1) {
        SOCKET ClientSocket = INVALID_SOCKET;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        handle_client(ClientSocket);
    }

    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
