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
    FileRegistered() : policy(-1), display(-1), verbosity(-1), analyze_percent(0), analyzed(false),
        implementation_valid(false), policy_valid(false), report_kind(0), need_update(true)
    {
    }

    FileRegistered(const FileRegistered& f)
    {
        if (&f == this)
            return;
        this->filename = f.filename;
        this->filepath = f.filepath;
        this->policy = f.policy;
        this->display = f.display;
        this->verbosity = f.verbosity;
        this->analyze_percent = f.analyze_percent;
        this->analyzed = f.analyzed;
        this->implementation_valid = f.implementation_valid;
        this->policy_valid = f.policy_valid;
        this->index = f.index;
        this->report_kind = f.report_kind;
        this->need_update = f.need_update;
    }

    std::string  filename;
    std::string  filepath;
    int          policy;
    int          display;
    int          verbosity;

    double       analyze_percent;
    bool         analyzed;
    bool         implementation_valid;
    bool         policy_valid;

    int          report_kind;

    unsigned int index;
    bool need_update;
};

}

#endif
