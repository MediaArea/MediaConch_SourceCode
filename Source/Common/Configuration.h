/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Configuration functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef ConfigurationH
#define ConfigurationH
//---------------------------------------------------------------------------

#include <string>
#include "Container.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//***************************************************************************
// Class Configuration
//***************************************************************************

static std::string configName("MediaConch.rc");

class Configuration
{
public:
    //Constructor/Destructor
    Configuration();
    virtual ~Configuration();

    void set_file(const std::string& file);
    int parse();

    int get(const std::string& key, std::string &val);
    int get(const std::string& key, long &val);
    int get(const std::string& key, double &val);
    int get(const std::string& key, bool &val);
    int get(const std::string& key);
    int get(const std::string& key, std::vector<Container::Value>& val);
    int get(const std::string& key, std::map<std::string, Container::Value>& val);

    std::string get_error() const;
private:
    Container        *c;
    Container::Value  values;
    std::string       file;
    std::string       error;
    bool              parse_error;
    
    Container::Value *get_value_for_key(const std::string& key);

    Configuration (const Configuration&);
    Configuration& operator=(const Configuration&);
};

}

#endif
