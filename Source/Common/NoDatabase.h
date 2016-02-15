/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// NoDatabase functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef NODATABASEH
#define NODATABASEH

//---------------------------------------------------------------------------
#include "Database.h"

//---------------------------------------------------------------------------
namespace MediaConch {

class FileRegistered;

//***************************************************************************
// Class NoDatabase
//***************************************************************************

class NoDatabase : public Database
{
public:
    //Constructor/Destructor
    NoDatabase();
    virtual ~NoDatabase();

    virtual int init();
    virtual int init_report();
    virtual int init_ui();

    // Report
    virtual int create_report_table();
    virtual int update_report_table();
    virtual int save_report(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification, const std::string& report, MediaConchLib::compression);
    virtual int remove_report(const std::string& filename);
    virtual void get_report(MediaConchLib::report reportKind, MediaConchLib::format format,
                            const std::string& filename, const std::string& file_last_modification,
                            std::string& report, MediaConchLib::compression&);
    virtual bool file_is_registered(MediaConchLib::report reportKind, MediaConchLib::format format, const std::string& filename, const std::string& file_last_modification);
    virtual void get_elements(std::vector<std::string>& vec);

    // UI
    virtual int ui_add_file(const FileRegistered* file);
    virtual int ui_update_file(const FileRegistered* file);
    virtual int ui_get_file(FileRegistered* file);
    virtual int ui_remove_file(const FileRegistered* file);
    virtual void ui_get_elements(std::vector<FileRegistered*>& vec);

protected:
    virtual int execute();

private:
    NoDatabase (const NoDatabase&);
    NoDatabase& operator=(const NoDatabase&);

    struct Report
    {
        MediaConchLib::report reportKind;
        MediaConchLib::format format;
        std::string file_last_modification;
        std::string report;
    };

    //TODO UI

    std::map<std::string, std::vector<Report*> > reports_saved;
};

}

#endif /* !NODATABASEH */
