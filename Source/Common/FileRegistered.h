/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Core functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef FileRegisteredH
#define FileRegisteredH

//---------------------------------------------------------------------------
#include <string>

namespace MediaConch {

//***************************************************************************
// Struct FileRegistered
//***************************************************************************

class FileRegistered
{
public:
    FileRegistered() : analyze_percent(0), file_id(-1), policy(-1), display(-1), verbosity(-1), report_kind(0), mil_version(0),
        analyzed(false), implementation_valid(false), implementation_has_info(false), implementation_has_warning(false),
        policy_valid(false), policy_has_info(false), policy_has_warning(false), create_policy(false), need_update(true)
    {
    }

    FileRegistered(const FileRegistered& f)
    {
        if (&f == this)
            return;

        this->filename = f.filename;
        this->filepath = f.filepath;
        this->file_id = f.file_id;
        this->policy = f.policy;
        this->display = f.display;
        this->verbosity = f.verbosity;
        this->analyze_percent = f.analyze_percent;
        this->analyzed = f.analyzed;
        this->implementation_valid = f.implementation_valid;
        this->implementation_has_info = f.implementation_has_info;
        this->implementation_has_warning = f.implementation_has_warning;
        this->policy_valid = f.policy_valid;
        this->policy_has_info = f.policy_has_info;
        this->policy_has_warning = f.policy_has_warning;
        this->create_policy = f.create_policy;
        this->index = f.index;
        this->report_kind = f.report_kind;
        this->mil_version = f.mil_version;
        this->need_update = f.need_update;

        for (size_t x = 0; x < f.generated_id.size(); ++x)
            this->generated_id.push_back(f.generated_id[x]);

        for (size_t x = 0; x < f.options.size(); ++x)
        {
            if (f.options[x] != "File_TryToFix")
                this->options.push_back(f.options[x]);
        }
    }

    std::string               filename;
    std::string               filepath;
    std::vector<long>         generated_id;
    std::vector<std::string>  options;

    double                    analyze_percent;
    long                      file_id;
    int                       policy;
    int                       display;
    int                       verbosity;

    int                       report_kind;

    unsigned long long        mil_version;

    unsigned int              index;
    bool                      analyzed;
    bool                      implementation_valid;
    bool                      implementation_has_info;
    bool                      implementation_has_warning;
    bool                      policy_valid;
    bool                      policy_has_info;
    bool                      policy_has_warning;
    bool                      create_policy;
    bool                      need_update;
};

}

#endif
