/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Scheduler functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef IMSC1H
#define IMSC1H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include <map>
#include "MediaConchLib.h"
#include "PluginFormat.h"

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// Class VeraPDF
//***************************************************************************

class IMSC1 : public PluginFormat
{
public:
    IMSC1();
    virtual ~IMSC1();
    IMSC1(const IMSC1&);

    virtual int load_plugin(const std::map<std::string, Container::Value>& obj, std::string& error);
    virtual int run(std::string& error);

private:
    IMSC1&    operator=(const IMSC1&);
};

}

#endif // !IMSC1H
