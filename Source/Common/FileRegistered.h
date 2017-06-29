/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
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
    FileRegistered() : analyze_percent(0), file_id(-1), policy(-1), display(-1), verbosity(-1), report_kind(0),
        analyzed(false), implementation_valid(false), policy_valid(false), create_policy(false), need_update(true)
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
        this->policy_valid = f.policy_valid;
        this->create_policy = f.create_policy;
        this->index = f.index;
        this->report_kind = f.report_kind;
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

    unsigned int              index;
    bool                      analyzed;
    bool                      implementation_valid;
    bool                      policy_valid;
    bool                      create_policy;
    bool                      need_update;
};

}

#endif
