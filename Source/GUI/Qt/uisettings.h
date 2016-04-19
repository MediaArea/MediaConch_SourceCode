/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef UISETTINGS_H
#define UISETTINGS_H

#include <string>

//---------------------------------------------------------------------------
namespace MediaConch {

class DatabaseUi;

class UiSettings
{
public:
    UiSettings();
    ~UiSettings();
    
//***************************************************************************
// Functions
//***************************************************************************
    //General
    int          init();
    void         set_database(DatabaseUi* db);

    // Default
    std::string  get_default_policy() const;
    void         change_default_policy(const std::string& policy);
    std::string  get_default_display() const;
    void         change_default_display(const std::string& display);
    int          get_default_verbosity() const;
    void         change_default_verbosity(int verbosity);

private:
    std::string  default_policy;
    std::string  default_display;
    int          default_verbosity;
    DatabaseUi  *database;
};

}

#endif // UISETTINGS_H
