/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "uisettings.h"
#include "DatabaseUi.h"

namespace MediaConch {

//***************************************************************************
// Constructor / Desructor
//***************************************************************************

UiSettings::UiSettings(DatabaseUi* db) : database(db)
{
}

UiSettings::~UiSettings()
{
}

//***************************************************************************
// Functions
//***************************************************************************

// General
//---------------------------------------------------------------------------
void UiSettings::init()
{
}

// Default
//---------------------------------------------------------------------------
std::string UiSettings::get_default_policy() const
{
    return default_policy;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_policy(const std::string& policy)
{
    if (default_policy == policy)
        return;

    default_policy = policy;
    database->ui_save_default_policy(policy);
}

//---------------------------------------------------------------------------
std::string UiSettings::get_default_display() const
{
    return default_display;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_display(const std::string& display)
{
    if (default_display == display)
        return;

    default_display = display;
    database->ui_save_default_display(display);
}

//---------------------------------------------------------------------------
int UiSettings::get_default_verbosity() const
{
    return default_verbosity;
}

//---------------------------------------------------------------------------
void UiSettings::change_default_verbosity(int& verbosity)
{
    if (default_verbosity == verbosity)
        return;

    default_verbosity = verbosity;
    database->ui_save_default_verbosity(verbosity);
}

}
