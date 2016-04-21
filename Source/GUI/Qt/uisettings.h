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

    // Last used
    std::string  get_last_policy() const;
    void         change_last_policy(const std::string& policy);
    std::string  get_last_display() const;
    void         change_last_display(const std::string& display);
    int          get_last_verbosity() const;
    void         change_last_verbosity(int verbosity);

private:
    std::string  default_policy;
    std::string  default_display;
    int          default_verbosity;
    std::string  last_policy;
    std::string  last_display;
    int          last_verbosity;
    DatabaseUi  *database;
};

}

#endif // UISETTINGS_H
