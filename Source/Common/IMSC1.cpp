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

//---------------------------------------------------------------------------
#include "IMSC1.h"
#include "IMSC1/IMSC1Plugin.h"

#include <fstream>
#include <algorithm>

//---------------------------------------------------------------------------

namespace MediaConch {

    //***************************************************************************
    // Constructor/Destructor
    //***************************************************************************

    //---------------------------------------------------------------------------
    IMSC1::IMSC1()
    {
        type = MediaConchLib::PLUGIN_FORMAT;
        report_kind = MediaConchLib::report_MediaImsc1Validation;
    }

    //---------------------------------------------------------------------------
    IMSC1::~IMSC1()
    {
    }

    //---------------------------------------------------------------------------
    IMSC1::IMSC1(const IMSC1& p): PluginFormat(p)
    {
    }

    //---------------------------------------------------------------------------
    int IMSC1::load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error)
    {
        if (obj.find("format") == obj.end() || obj.at("format").type != Container::Value::CONTAINER_TYPE_STRING)
        {
            error += "Field 'format' is not present\n";
            return -1;
        }
        format = obj.at("format").s;

        return 0;
    }

    //---------------------------------------------------------------------------
    int IMSC1::run(std::string& error)
    {
        bool pass = false;
        std::string message;

        std::string schema_dir;
        if (create_report_dir("TTMLTemp", "TTMLSchemaDir", schema_dir) < 0)
        {
            error = "Unable to create temporary directory.";
            return -1;
        }

        IMSC1Plugin plugin(schema_dir);


        if (plugin.validate_ttml_file(filename, pass, message, error))
        {
            delete_report_dir(schema_dir);
            return -1;
        }


        if (plugin.create_report(report, filename, pass, message, error))
        {
            delete_report_dir(schema_dir);
            return -1;
        }

        delete_report_dir(schema_dir);

        return 0;
    }
}
