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
    FileRegistered() : policy(-1), display(-1), analyze_percent(0), analyzed(false), implementation_valid(false), policy_valid(false) {}

    std::string  filename;
    std::string  filepath;
    int          policy;
    int          display;

    double       analyze_percent;
    bool         analyzed;
    bool         implementation_valid;
    bool         policy_valid;

    unsigned int index;
};

}

#endif
