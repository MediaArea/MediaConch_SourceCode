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

    //---------------------------------------------------------------------------
    int Plugin::exec_bin(const std::vector<std::string>& params, std::string& error)
    {
        std::string cmd;
        for (size_t i = 0; i < params.size(); ++i)
        {
            if (i)
                cmd += " ";
            cmd += params[i];
        }

        FILE* pipe = popen(cmd.c_str(), "r");

        if (!pipe)
        {
            error = "Command cannot be executed";
            return -1;
        }

        char buffer[4096];
        while (!feof(pipe))
        {
            if (fgets(buffer, 4096, pipe) != NULL)
                report += buffer;
        }
        pclose(pipe);
        return 0;
    }

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
