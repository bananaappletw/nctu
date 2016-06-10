#ifndef SHELL_H
#define SHELL_H

#include<iostream>
#include<map>
#define BUF_LEN 1024
#define SHUTDOWN 0
#define ERROR -1
using namespace std;
struct response
{
    string header;
    string content;
    int header_size;
    int content_size;
};
typedef struct response RESPONSE;
void serve(int fd, string dir_root);
int get_request(int fd, string& request);
void parse_request(string request, string& request_header, map<string, string>& parameters);
void deal_request(string dir_root, string request_header, map<string, string> parameters, RESPONSE& response);
int reply_request(int fd, RESPONSE response);
void do_get(string original_path, string path, string& status_string, string& file_extension, RESPONSE& response, map<string, string>parameters, map<string, string>& additional_map);
string get_file_extension(string path);
string get_status_string(int status);
string get_mime_type(string file_extension);
string get_time_string(void);
string get_file_content(string path, int& size);
string get_dir_content(string original_path, string path, int& size);
string get_not_found(int& size);
string get_not_accessible(int& size);
string get_index_error(int& size);
bool is_accessible(string path);
bool is_exist(string path);
bool is_dir(string path);
#endif
