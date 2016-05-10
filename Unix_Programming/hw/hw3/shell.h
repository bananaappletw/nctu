#ifndef SHELL_H
#define SHELL_H

#include<iostream>
#include<string>
#include<sstream>
#include<queue>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

#define SYNTAX_ERROR 1
#define NONE "\033[m"
#define WHITE "\033[1;37m"
#define RED "\033[0;32;31m"

using namespace std;

typedef struct cmd
{
    int pid;
    char* const * argv;
    int input;
    int output;
    bool first;
    bool last;
    bool complete;
} CMD;

typedef struct job
{
    pid_t pgid;
    string original_cmd;
    vector<CMD> cmds;
} JOB;

typedef struct job_cmd
{
    string cmd;
    int job_num;
    bool num;
} JOB_CMD;

typedef struct environ_cmd
{
    string cmd;
    string name;
    string value;
} ENVIRON_CMD;

void print_prompt();
string strip_spaces(string input);
void print_help();
bool is_environ_cmd(string cmd);
int split_environ_cmd(string cmd,ENVIRON_CMD& environ_cmd);
void exec_environ_cmd(ENVIRON_CMD environ_cmd);
bool is_job_cmd(string cmd);
bool is_digit(string str);
int split_job_cmd(string cmd,JOB_CMD& job_cmd);
void exec_job_cmd(vector<JOB>& jobs,JOB_CMD job_cmd);
int split_line(string line,vector<JOB>& jobs,bool& background);
int check_background(string line,string & strip_line,bool& background);
int redirect_io(string,CMD& cmd,string &);
int split_argv(string input,CMD& cmd);
bool is_glob(string token);
void launch_job(JOB& job);
void put_job_foreground(JOB job);
void put_job_background(JOB job);
void wait_job(vector<JOB>& jobs,int index);
void exec_cmd(CMD& cmd,JOB& job);
void wait_finished_job(vector<JOB>& jobs);
void clear_finished_job(vector<JOB>& jobs);

#endif
