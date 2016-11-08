/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#include "PluginFileLog.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class PluginFileLog
//***************************************************************************

//---------------------------------------------------------------------------
PluginFileLog::PluginFileLog()
{
    type = MediaConchLib::PLUGIN_LOG;
}

//---------------------------------------------------------------------------
PluginFileLog::~PluginFileLog()
{
    if (this->is_open())
        this->close();
    this->filename.clear();
}

//---------------------------------------------------------------------------
int PluginFileLog::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
{
    if (obj.find("file") == obj.end() || obj.at("file").type != Container::Value::CONTAINER_TYPE_STRING)
    {
        error += "Field 'file' is not present\n";
        return -1;
    }
    this->set_file(obj.at("file").s);

    if (obj.find("level") != obj.end() && obj.at("level").type == Container::Value::CONTAINER_TYPE_STRING)
    {
        std::string level(obj.at("level").s);
        if (level == "nothing")
            this->set_level(PluginLog::LOG_LEVEL_NOTHING);
        else if (level == "all" || level == "debug")
            this->set_level(PluginLog::LOG_LEVEL_DEBUG);
        else if (level == "warning")
            this->set_level(PluginLog::LOG_LEVEL_WARNING);
        else if (level == "error")
            this->set_level(PluginLog::LOG_LEVEL_ERROR);
    }

    return 0;
}

//---------------------------------------------------------------------------
int PluginFileLog::run(std::string& error)
{
    error = "Nothing to do for Plugin Log";
    return -1;
}

//---------------------------------------------------------------------------
void PluginFileLog::set_file(const std::string& file)
{
    file_mutex.Enter();
    this->filename = file;
    file_mutex.Leave();

    this->close();
    this->open();
}

//---------------------------------------------------------------------------
void PluginFileLog::add_log(const std::string& time, int l, const std::string& log)
{
    if (l >= this->level && this->is_open())
    {
        file_mutex.Enter();
        this->file_handle << time << ":" << log << std::endl;
        file_mutex.Leave();
    }
}

//---------------------------------------------------------------------------
bool PluginFileLog::is_open()
{
    if (!this->filename.length())
        return false;

    file_mutex.Enter();
    bool is = this->file_handle.is_open();
    file_mutex.Leave();
    return is;
}

//---------------------------------------------------------------------------
bool PluginFileLog::open()
{
    if (!this->filename.length())
        return false;

    file_mutex.Enter();
    this->file_handle.open(this->filename.c_str(), std::ofstream::out | std::ofstream::app);
    file_mutex.Leave();
    return this->is_open();
}

//---------------------------------------------------------------------------
void PluginFileLog::close()
{
    if (this->is_open())
    {
        file_mutex.Enter();
        this->file_handle.close();
        file_mutex.Leave();
    }
}

}
