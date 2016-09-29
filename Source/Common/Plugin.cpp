/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "Plugin.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#if defined(_WIN32)
#include <ZenLib/Ztring.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#else
#include <stdlib.h>
#include <unistd.h>
extern char **environ;
#include <sys/wait.h>
#endif

//---------------------------------------------------------------------------
namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    Plugin::Plugin()
    {
    }

    //---------------------------------------------------------------------------
    Plugin::~Plugin()
    {
    }

#if defined(_WIN32)

    //---------------------------------------------------------------------------
    int Plugin::exec_bin(const std::vector<std::string>& params, std::string& error)
    {
        std::string cmd;
        for (size_t i = 0; i < params.size(); ++i)
        {
            if (i)
                cmd += " ";
            if (params[i].length() && params[i][0] != '"')
                cmd += "\"" + params[i] + "\"";
            else
                cmd += params[i];
        }

        HANDLE handler_out_rd = NULL;
        HANDLE handler_out_wr = NULL;
        HANDLE handler_err_rd = NULL;
        HANDLE handler_err_wr = NULL;

        if (create_pipe(&handler_out_rd, &handler_out_wr) < 0)
            return -1;

        if (create_pipe(&handler_err_rd, &handler_err_wr) < 0)
            return -1;

        if (execute_the_command(cmd, handler_out_wr, handler_err_wr) < 0)
        {
            error = "Error in command execution";
            return -1;
        }

        if (read_the_output(handler_out_wr, handler_out_rd, true) < 0)
            return -1;

        if (read_the_output(handler_err_wr, handler_err_rd, false) < 0)
            return -1;
        return 0;
    }

    int Plugin::create_pipe(HANDLE* handler_out_rd, HANDLE* handler_out_wr)
    {
        SECURITY_ATTRIBUTES saAttr;

        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        if (!CreatePipe(handler_out_rd, handler_out_wr, &saAttr, 0))
        {
            error = "Cannot create pipe for executing";
            return -1;
        }

        if (!SetHandleInformation(*handler_out_rd, HANDLE_FLAG_INHERIT, 0))
        {
            error = "Stdout SetHandleInformation";
            return -1;
        }

        return 0;
    }

    int Plugin::execute_the_command(std::string& cmd, HANDLE handler_out_wr, HANDLE handler_err_wr)
    {
        PROCESS_INFORMATION piProcInfo = { 0 };
        STARTUPINFO siStartInfo = { 0 };

        siStartInfo.cb = sizeof(STARTUPINFO);
        siStartInfo.hStdError = handler_err_wr;
        siStartInfo.hStdOutput = handler_out_wr;
        siStartInfo.hStdInput = NULL;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        ZenLib::Ztring wcmd = ZenLib::Ztring().From_UTF8(cmd);
        wchar_t buff_cmd[32768] = {0};
        size_t i = 0;
        for (; i < wcmd.length(); ++i)
            buff_cmd[i] = wcmd[i];
        buff_cmd[i] = 0;

        BOOL ret = CreateProcess(NULL, buff_cmd, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo);

        if (!ret)
        {
            error = "cannot execute CreateProcess";
            return -1;
        }

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        return 0;
    }

    int Plugin::read_the_output(HANDLE handler_out_wr, HANDLE handler_out_rd, bool is_out)
    {
        DWORD dwRead;
        CHAR chBuf[4096];
        BOOL ret = FALSE;
        HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        ZenLib::Ztring tmp;

        if (!CloseHandle(handler_out_wr))
        {
            error = "cannot close the pipe write end";
            return -1;
        }

        for (;;)
        {
            ret = ReadFile(handler_out_rd, chBuf, 4096, &dwRead, NULL);
            if (!ret || dwRead == 0)
                break;
            tmp += ZenLib::Ztring(chBuf, dwRead);
        }
        if (is_out)
            report = tmp.To_UTF8();
        else
            report_err = tmp.To_UTF8();

        return 0;
    }

#else

    //---------------------------------------------------------------------------
    int Plugin::exec_bin(const std::vector<std::string>& params, std::string& error)
    {
        if (!params.size())
            return -1;

        int pipe_out_fd[2];
        int pipe_err_fd[2];

        if (pipe(pipe_out_fd) < 0)
        {
            error = "Cannot create the out pipe";
            return -1;
        }

        if (pipe(pipe_err_fd) < 0)
        {
            error = "Cannot create the error pipe";
            return -1;
        }

        pid_t pid = fork();
        if (pid < 0)
        {
            error = "Cannot fork";
            return -1;
        }

        if (pid == (pid_t)0)
        {
            close(pipe_out_fd[0]);
            close(pipe_err_fd[0]);

            dup2(pipe_out_fd[1], STDOUT_FILENO);
            close(pipe_out_fd[1]);

            dup2(pipe_err_fd[1], STDERR_FILENO);
            close(pipe_err_fd[1]);

            const char *bin = params[0].c_str();
            char* args[params.size() + 1];
            size_t i = 0;
            for (; i < params.size(); ++i)
                args[i] = const_cast<char*>(params[i].c_str());
            args[i] = NULL;

            execvpe(bin, args, environ);
            exit(1);
        }
        else
        {
            close(pipe_out_fd[1]);
            close(pipe_err_fd[1]);

            waitpid(pid, NULL, 0);

            int rd = 0;
            char buf[4096];
            for (; 1;)
            {
                rd = read(pipe_out_fd[0], buf, 4095);
                if (rd <= 0)
                    break;

                report += std::string(buf, rd);
            }

            for (; 1;)
            {
                rd = read(pipe_err_fd[0], buf, 4095);
                if (rd <= 0)
                    break;

                report_err += std::string(buf, rd);
            }
        }

        return 0;
    }

#endif

    //---------------------------------------------------------------------------
    int Plugin::read_report(const std::string& file, std::string& report)
    {
        std::ifstream file_handler(file.c_str(), std::ios_base::ate);

        if (!file_handler.is_open())
            return -1;

        int size = file_handler.tellg();
        if (size < 0)
            return -1;

        file_handler.seekg(0, file_handler.beg);

        report.reserve(size);
        report.assign(std::istreambuf_iterator<char>(file_handler),
                     std::istreambuf_iterator<char>());

        file_handler.close();
        return 0;
    }

    //---------------------------------------------------------------------------
    void Plugin::unified_string(std::string& str)
    {
        size_t pos = 0;
        for (;;)
        {
            pos = str.find(" ", pos);
            if (pos == std::string::npos)
                break;
            str.replace(pos, 1, "\\ ");
            pos += 2;
        }
    }

}
