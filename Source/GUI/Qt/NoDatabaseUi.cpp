/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifdef __BORLANDC__
    #pragma hdrstop
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "NoDatabaseUi.h"
#include "Common/FileRegistered.h"
#include <sstream>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
namespace MediaConch {

//***************************************************************************
// NoDatabaseUi
//***************************************************************************

//***************************************************************************
// Constructor/Destructor
//***************************************************************************

//---------------------------------------------------------------------------
NoDatabaseUi::NoDatabaseUi() : DatabaseUi()
{
}

//---------------------------------------------------------------------------
NoDatabaseUi::~NoDatabaseUi()
{
}

//---------------------------------------------------------------------------
int NoDatabaseUi::init()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::init_ui()
{
    return init();
}

//---------------------------------------------------------------------------
int NoDatabaseUi::execute()
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_add_file(const FileRegistered*)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_add_files(const std::vector<FileRegistered*>&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_update_file(const FileRegistered*)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_update_files(const std::vector<FileRegistered*>&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_get_file(FileRegistered*)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_remove_file(const FileRegistered*)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_remove_files(const std::vector<FileRegistered*>&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_remove_all_files()
{
    return 0;
}

//---------------------------------------------------------------------------
void NoDatabaseUi::ui_get_elements(std::vector<FileRegistered*>&)
{
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_save_default_policy(const std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_get_default_policy(std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_save_default_display(const std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_get_default_display(std::string&)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_save_default_verbosity(int)
{
    return 0;
}

//---------------------------------------------------------------------------
int NoDatabaseUi::ui_get_default_verbosity(int&)
{
    return 0;
}

}
