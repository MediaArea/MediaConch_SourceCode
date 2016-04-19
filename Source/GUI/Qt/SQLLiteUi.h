/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// SQLLite functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef HAVE_SQLITE

//---------------------------------------------------------------------------
#ifndef SQLLiteUiH
#define SQLLiteUiH
//---------------------------------------------------------------------------

#include "DatabaseUi.h"
#include "Common/SQLLite.h"
#include <sqlite3.h>
//---------------------------------------------------------------------------

namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class SQLLite
//***************************************************************************

class SQLLiteUi : public DatabaseUi, public SQLLite
{
public:
    //Constructor/Destructor
    SQLLiteUi();
    virtual ~SQLLiteUi();

    // General
    virtual int init();
    virtual int init_ui();

    // UI Settings
    virtual int create_ui_settings_table();

    virtual int ui_save_default_policy(const std::string&);
    virtual int ui_get_default_policy(std::string&);

    virtual int ui_save_default_display(const std::string&);
    virtual int ui_get_default_display(std::string&);

    virtual int ui_save_default_verbosity(int);
    virtual int ui_get_default_verbosity(int&);

    // UI
    virtual int create_ui_table();
    virtual int update_ui_table();
    virtual int ui_add_file(const FileRegistered* file);
    virtual int ui_add_files(const std::vector<FileRegistered*>& files);
    virtual int ui_update_file(const FileRegistered* file);
    virtual int ui_update_files(const std::vector<FileRegistered*>& files);
    virtual int ui_get_file(FileRegistered* file);
    virtual int ui_remove_file(const FileRegistered* file);
    virtual int ui_remove_files(const std::vector<FileRegistered*>& files);
    virtual int ui_remove_all_files();
    virtual void ui_get_elements(std::vector<FileRegistered*>& vec);

private:
    int           ui_version;
    static int    current_ui_version;

    SQLLiteUi (const SQLLiteUi&);
    SQLLiteUi& operator=(const SQLLiteUi&);
};

}

#endif /* !SQLLiteUiH */

#endif /* !HAVE_SQLITE */
