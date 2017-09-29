#ifndef SERVER_H
#define SERVER_H

void server();
int create_socket();
bool socket_has_data();
std::string read_client_message(int& client_fd);
void handle_request(std::string message, FILE* &client_stream);
void block_child();
void unblock_child();

#endif
