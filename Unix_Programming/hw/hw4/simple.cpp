#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<map>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include <errno.h>
#include<dirent.h>
#include "simple.h"
using namespace std;

void serve(int fd, string dir_root)
{
    int status;
    for (;;)
    {
        string request;
        string request_header;
        map<string, string> parameters;
        string response_header;
        string response_content;
        status = get_request(fd, request);
        if (status == SHUTDOWN)
        {
            cout << "Peer Close Connection." << endl;
            return;
        }
        else if (status == ERROR)
        {
            cout << "Error happens." << endl;
            return;
        }
        request_header.clear();
        parameters.clear();
        parse_request(request, request_header, parameters);
        cout << request_header << endl;
        RESPONSE response;
        response.header.clear();
        response.content.clear();
        response.header_size = 0;
        response.content_size = 0;
        deal_request(dir_root, request_header, parameters, response);
        status = reply_request(fd, response);
        cout << response.header << endl;
        if (status == ERROR)
        {
            cout << "Error happens." << endl;
            return;
        }
    }
}

int get_request(int fd, string& request)
{
    int ret;
    char buf[BUF_LEN];
    ret = recv(fd, buf, BUF_LEN, 0);
    if (ret == 0)
        return SHUTDOWN;
    else if (ret == -1)
        return ERROR;
    request = string(buf);
    return ret;
}

void parse_request(string request, string& request_header, map<string, string>& parameters)
{
    stringstream ss(request);
    getline(ss, request_header);
    string line;
    while (getline(ss, line))
    {
        stringstream ss_parameter(line);
        string key, value;
        if (ss_parameter >> key >> value)
        {
            key = key.substr(0, key.length() - 1);
            parameters[key] = value;
        }
    }
}



void deal_request(string dir_root, string request_header, map<string, string> parameters, RESPONSE& response)
{
    string method;
    string path;
    string version;
    string file_extension;
    string status_string;
    map<string, string> additional_map;
    additional_map.clear();
    stringstream ss(request_header);
    ss >> method >> path >> version;
    string original_path = path;
    size_t pos = path.find("?");
    if (pos != string::npos)
    {
        path = path.substr(0, pos);
    }
    path = dir_root + path;
    if (method == "GET")
        do_get(original_path, path, status_string, file_extension, response, parameters, additional_map);
    else
    {
        exit(0);
    }
    stringstream ss_header;
    ss_header << "HTTP/1.1 " << status_string << "\r" << endl;
    ss_header << "Server: " << "bananaapple" << "\r" << endl;
    ss_header << "Date: " << get_time_string() << "\r" << endl;
    ss_header << "Content-Type: " << get_mime_type(file_extension) << "\r" << endl;
    ss_header << "Content-Length: " << response.content_size << "\r" << endl;
    map<string, string>::iterator it;
    for (it = additional_map.begin(); it != additional_map.end(); it++)
        ss_header << it->first << ": " << it->second << "\r" << endl;
    ss_header << "\r" << endl;
    response.header = ss_header.str();
    response.header_size = response.header.length();
}

int reply_request(int fd, RESPONSE response)
{
    int ret = 0;
    int ret1 = 0;
    int ret2 = 0;
    while (ret1 != response.header_size)
    {
        ret = write(fd, response.header.c_str(), response.header_size);
        if (ret == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return ERROR;
        }
        ret1 += ret;
    }
    while (ret2 != response.content_size)
    {
        ret = write(fd, response.content.c_str(), response.content_size);
        if (ret == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return ERROR;
        }
        ret2 += ret;
    }
    return ret1 + ret2;
}

void do_get(string original_path, string path, string& status_string, string& file_extension, RESPONSE& response, map<string, string>parameters, map<string, string>& additional_map)
{
    if (!is_exist(path))
    {
        //file non-exist
        status_string = "403 FORBIDDEN MISSING FILE";
        file_extension = "html";
        response.content = get_not_found(response.content_size);
    }
    else if (!is_accessible(path))
    {
        //file unaccessible
        status_string = "404 NOT FOUND PERMISSION DENIED";
        file_extension = "html";
        response.content = get_not_accessible(response.content_size);
    }
    else if (*path.rbegin() == '/')
    {
        //directory
        if (is_accessible(path + "index.html"))
        {
            //read index.html
            status_string = "200 OK";
            file_extension = "html";
            response.content = get_file_content(path + "index.html", response.content_size);
        }
        else if (is_accessible(path + "index.htm"))
        {
            //read index.htm
            status_string = "200 OK";
            file_extension = "html";
            response.content = get_file_content(path + "index.htm", response.content_size);
        }
        else if (is_exist(path + "index.html") || is_exist(path + "index.htm"))
        {
            status_string = "403 FORBIDDEN";
            file_extension = "html";
            response.content = get_index_error(response.content_size);
        }
        else
        {
            //gererate index.html
            status_string = "200 OK";
            file_extension = "html";
            response.content = get_dir_content(original_path, path, response.content_size);
        }
    }
    else
    {
        //file
        if (is_dir(path))
        {
            //without slash at end but is directory
            status_string = "301 Moved Permanently";
            file_extension = "html";
            string strip_path;
            size_t pos = original_path.find("?");
            if (pos != string::npos)
                strip_path = original_path.substr(0, pos) + "/" + original_path.substr(pos, original_path.length());
            else
                strip_path = original_path + "/";
            additional_map["Location"] = "http://" + parameters["Host"] + strip_path;
        }
        else
        {
            //normal file
            status_string = "200 OK";
            file_extension = get_file_extension(path);
            response.content = get_file_content(path, response.content_size);
        }
    }
}
string get_file_extension(string path)
{
    size_t pos = path.find_last_of('.');
    return path.substr(pos + 1, path.length());
}
string get_status_string(int status)
{
    if (status == 200)
        return "200 OK";
    if (status == 403)
        return "403 Forbidden";
    if (status == 404)
        return "404 Not Found";
    if (status == 301)
        return "301 Moved Permanently";
    if (status == 302)
        return "302 Found";
    return "500 Internal Server Error";
}
string get_mime_type(string file_extension)
{
    if (file_extension == "html" || file_extension == "htm")
        return "text/html";
    if (file_extension == "txt")
        return "text/plain";
    if (file_extension == "css")
        return "text/css";
    if (file_extension == "gif")
        return "image/gif";
    if (file_extension == "jpg")
        return "image/jpeg";
    if (file_extension == "png")
        return "image/png";
    if (file_extension == "bmp")
        return "image/x-ms-bmp";
    if (file_extension == "doc")
        return "application/msword";
    if (file_extension == "pdf")
        return "application/pdf";
    if (file_extension == "mp4")
        return "video/mp4";
    if (file_extension == "swf" || file_extension == "swfl")
        return "application/x-shockwave-flash";
    if (file_extension == "ogg")
        return "audio/ogg";
    if (file_extension == "bz2")
        return "application/x-bzip2";
    if (file_extension == "gz" || file_extension == "tar.gz")
        return "application/x-gzip";
    return "";
}
string get_time_string(void)
{
    char buf[BUF_LEN];
    time_t timer;
    struct tm * ptm;
    timer = time(NULL);
    ptm = gmtime(&timer);
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %Z", ptm);
    return string(buf);
}
string get_file_content(string path, int& size)
{
    char* buf;
    ifstream ifs;
    ifs.open(path.c_str(), ifstream::in | ios::binary);
    ifs.seekg(0, ios::end);
    size = ifs.tellg();
    ifs.seekg(0, ios::beg);
    buf = (char*) calloc(size, sizeof(char));
    ifs.read(&buf[0], size);
    string ret = string(buf, size);
    free(buf);
    return ret;
    /*
    ifstream infile { string.c_str() };
    string file_contents { istreambuf_iterator<char>(infile), istreambuf_iterator<char>() };
    return file_contents
    */
}
string get_dir_content(string original_path, string path, int& size)
{
    stringstream ss;
    ss << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"><html>" << endl;
    ss << "<title>Directory listing for " << original_path << "</title>" << endl;
    ss << "<body>" << endl;
    ss << "<h2>Directory listing for " << original_path << "</h2>" << endl;
    ss << "<hr>" << endl;
    ss << "<ul>" << endl;
    struct dirent *direntptr;
    DIR *dirptr;
    dirptr = opendir(path.c_str());
    if (dirptr)
        while ((direntptr = readdir(dirptr)) != NULL)
            if (strcmp(direntptr->d_name, "..") && strcmp(direntptr->d_name, "."))
            {
                ss << "<li>";
                ss << "<a href=\"";
                ss << direntptr->d_name;
                ss << "\">";
                ss << direntptr->d_name;
                ss << "</a>" << endl;
                ss << "</li>";
            }
    closedir(dirptr);
    ss << "</ul>" << endl;
    ss << "<hr>" << endl;
    ss << "<body>" << endl;
    ss << "</html>" << endl;
    string s = ss.str();
    size = s.length();
    return s;
}

string get_not_found(int& size)
{
    stringstream ss;
    ss << "<head>" << endl;
    ss << "<title>Error response</title>" << endl;
    ss << "</head>" << endl;
    ss << "<body>" << endl;
    ss << "<h1>Error response</h1>" << endl;
    ss << "<p>Error code 403." << endl;
    ss << "<p>Message: File not found." << endl;
    ss << "<p>Error code explanation: 403 = Nothing matches the given URI." << endl;
    ss << "</body>" << endl;
    size = ss.str().length();
    return ss.str();
}

string get_not_accessible(int& size)
{
    stringstream ss;
    ss << "<head>" << endl;
    ss << "<title>Error response</title>" << endl;
    ss << "</head>" << endl;
    ss << "<body>" << endl;
    ss << "<h1>Error response</h1>" << endl;
    ss << "<p>Error code 404." << endl;
    ss << "<p>Message: Permission denied." << endl;
    ss << "<p>Error code explanation: 404 = You don't have permission." << endl;
    ss << "</body>" << endl;
    size = ss.str().length();
    return ss.str();
}

string get_index_error(int& size)
{
    stringstream ss;
    ss << "<head>" << endl;
    ss << "<title>Error response</title>" << endl;
    ss << "</head>" << endl;
    ss << "<body>" << endl;
    ss << "<h1>Error response</h1>" << endl;
    ss << "<p>Error code 403." << endl;
    ss << "<p>Message: Index file permission not correct." << endl;
    ss << "<p>Error code explanation: 404 = Index file is forbidden" << endl;
    ss << "</body>" << endl;
    size = ss.str().length();
    return ss.str();
}
bool is_accessible(string path)
{
    if (!access(path.c_str(), R_OK))
        return true;
    return false;
}

bool is_exist(string path)
{
    if (!access(path.c_str(), F_OK))
        return true;
    return false;
}

bool is_dir(string path)
{
    struct stat buf;
    if (!stat(path.c_str(), &buf) && S_ISDIR(buf.st_mode))
        return true;
    return false;
}
