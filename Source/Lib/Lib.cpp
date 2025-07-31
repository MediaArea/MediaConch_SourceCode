/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//---------------------------------------------------------------------------
#include "Lib.h"

#include "Common/MediaConchLib.h"
#include "Common/Reports.h"

#include <ZenLib/Ztring.h>
#include <ZenLib/File.h>
#if !defined(WINDOWS)
    #include <unistd.h>
#else
    #include <windows.h>
#endif //!defined(WINDOWS)
#include <algorithm>

using namespace std;
using namespace ZenLib;
using namespace MediaInfoNameSpace;

//TEMP
#include <iostream>

//---------------------------------------------------------------------------
namespace MediaConch
{
    //***************************************************************************
    // CallBacks
    //***************************************************************************

    //--------------------------------------------------------------------------
    void __stdcall Event_CallBackFunction(unsigned char* Data_Content, size_t Data_Size, void* UserHandle_Void)
    {
        struct MediaInfo_Event_Generic*     Event_Generic=(struct MediaInfo_Event_Generic*)Data_Content;
        unsigned short                      EventID;
        unsigned char                       EventVersion;

        /*integrity tests*/
        if (Data_Size<4)
            return; //There is a problem

        /*Retrieving EventID*/
        EventID     =(unsigned short)((Event_Generic->EventCode&0x00FFFF00)>>8 );
        EventVersion=(unsigned char) ( Event_Generic->EventCode&0x000000FF     );

        switch (EventID)
        {
            case MediaInfo_Event_Global_BytesRead:
                if (EventVersion == 0 && Data_Size >= sizeof(struct MediaInfo_Event_Global_BytesRead_0) && UserHandle_Void != NULL)
                {
                    struct MediaInfo_Event_Global_BytesRead_0* Event_Data=(struct MediaInfo_Event_Global_BytesRead_0*)Data_Content;
                    string* UserHandle=(string*)UserHandle_Void;

                    UserHandle->append((const char*)Event_Data->Content, Event_Data->Content_Size);
                }
                break;
            //TODO:
            //case MediaInfo_Event_Log:
            //    if (EventVersion == 0 && Data_Size >= sizeof(struct MediaInfo_Event_Log_0))
            //        Log_0((struct MediaInfo_Event_Log_0*)Data_Content);
            //    break;
            default: ;
        }
    }

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    Lib::Lib() : force_analyze(false), format(MediaConchLib::format_Max)
    {
        string err;
        if (MCL.create_default_implementation_schema(err) < 0)
        {
            last_error = err.empty() ? "Unknown error while creating default implementation shema" : err;
            return;
        }

        if (MCL.init(err) < 0)
        {
            last_error = err.empty() ? "Unknown error while initializing MediaConch" : err;
            return;
        }
    }

    //---------------------------------------------------------------------------
    Lib::~Lib()
    {
    }

    const string Lib::get_last_error()
    {
        return last_error;
    }

    //***************************************************************************
    // API
    //***************************************************************************

    //---------------------------------------------------------------------------
    void Lib::set_mil_option(const string& name, const string& value)
    {
        last_error = string();

        string report;
        string nc_value = value;
        if (MCL.test_mil_option(name, nc_value, report) < 0)
        {
            last_error = "Unknown option: " + name;
            return;
        }

        options.push_back(make_pair(name, value));
    }

    //---------------------------------------------------------------------------
    void Lib::set_force_analyze(bool force)
    {
        force_analyze = force;
    }

    //---------------------------------------------------------------------------
    void Lib::set_policy_verbosity(int verbosity)
    {
        policy_verbosity = Ztring().From_Number(verbosity).To_UTF8();
    }

    //---------------------------------------------------------------------------
    void Lib::set_implementation_verbosity(int verbosity)
    {
        MCL.set_implementation_verbosity(Ztring().From_Number(verbosity).To_UTF8());
    }

    //---------------------------------------------------------------------------
    long Lib::add_file(const string& filename)
    {
        last_error = string();

        string err;
        long file_id;
        bool registered;
        if (MCL.checker_analyze(-1 /*TODO: users */, filename, vector<string>() /* TODO: plugins*/, options, registered, file_id, err, force_analyze, true /* TODO: mil_analize */) < 0)
        {
            last_error = err.empty() ? "Unknown error while analyzing file: " + filename : err;
            return -1;
        }

        MediaConchLib::report report_kind;
        if (is_ready(file_id, report_kind, err) < 0)
        {
            last_error = err.empty() ? "Unknown error while analyzing file: " + filename : err;
            return -1;
        }

        return file_id;
    }

    //---------------------------------------------------------------------------
    void Lib::add_report(MediaConchLib::report report)
    {
        last_error = string();

        report_set.set(report);
    }

    //---------------------------------------------------------------------------
    void Lib::add_policy(const string& filename)
    {
        last_error = string();

        string content;
        if (!get_file_content(filename, content, last_error))
            return;

        policies.push_back(content);
    }

    //---------------------------------------------------------------------------
    void Lib::set_format(MediaConchLib::format f)
    {
        last_error = string();

        format = f;
    }

    //---------------------------------------------------------------------------
    void Lib::set_display(const string& filename)
    {
        last_error = string();

        string content;
        if (!get_file_content(filename, content, last_error))
            return;

        display = content;
    }

    //---------------------------------------------------------------------------
    const string Lib::get_report(long file_id)
    {
        last_error = string();

        string err;
        if (!MCL.ReportAndFormatCombination_IsValid(vector<string>(), report_set, display, format, err))
        {
            last_error = err.empty() ? "Invalid report and format combination" : err;
            return string();
        }

        //Config
        vector<long> _file_ids;
        if (file_id == -1)
            _file_ids = files;
        else
            _file_ids.push_back(file_id);

        std::bitset<MediaConchLib::report_Max> _report_set = report_set;
        if (_report_set.count() == 0)
            _report_set.set(MediaConchLib::report_MediaConch);

        MediaConchLib::format _format = format;
        if (_format == MediaConchLib::format_Max)
            _format = MediaConchLib::format_Xml;

        //Output
        CheckerReport cr;
        cr.user = -1 ; //TODO: users
        cr.format = _format;
        cr.files = _file_ids;
        cr.report_set = _report_set;
        cr.display_content = &display;
        cr.policies_contents = policies;
        cr.options["verbosity"] = MCL.get_implementation_verbosity();
        if (policy_verbosity.size())
            cr.options["policy_verbosity"] = policy_verbosity;

        //TODO:
        //if (report_set[MediaConchLib::report_MediaInfo] && mi_inform.size())
        //    cr.mi_inform = &mi_inform;
        //cr.format = format;

        MediaConchLib::Checker_ReportRes result;
        if (MCL.checker_get_report(cr, &result, err) < 0)
        {
            last_error = err.empty() ? "Unknown error while retrieving report" : err;
            return string();
        }

        return result.report;
    }

    //***************************************************************************
    // Helpers
    //***************************************************************************

    //---------------------------------------------------------------------------
    bool Lib::get_file_content(const string& filename, string& content, string& err)
    {
        if (filename.find("://") != string::npos && MCL.mil_has_curl_enabled())
        {
            MediaInfo tmpMI;
            tmpMI.Option(__T("ParseSpeed"), __T("1"));

            for(vector<pair<string, string> >::iterator it = options.begin(); it != options.end(); ++it)
            {
                string report;
                if (MCL.test_mil_option(it->first, it->second, report) == 0)
                    tmpMI.Option(Ztring().From_UTF8(it->first), Ztring().From_UTF8(it->second));
            }

            string data;
            stringstream ss;
            ss << "CallBack=memory://" << (int64u)Event_CallBackFunction << ";UserHandler=memory://" << (int64u)&data;
            tmpMI.Option(__T("File_Event_CallBackFunction"), Ztring().From_UTF8(ss.str()));
            tmpMI.Option(__T("Event_CallBackFunction"), Ztring().From_UTF8(ss.str()));
            tmpMI.Open(Ztring().From_UTF8(filename));

            if (data.empty())
            {
                err = "Unable to fetch file: " + filename;
                return false;
            }

            tmpMI.Close();
            tmpMI.Option(__T("ParseSpeed"), __T("0.5")); // Restore default value

            content = data;
        }
        else
        {
            Ztring z_filename = ZenLib::Ztring().From_UTF8(filename);
            if (!File::Exists(z_filename))
            {
               err = "File not found: " + filename;
               return false;
            }

            File file(z_filename);
            int64u size = file.Size_Get();

            if (size == (ZenLib::int64u)-1)
            {
               err = "Unable to open file: " + filename;
               return false;
            }

            int8u* buffer = new int8u[size + 1];

            size_t len = file.Read(buffer, size);
            buffer[len] = '\0';

            file.Close();

            Ztring z_content;
            z_content.From_UTF8((char*)buffer);
            if (z_content.empty())
                z_content.From_Local((char*)buffer);

            delete[] buffer;

            content = z_content.To_UTF8();
        }

        return true;
    }


    //--------------------------------------------------------------------------
    int Lib::is_ready(long file_id, MediaConchLib::report& report_kind, string& err)
    {
        MediaConchLib::Checker_StatusRes res;
        if (MCL.checker_status(-1 /*TODO: users */, file_id, res, err) < 0)
            return -1;

        while (!res.finished)
        {
            #ifdef WINDOWS
                ::Sleep((DWORD)5);
            #else
                usleep(500000);
            #endif
            if (MCL.checker_status(-1 /*TODO: users */, file_id, res, err) < 0)
                return -1;
        }

        string filename;
        if (MCL.checker_file_from_id(-1 /* TODO: users */, file_id, filename, err) < 0)
            return -1;

        if (res.has_error)
        {
            err = "Problem while analyzing file: " + filename + "\n\tError logs are: " + res.error_log;
            return -1;
        }

        if (res.tool && report_kind == MediaConchLib::report_Max)
            report_kind = (MediaConchLib::report)*res.tool;

        if (find(files.begin(), files.end(), file_id) == files.end())
            files.push_back(file_id);
        for (size_t i = 0; i < res.generated_id.size(); ++i)
        {
            if (res.generated_id[i] == -1)
                continue;

            file_id = res.generated_id[i];

            int ret;
            if ((ret = is_ready(file_id, report_kind, err)) <= 0)
                return ret;
        }

        return 1;
    }
}
