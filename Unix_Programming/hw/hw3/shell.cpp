#include<iostream>
#include<string>
#include<sstream>
#include<queue>
#include<vector>
#include<cstddef>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<sys/stat.h>
#include "shell.h"
#include<glob.h>
using namespace std;

void print_prompt()
{
    cout<<RED<<"$> "<<NONE;
}

string strip_spaces(string input)
{
    size_t front;
    size_t end;
    string output;

    if((front = input.find_first_not_of(' '))== string::npos)
        front = input.length();

    if((end = input.find_last_not_of(' '))==string::npos)
        end = 0;
    //all spaces
    if(front>end)
        output = "";
    else
        output = input.substr(front, (end-front+1));
    return output;
}
void print_help()
{
    cout<<WHITE<<"help"<<NONE<<endl;
    cout<<"print help messages"<<endl;
    cout<<WHITE<<"jobs"<<NONE<<endl;
    cout<<"print all jobs status"<<endl;
    cout<<WHITE<<"fg"<<NONE<<endl;
    cout<<"move last job to foreground"<<endl;
    cout<<WHITE<<"bg"<<NONE<<endl;
    cout<<"make last job running"<<endl;
    cout<<WHITE<<"fg [ %job_id ]"<<NONE<<endl;
    cout<<"move specific job to foreground"<<endl;
    cout<<WHITE<<"bg [ %job_id ]"<<NONE<<endl;
    cout<<"make specific job running"<<endl;
    cout<<WHITE<<"export [ name ] [ value ]"<<NONE<<endl;
    cout<<"set environment varible name = value"<<endl;
    cout<<WHITE<<"unset [ name ]"<<NONE<<endl;
    cout<<"unset environment varible"<<endl;
}

bool is_environ_cmd(string cmd)
{
    stringstream ss(cmd);
    string temp;
    getline(ss,temp,' ');
    if(temp == "export")
        return true;
    if(temp == "unset")
        return true;
    return false;
}
int split_environ_cmd(string cmd,ENVIRON_CMD& environ_cmd)
{
    stringstream ss(cmd);
    string parameter;
    getline(ss,environ_cmd.cmd,' ');
    if(!getline(ss,environ_cmd.name,' '))
        return 1;
    if(environ_cmd.cmd == "export")
    {
        if(!getline(ss,environ_cmd.value,' '))
            return 1;
    }
    else if(environ_cmd.cmd == "unset")
    {
        return 0;
    }
    return 0;
}
void exec_environ_cmd(ENVIRON_CMD environ_cmd)
{
    if(environ_cmd.cmd == "export")
    {
        setenv(environ_cmd.name.c_str(),environ_cmd.value.c_str(),true);
    }
    else if(environ_cmd.cmd == "unset")
    {
        unsetenv(environ_cmd.name.c_str());
    }
}
bool is_job_cmd(string cmd)
{
    stringstream ss(cmd);
    string temp;
    getline(ss,temp,' ');
    if(temp == "jobs")
        return true;
    if(temp == "fg")
        return true;
    if(temp == "bg")
        return true;
    return false;
}

bool is_digit(string str)
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

int split_job_cmd(string cmd,JOB_CMD& job_cmd)
{
    job_cmd.num = false;
    stringstream ss(cmd);
    string parameter;
    getline(ss,job_cmd.cmd,' ');
    if(getline(ss,parameter,' '))
    {
        size_t num_start;
        num_start=parameter.find('%');
        if(num_start==string::npos)
            return 1;
        string num;
        num = parameter.substr(num_start+1,parameter.length());
        if(!is_digit(num))
            return 1;
        job_cmd.job_num = atoi(num.c_str());
        job_cmd.num = true;
    }
    return 0;
}

void exec_job_cmd(vector<JOB>& jobs,JOB_CMD job_cmd)
{
    if(job_cmd.cmd == "jobs")
    {
        if(jobs.empty())
        {
            cout<<"There are no job"<<endl;
            return;
        }
        for(unsigned int i=0; i<jobs.size(); i++)
            cout<<"["<<i+1<<"]+"<<"\tStopped\t\t"<<jobs[i].original_cmd<<endl;
    }
    else if(job_cmd.cmd == "fg" && !job_cmd.num)
    {
        if(jobs.empty())
        {
            cout<<"There are no job"<<endl;
            return;
        }
        put_job_foreground(*jobs.rbegin());
        wait_job(jobs,jobs.size()-1);
    }
    else if(job_cmd.cmd == "bg" && !job_cmd.num)
    {
        if(jobs.empty())
        {
            cout<<"There are no job"<<endl;
            return;
        }
        put_job_foreground(*jobs.rbegin());
        wait_job(jobs,jobs.size()-1);
    }
    else if(job_cmd.cmd == "fg" && job_cmd.num)
    {
        if(jobs.empty())
        {
            cout<<"There are no job"<<endl;
            return;
        }
        put_job_foreground(jobs[job_cmd.job_num-1]);
        wait_job(jobs,job_cmd.job_num-1);
    }
    else if(job_cmd.cmd == "bg" && job_cmd.num)
    {
        if(jobs.empty())
        {
            cout<<"There are no job"<<endl;
            return;
        }
        put_job_background(jobs[job_cmd.job_num-1]);
        wait_job(jobs,job_cmd.job_num-1);
    }
}

int split_line(string line,vector<JOB>& jobs,bool& background)
{
    string strip_line;
    strip_line = strip_spaces(line);
    if( strip_line == "exit" )
        exit(0);

    background = false;
    if(check_background(line,strip_line,background) == SYNTAX_ERROR)
    {
        return 1;
    }

    //split by '|' character
    stringstream ss(strip_line);
    string temp_str;
    string strip_cmd;
    JOB temp_job;
    CMD temp_cmd;
    vector<string> sub_cmds;
    sub_cmds.clear();

    while(getline(ss,temp_str,'|'))
    {
        //strip spaces from front and end
        string strip_cmd;
        strip_cmd = strip_spaces(temp_str);
        sub_cmds.push_back(strip_cmd);
    }
    //check syntax
    //no empty string in pipeline commands
    for(unsigned int i=0; i<sub_cmds.size(); i++)
    {
        if(sub_cmds[i].empty())
            return 1;
    }

    for(unsigned int i=0; i<sub_cmds.size(); i++)
    {
        temp_cmd.first = false;
        temp_cmd.last = false;
        if(i==0)
            temp_cmd.first = true;
        if(i==sub_cmds.size()-1)
            temp_cmd.last = true;
        temp_cmd.input = 0;
        temp_cmd.output = 1;

        if(redirect_io(sub_cmds[i],temp_cmd,strip_cmd)==SYNTAX_ERROR)
        {
            return 1;
        }


        if(split_argv(strip_cmd,temp_cmd)==SYNTAX_ERROR)
        {
            return 1;
        }
        temp_cmd.complete = false;
        temp_cmd.pid = 0;
        temp_job.cmds.push_back(temp_cmd);
    }
    temp_job.pgid=0;
    temp_job.original_cmd=strip_line;
    jobs.push_back(temp_job);
    return 0;
}

int check_background(string line,string& strip_line,bool & background)
{
    strip_line = strip_spaces(line);
    size_t find;
    find = strip_line.find('&');
    if(find == string::npos)
    {
        background =  false;
        return 0;
    }
    else if(find == strip_line.length()-1)
    {
        strip_line = strip_line.substr(0,strip_line.length()-1);
        background =  true;
        return 0;
    }
    else
    {
        return 1;
    }
}

int redirect_io(string cmd,CMD& temp_cmd,string& strip_cmd)
{
    size_t pattern;
    FILE* fd;
    string filename;

    if(( pattern = cmd.find(">>")) != string::npos )
    {
        //append
        filename = strip_spaces(cmd.substr(pattern+2,cmd.length()));
        if((fd = fopen(filename.c_str(),"a+"))!=NULL)
            temp_cmd.output = fileno(fd);
        strip_cmd =  cmd.substr(0,pattern);
    }
    else if(( pattern = cmd.find(">")) != string::npos )
    {
        //overwrite
        filename = strip_spaces(cmd.substr(pattern+1,cmd.length()));
        if((fd = fopen(filename.c_str(),"w+"))!=NULL)
            temp_cmd.output = fileno(fd);
        strip_cmd =  cmd.substr(0,pattern);
    }
    else if(( pattern = cmd.find("<")) != string::npos )
    {
        //input
        filename = strip_spaces(cmd.substr(pattern+1,cmd.length()));
        if( access( filename.c_str(), F_OK ) == -1 )
        {
            //file non-exist
            return 1;
        }
        if((fd = fopen(filename.c_str(),"r"))!=NULL)
            temp_cmd.input = fileno(fd);
        strip_cmd =  cmd.substr(0,pattern);
    }
    else
    {
        strip_cmd = cmd;
    }
    return 0;
}

int split_argv(string input,CMD& cmd)
{
    stringstream ss(input);
    string temp;
    queue<string> tokens;

    //input.split(' ')
    while(getline(ss,temp,' '))
    {
        if(!is_glob(temp))
            tokens.push(temp);
        else
        {
            glob_t globbuf;
            glob(temp.c_str(), GLOB_TILDE , NULL, &globbuf);
            for(unsigned int i = 0 ; i < globbuf.gl_pathc ; i++)
                tokens.push(string(globbuf.gl_pathv[i]));
            globfree(&globbuf);
        }
    }
    int index = 0;
    char** argv = (char **)malloc((tokens.size()+1)*sizeof(char*));

    while(!tokens.empty())
    {
        temp = tokens.front();
        tokens.pop();
        argv[index]=strdup(temp.c_str());
        index++;
    }
    argv[index] = NULL;
    cmd.argv = argv;
    return 0;
}
bool is_glob(string token)
{
    if(token.find_first_of("*.~?/")!=string::npos)
        return true;
    return false;
}

void launch_job(JOB& job)
{
    int input;
    input = STDIN_FILENO;
    for(unsigned int i=0; i<job.cmds.size(); i++)
    {
        CMD& cur_cmd = job.cmds[i];
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
            cout<<"pipe error"<<endl;
        }
        if(!cur_cmd.first && cur_cmd.input == STDIN_FILENO )
            cur_cmd.input = input;
        if(!cur_cmd.last && cur_cmd.output == STDOUT_FILENO )
            cur_cmd.output = pipefd[1];

        exec_cmd(cur_cmd,job);
        if(input != STDIN_FILENO)
            close(input);
        input = pipefd[0];
        close(pipefd[1]);
    }
    close(input);
}

void put_job_foreground(JOB job)
{
    tcsetpgrp(0,job.pgid);
    tcsetpgrp(1,job.pgid);
    tcsetpgrp(2,job.pgid);
    killpg(job.pgid,SIGCONT);
}
void put_job_background(JOB job)
{
    killpg(job.pgid,SIGCONT);
}

void wait_job(vector<JOB>& jobs,int index)
{
    JOB& job=jobs[index];
    for(unsigned int i=0; i<job.cmds.size(); i++)
    {
        CMD& cmd = job.cmds[i];
        int status;
        waitpid (cmd.pid, &status, 0 | WUNTRACED );
        if (WIFEXITED(status))
        {
            cmd.complete = true;
        }
        else if (WIFSTOPPED (status))
        {
            cout<<"["<<jobs.size()<<"]+"<<"\tStopped\t\t"<<job.original_cmd<<endl;
            break;
        }
    }
}

void exec_cmd(CMD& cmd,JOB& job)
{
    pid_t pid = fork();
    if(pid < 0)
    {
        cout<<"Fork error"<<endl;
    }
    else if(pid == 0)
    {
        //child need to be stop by Ctrl+z
        signal(SIGTTOU, SIG_DFL);
        if(job.pgid)
            setpgid(0,job.pgid);
        else
            setpgrp();
        dup2(cmd.input,STDIN_FILENO);
        dup2(cmd.output,STDOUT_FILENO);

        if(execvp(cmd.argv[0],cmd.argv)<0)
        {
            cout<<cmd.argv[0]<<": Command not found."<<endl;
            exit(0);
        }
    }
    else
    {
        cmd.pid = pid;
        if(job.pgid)
        {
            setpgid(pid,job.pgid);
        }
        else
        {
            setpgid(pid,pid);
            job.pgid=getpgid(pid);
        }
    }
}

void wait_finished_job(vector<JOB>& jobs)
{
    for(vector<JOB>::iterator it = jobs.begin() ; it != jobs.end(); it++)
    {
        JOB& job=*it;
        for(unsigned int i=0; i<job.cmds.size(); i++)
            if(!job.cmds[i].complete)
            {
                int status;
                waitpid(job.cmds[i].pid, &status, 0 | WUNTRACED | WNOHANG);
                if (WIFEXITED(status))
                {
                    job.cmds[i].complete = true;
                }
                else if (WIFSTOPPED (status))
                {
                    continue;
                }
            }
    }
}

void clear_finished_job(vector<JOB>& jobs)
{
    for(vector<JOB>::iterator it = jobs.begin() ; it != jobs.end();)
    {
        bool complete=true;
        JOB& job=*it;
        for(unsigned int i=0; i<job.cmds.size(); i++)
            if(!job.cmds[i].complete)
                complete=false;
        if(complete)
            jobs.erase(it);
        else
            it++;
    }
}
