#include<iostream>
#include<string>
#include<sstream>
#include<queue>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include "shell.h"

using namespace std;

int main()
{
    /*
    ignore SIGTTOU
    when parent is in background process group
    this signal will cause process stop
    */
    signal(SIGTTOU, SIG_IGN);
    signal(SIGINT, SIG_DFL);
    vector<JOB> jobs;
    bool background;
    ENVIRON_CMD environ_cmd;
    JOB_CMD job_cmd;
    while(true)
    {
        setpgrp();
        tcsetpgrp(0,getpgrp());
        tcsetpgrp(1,getpgrp());
        tcsetpgrp(2,getpgrp());
        tcsetpgrp(STDIN_FILENO,getpgrp());
        print_prompt();

        string line;
        getline(cin,line);
        line = strip_spaces(line);

        wait_finished_job(jobs);
        clear_finished_job(jobs);

        if(line=="")
        {
            continue;
        }
        else if(line == "help")
        {
            print_help();
            continue;
        }
        else if(is_environ_cmd(line))
        {
            if(split_environ_cmd(line,environ_cmd) == SYNTAX_ERROR)
            {
                //syntax error
                cout<<"Syntax error"<<endl;
                continue;
            }
            exec_environ_cmd(environ_cmd);
            continue;
        }
        else if(is_job_cmd(line))
        {
            if(split_job_cmd(line,job_cmd) == SYNTAX_ERROR)
            {
                //syntax error
                cout<<"Syntax error"<<endl;
                continue;
            }
            exec_job_cmd(jobs,job_cmd);
            continue;
        }

        if(split_line(line,jobs,background) == SYNTAX_ERROR)
        {
            //syntax error
            cout<<"Syntax error"<<endl;
            continue;
        }

        launch_job(*jobs.rbegin());
        if(!background)
        {
            put_job_foreground(*jobs.rbegin());
            wait_job(jobs,jobs.size()-1);
        }
        else
            wait_finished_job(jobs);
    }
    return 0;
}

