/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// RESTAPI functions
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifndef RESTAPIH
#define RESTAPIH
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include <utility>
#include "Container.h"
#include "MediaConchLib.h"
//---------------------------------------------------------------------------

namespace MediaConch {

//---------------------------------------------------------------------------

//***************************************************************************
// Class RESTAPI
//***************************************************************************

class RESTAPI
{
public:

    static const std::string API_VERSION;

    enum Reason
    {
        NO_REASON = 0,
        FILE_NOT_EXISTING,
        ID_NOT_EXISTING,
        NOT_READY,
    };

    enum Report
    {
        NO_REPORT = 0,
        POLICY,
        IMPLEMENTATION,
        MEDIAINFO,
        MEDIATRACE,
        VERAPDF,
        DPFMANAGER,
    };

    std::string get_Report_string(Report r)
    {
#define ReportString(report) \
        case report:         \
            return #report;

        switch (r)
        {
            ReportString(POLICY);
            ReportString(IMPLEMENTATION);
            ReportString(MEDIAINFO);
            ReportString(MEDIATRACE);
            ReportString(VERAPDF);
            ReportString(DPFMANAGER);
            default:
                break;
        }
#undef ReportString
        return std::string();
    }

    Report string_to_Report(const std::string& str)
    {
#define ReportString(report)       \
        if (!str.compare(#report)) \
            return report;

        ReportString(POLICY);
        ReportString(IMPLEMENTATION);
        ReportString(MEDIAINFO);
        ReportString(MEDIATRACE);
        ReportString(VERAPDF);
        ReportString(DPFMANAGER);

#undef ReportString

        return NO_REPORT;
    }

    std::string get_Reason_string(Reason r)
    {
#define ReasonString(reason) \
        case reason:         \
            return #reason;

        switch (r)
        {
            ReasonString(NO_REASON);
            ReasonString(FILE_NOT_EXISTING);
            ReasonString(ID_NOT_EXISTING);
            ReasonString(NOT_READY);
            default:
                break;
        }
#undef ReasonString
        return std::string();
    }

    Reason string_to_Reason(const std::string& str)
    {
#define ReasonString(reason)       \
        if (!str.compare(#reason)) \
            return reason;

        ReasonString(NO_REASON);
        ReasonString(FILE_NOT_EXISTING);
        ReasonString(ID_NOT_EXISTING);
        ReasonString(NOT_READY);

#undef ReasonString

        return NO_REASON;
    }

//***************************************************************************
// MediaConch General
//***************************************************************************

    // Generic error
    struct MediaConch_Nok
    {
        MediaConch_Nok() : id(NULL) {}
        ~MediaConch_Nok();

        long        *id;
        std::string  error;
        std::string  to_str() const;
    };

    // Get Plugins
    struct MediaConch_Get_Plugins_Req
    {
        std::string  to_str() const;
    };

    struct MediaConch_Get_Plugins_Res
    {
        MediaConch_Get_Plugins_Res() : nok(NULL) {}
        ~MediaConch_Get_Plugins_Res();

        std::vector<std::string>         plugins;
        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // List Watch Folder
    struct MediaConch_List_Watch_Folders_Req
    {
        std::string  to_str() const;
    };

    struct MediaConch_List_Watch_Folders_Res
    {
        MediaConch_List_Watch_Folders_Res() : nok(NULL) {}
        ~MediaConch_List_Watch_Folders_Res();

        std::vector<std::string>         folders;
        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // Watch Folder
    struct MediaConch_Watch_Folder_Req
    {
        MediaConch_Watch_Folder_Req() : user(NULL), recursive(true) {}
        ~MediaConch_Watch_Folder_Req();

        std::string                                       to_str() const;

        std::string                                       folder;
        std::string                                       folder_reports;
        std::vector<std::string>                          plugins;
        std::vector<std::string>                          policies;
        std::vector<std::pair<std::string,std::string> >  options;
        long                                             *user;
        bool                                              recursive;
    };

    struct MediaConch_Watch_Folder_Res
    {
        MediaConch_Watch_Folder_Res() : nok(NULL) {}
        ~MediaConch_Watch_Folder_Res();

        long                             user;
        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // Edit Watch Folder
    struct MediaConch_Edit_Watch_Folder_Req
    {
        std::string  to_str() const;

        std::string  folder;
        std::string  folder_reports;
    };

    struct MediaConch_Edit_Watch_Folder_Res
    {
        MediaConch_Edit_Watch_Folder_Res() : nok(NULL) {}
        ~MediaConch_Edit_Watch_Folder_Res();

        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // Remove Watch Folder
    struct MediaConch_Remove_Watch_Folder_Req
    {
        std::string  to_str() const;

        std::string  folder;
    };

    struct MediaConch_Remove_Watch_Folder_Res
    {
        MediaConch_Remove_Watch_Folder_Res() : nok(NULL) {}
        ~MediaConch_Remove_Watch_Folder_Res();

        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

//***************************************************************************
// Checker
//***************************************************************************

    // Analyze
    struct Checker_Analyze_Arg
    {
        Checker_Analyze_Arg() : user(-1), has_force_analyze(false), mil_analyze(true) {}

        std::string              to_str() const;
        std::string              file;
        int                      user;
        int                      id;
        std::vector<std::string> plugins;
        std::vector<std::pair<std::string,std::string> > options;
        bool                     has_force_analyze;
        bool                     force_analyze;
        bool                     mil_analyze;
    };

    struct Checker_Analyze_Req
    {
        std::vector<Checker_Analyze_Arg> args;
        std::string                      to_str() const;
    };

    struct Checker_Analyze_Ok
    {
        long                   inId;
        long                   outId;
        bool                   create;
        std::string            to_str() const;
    };

    struct Checker_Analyze_Res
    {
        ~Checker_Analyze_Res();
        std::vector<Checker_Analyze_Ok*>  ok;
        std::vector<MediaConch_Nok*>      nok;
        std::string                       to_str() const;
    };

    // Status
    struct Checker_Status_Req
    {
        Checker_Status_Req() :  user(-1) {}

        int                     user;
        std::vector<long>       ids;
        std::string             to_str() const;
    };

    struct Checker_Status_Ok
    {
        Checker_Status_Ok() : finished(false), has_error(false), percent(NULL), tool(NULL), source_id(-1) {}
        ~Checker_Status_Ok();

        long                     id;
        bool                     finished;
        bool                     has_error;
        std::string              error_log;
        double                  *percent;
        Report                  *tool;
        std::vector<long>        generated_id;
        long                     source_id;
        std::string              to_str() const;
    };

    struct Checker_Status_Res
    {
        ~Checker_Status_Res();
        std::vector<Checker_Status_Ok*>  ok;
        std::vector<MediaConch_Nok*>     nok;
        std::string                      to_str() const;
    };

    // Report
    struct Checker_Report_Req
    {
        Checker_Report_Req() :             user(-1) {}

        int                                user;
        std::vector<long>                  ids;
        std::vector<Report>                reports;
        std::vector<size_t>                policies_ids;
        std::vector<std::string>           policies_contents;
        std::string                        display_name;
        std::string                        display_content;
        std::map<std::string, std::string> options;
        std::string                        to_str() const;
    };

    struct Checker_Report_Ok
    {
        Checker_Report_Ok() :   has_valid(false) {}
        std::string             report;
        bool                    has_valid;
        bool                    valid;
        std::string             to_str() const;
    };

    struct Checker_Report_Res
    {
        Checker_Report_Res() : ok(NULL), nok(NULL) {}
        ~Checker_Report_Res();

        Checker_Report_Ok               *ok;
        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // Clear
    struct Checker_Clear_Req
    {
        Checker_Clear_Req() :   user(-1) {}

        int                     user;
        std::vector<long>       ids;
        std::string             to_str() const;
    };

    struct Checker_Clear_Res
    {
        ~Checker_Clear_Res();

        std::vector<long>               ok;
        std::vector<MediaConch_Nok*>    nok;
        std::string                     to_str() const;
    };

    // Stop
    struct Checker_Stop_Req
    {
        Checker_Stop_Req() :   user(-1) {}

        int                     user;
        std::vector<long>       ids;
        std::string             to_str() const;
    };

    struct Checker_Stop_Res
    {
        ~Checker_Stop_Res();

        std::vector<long>               ok;
        std::vector<MediaConch_Nok*>    nok;
        std::string                     to_str() const;
    };

    // List
    struct Checker_List_Req
    {
        Checker_List_Req() :   user(-1) {}

        int                    user;
        std::string            to_str() const;
    };

    struct Checker_List_File
    {
        std::string            file;
        long                   id;
    };

    struct Checker_List_Res
    {
        Checker_List_Res() : nok(NULL) {}
        ~Checker_List_Res();

        std::vector<Checker_List_File*>  files;
        MediaConch_Nok                  *nok;
        std::string                      to_str() const;
    };

    // Validate
    struct Checker_Validate_Req
    {
        Checker_Validate_Req() :           user(-1) {}

        int                                user;
        std::vector<long>                  ids;
        Report                             report;
        std::vector<size_t>                policies_ids;
        std::vector<std::string>           policies_contents;
        std::map<std::string, std::string> options;
        std::string                        to_str() const;
    };

    struct Checker_Validate_Ok
    {
        Checker_Validate_Ok() :    valid(false) {}

        long                       id;
        bool                       valid;
        std::string                to_str() const;
    };

    struct Checker_Validate_Res
    {
        Checker_Validate_Res() : nok(NULL) {}
        ~Checker_Validate_Res();

        std::vector<Checker_Validate_Ok*>  ok;
        MediaConch_Nok                    *nok;
        std::string                        to_str() const;
    };

    struct Checker_File_From_Id_Req
    {
        Checker_File_From_Id_Req() : user(-1), id(-1) {}

        int          user;
        long         id;
        std::string  to_str() const;
    };

    struct Checker_File_From_Id_Res
    {
        Checker_File_From_Id_Res() : nok(NULL) {}
        ~Checker_File_From_Id_Res();

        std::string     file;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Checker_Id_From_Filename_Req
    {
        Checker_Id_From_Filename_Req() : user(-1) {}

        int                                               user;
        std::string                                       filename;
        std::vector<std::pair<std::string,std::string> >  options;
        std::string                                       to_str() const;
    };

    struct Checker_Id_From_Filename_Res
    {
        Checker_Id_From_Filename_Res() : id(-1), nok(NULL) {}
        ~Checker_Id_From_Filename_Res();

        long            id;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Checker_File_Information_Req
    {
        Checker_File_Information_Req() : user(-1), id(-1) {}

        int          user;
        long         id;
        std::string  to_str() const;
    };

    struct Checker_File_Information_Res
    {
        Checker_File_Information_Res() : source_id(-1), generated_time((size_t)-1),
                                         analyzed(false), has_error(false), nok(NULL) {}
        ~Checker_File_Information_Res();

        std::string                                       filename;
        std::string                                       file_last_modification;
        std::string                                       generated_log;
        std::string                                       generated_error_log;
        std::vector<std::pair<std::string,std::string> >  options;
        std::string                                       error_log;
        std::vector<long>                                 generated_id;
        long                                              source_id;
        size_t                                            generated_time;
        bool                                              analyzed;
        bool                                              has_error;
        MediaConch_Nok                                   *nok;
        std::string                                       to_str() const;
    };

    struct Default_Values_For_Type_Req
    {
        std::string  type;
        std::string  field;
        std::string  to_str() const;
    };

    struct Default_Values_For_Type_Res
    {
        Default_Values_For_Type_Res() : nok(NULL) {}
        ~Default_Values_For_Type_Res();

        std::vector<std::string>  values;
        MediaConch_Nok           *nok;
        std::string               to_str() const;
    };

//***************************************************************************
// Policy
//***************************************************************************
    struct XSLT_Policy_Create_Req
    {
        XSLT_Policy_Create_Req() : user(-1), parent_id(-1) {}
        int         user;
        int         parent_id;
        std::string type;
        std::string to_str() const;
    };

    struct XSLT_Policy_Create_Res
    {
        XSLT_Policy_Create_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Create_Res();

        int             id;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Import_Req
    {
        Policy_Import_Req() : user(-1) {}

        int         user;
        std::string xml;
        std::string to_str() const;
    };

    struct Policy_Import_Res
    {
        Policy_Import_Res() : id(-1), nok(NULL) {}
        ~Policy_Import_Res();

        int             id;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Remove_Req
    {
        Policy_Remove_Req() : user(-1), id(-1) {}

        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Remove_Res
    {
        Policy_Remove_Res() : nok(NULL) {}
        ~Policy_Remove_Res();

        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Dump_Req
    {
        Policy_Dump_Req() : user(-1), id(-1), must_be_public(false) {}

        int         user;
        int         id;
        bool        must_be_public;
        std::string to_str() const;
    };

    struct Policy_Dump_Res
    {
        Policy_Dump_Res() : nok(NULL) {}
        ~Policy_Dump_Res();

        std::string     xml;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Save_Req
    {
        Policy_Save_Req() : user(-1), id(-1) {}

        int         user;
        int         id;
        std::string to_str() const;
    };

    struct Policy_Save_Res
    {
        Policy_Save_Res() : nok(NULL) {}
        ~Policy_Save_Res();

        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Duplicate_Req
    {
        Policy_Duplicate_Req() : user(-1), dst_user(NULL), id(-1), dst_policy_id(-1), must_be_public(false) {}
        ~Policy_Duplicate_Req();

        int         user;
        int        *dst_user;
        int         id;
        int         dst_policy_id;
        int         must_be_public;
        std::string to_str() const;
    };

    struct Policy_Duplicate_Res
    {
        Policy_Duplicate_Res() : id(-1), nok(NULL) {}
        ~Policy_Duplicate_Res();

        int             id;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Move_Req
    {
        Policy_Move_Req() : user(-1), id(-1), dst_policy_id(-1) {}

        int         user;
        int         id;
        int         dst_policy_id;
        std::string to_str() const;
    };

    struct Policy_Move_Res
    {
        Policy_Move_Res() : id(-1), nok(NULL) {}
        ~Policy_Move_Res();

        int             id;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Change_Info_Req
    {
        Policy_Change_Info_Req() : user(-1), id(-1) {}

        std::string  name;
        std::string  description;
        std::string  license;
        int          user;
        int          id;
        std::string  to_str() const;
    };

    struct Policy_Change_Info_Res
    {
        Policy_Change_Info_Res() : nok(NULL) {}
        ~Policy_Change_Info_Res();

        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Change_Type_Req
    {
        Policy_Change_Type_Req() : user(-1), id(-1) {}

        int          user;
        int          id;
        std::string  type;
        std::string  to_str() const;
    };

    struct Policy_Change_Type_Res
    {
        Policy_Change_Type_Res() : nok(NULL) {}
        ~Policy_Change_Type_Res();

        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Change_Is_Public_Req
    {
        Policy_Change_Is_Public_Req() : user(-1), id(-1), is_public(false) {}
        int          user;
        int          id;
        bool         is_public;
        std::string  to_str() const;
    };

    struct Policy_Change_Is_Public_Res
    {
        Policy_Change_Is_Public_Res() : nok(NULL) {}
        ~Policy_Change_Is_Public_Res();

        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Get_Req
    {
        Policy_Get_Req() : user(-1), id(-1), must_be_public(false), format("JSON") {}

        int         user;
        int         id;
        bool        must_be_public;
        std::string format;
        std::string to_str() const;
    };

    struct Policy_Get_Res
    {
        Policy_Get_Res() : policy(NULL), nok(NULL) {}
        ~Policy_Get_Res();

        MediaConchLib::Policy_Policy *policy;
        std::string                   policyTree;
        MediaConch_Nok               *nok;
        std::string                   to_str() const;
    };

    struct Policy_Get_Name_Req
    {
        Policy_Get_Name_Req() : user(-1), id(-1) {}

        int          user;
        int          id;
        std::string  to_str() const;
    };

    struct Policy_Get_Name_Res
    {
        Policy_Get_Name_Res() : nok(NULL) {}
        ~Policy_Get_Name_Res();

        std::string     name;
        MediaConch_Nok *nok;
        std::string     to_str() const;
    };

    struct Policy_Get_Policies_Count_Req
    {
        Policy_Get_Policies_Count_Req() : user(-1) {}

        int         user;
        std::string to_str() const;
    };

    struct Policy_Get_Policies_Count_Res
    {
        Policy_Get_Policies_Count_Res() : size(0), nok(NULL) {}
        ~Policy_Get_Policies_Count_Res();

        int              size;
        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct Policy_Clear_Policies_Req
    {
        Policy_Clear_Policies_Req() : user(-1) {}

        int         user;
        std::string to_str() const;
    };

    struct Policy_Clear_Policies_Res
    {
        Policy_Clear_Policies_Res() : nok(NULL) {}
        ~Policy_Clear_Policies_Res();

        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct Policy_Get_Policies_Req
    {
        Policy_Get_Policies_Req() : user(-1), format("JSON") {}

        int              user;
        std::vector<int> ids;
        std::string      format;
        std::string      to_str() const;
    };

    struct Policy_Get_Policies_Res
    {
        Policy_Get_Policies_Res() : nok(NULL) {}
        ~Policy_Get_Policies_Res();

        std::vector<MediaConchLib::Policy_Policy*>  policies;
        std::string                                 policiesTree;
        MediaConch_Nok                             *nok;
        std::string                                 to_str() const;
    };

    struct Policy_Public_Policy
    {
        Policy_Public_Policy() : id(-1), user(-1) {}

        long             id;
        long             user;
        std::string      name;
        std::string      description;
        std::string      license;
        std::string      to_str() const;
    };

    struct Policy_Get_Public_Policies_Req
    {
        std::string      to_str() const;
    };

    struct Policy_Get_Public_Policies_Res
    {
        Policy_Get_Public_Policies_Res() :  nok(NULL) {}
        ~Policy_Get_Public_Policies_Res();

        std::vector<Policy_Public_Policy*>  policies;
        MediaConch_Nok                     *nok;
        std::string                         to_str() const;
    };

    struct Policy_Get_Policies_Names_List_Req
    {
        Policy_Get_Policies_Names_List_Req() : user(-1) {}

        int              user;
        std::string      to_str() const;
    };

    struct Policy_Get_Policies_Names_List_Res
    {
        Policy_Get_Policies_Names_List_Res() : nok(NULL) {}
        ~Policy_Get_Policies_Names_List_Res();

        std::vector<std::pair<int, std::string> >   policies;
        MediaConch_Nok                             *nok;
        std::string                                 to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Req
    {
        XSLT_Policy_Create_From_File_Req() : user(-1), id(-1) {}

        int         user;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Create_From_File_Res
    {
        XSLT_Policy_Create_From_File_Res() : policy_id(-1), nok(NULL) {}
        ~XSLT_Policy_Create_From_File_Res();

        int               policy_id;
        MediaConch_Nok   *nok;
        std::string       to_str() const;
    };

    // XSLT Rule
    struct XSLT_Policy_Rule_Create_Req
    {
        XSLT_Policy_Rule_Create_Req() : user(-1), policy_id(-1) {}

        int         user;
        int         policy_id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Create_Res
    {
        XSLT_Policy_Rule_Create_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Rule_Create_Res();

        int              id;
        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct XSLT_Policy_Rule_Get_Req
    {
        XSLT_Policy_Rule_Get_Req() : user(-1), policy_id(-1), id(-1) {}

        int         user;
        int         policy_id;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Get_Res
    {
        XSLT_Policy_Rule_Get_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Get_Res();

        MediaConch_Nok                  *nok;
        MediaConchLib::XSLT_Policy_Rule  rule;
        std::string                      to_str() const;
    };

    struct XSLT_Policy_Rule_Edit_Req
    {
        XSLT_Policy_Rule_Edit_Req() : user(-1), policy_id(-1) {}

        int                             user;
        int                             policy_id;
        MediaConchLib::XSLT_Policy_Rule rule;
        std::string                     to_str() const;
    };

    struct XSLT_Policy_Rule_Edit_Res
    {
        XSLT_Policy_Rule_Edit_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Edit_Res();

        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct XSLT_Policy_Rule_Duplicate_Req
    {
        XSLT_Policy_Rule_Duplicate_Req() : user(-1), policy_id(-1), id(-1), dst_policy_id(-1) {}

        int         user;
        int         policy_id;
        int         id;
        int         dst_policy_id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Duplicate_Res
    {
        XSLT_Policy_Rule_Duplicate_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Rule_Duplicate_Res();

        int              id;
        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct XSLT_Policy_Rule_Move_Req
    {
        XSLT_Policy_Rule_Move_Req() : user(-1), policy_id(-1), id(-1), dst_policy_id(-1) {}

        int         user;
        int         policy_id;
        int         id;
        int         dst_policy_id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Move_Res
    {
        XSLT_Policy_Rule_Move_Res() : id(-1), nok(NULL) {}
        ~XSLT_Policy_Rule_Move_Res();

        int              id;
        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

    struct XSLT_Policy_Rule_Delete_Req
    {
        XSLT_Policy_Rule_Delete_Req() : user(-1), policy_id(-1), id(-1) {}

        int         user;
        int         policy_id;
        int         id;
        std::string to_str() const;
    };

    struct XSLT_Policy_Rule_Delete_Res
    {
        XSLT_Policy_Rule_Delete_Res() : nok(NULL) {}
        ~XSLT_Policy_Rule_Delete_Res();

        MediaConch_Nok  *nok;
        std::string      to_str() const;
    };

public:
    //Constructor/Destructor
    RESTAPI();
    virtual ~RESTAPI();

    // Serialize: Request
    int serialize_mediaconch_get_plugins_req(MediaConch_Get_Plugins_Req& req, std::string&, std::string& err);
    int serialize_mediaconch_watch_folder_req(MediaConch_Watch_Folder_Req& req, std::string&, std::string& err);
    int serialize_mediaconch_list_watch_folders_req(MediaConch_List_Watch_Folders_Req& req, std::string&, std::string& err);
    int serialize_mediaconch_edit_watch_folder_req(MediaConch_Edit_Watch_Folder_Req& req, std::string&, std::string& err);
    int serialize_mediaconch_remove_watch_folder_req(MediaConch_Remove_Watch_Folder_Req& req, std::string&, std::string& err);

    int serialize_checker_analyze_req(Checker_Analyze_Req& req, std::string&, std::string& err);
    int serialize_checker_status_req(Checker_Status_Req& req, std::string&, std::string& err);
    int serialize_checker_report_req(Checker_Report_Req& req, std::string&, std::string& err);
    int serialize_checker_clear_req(Checker_Clear_Req& req, std::string&, std::string& err);
    int serialize_checker_stop_req(Checker_Stop_Req& req, std::string&, std::string& err);
    int serialize_checker_list_req(Checker_List_Req& req, std::string&, std::string& err);
    int serialize_checker_validate_req(Checker_Validate_Req& req, std::string&, std::string& err);
    int serialize_checker_file_from_id_req(Checker_File_From_Id_Req& req, std::string&, std::string& err);
    int serialize_checker_id_from_filename_req(Checker_Id_From_Filename_Req& req, std::string&, std::string& err);
    int serialize_checker_file_information_req(Checker_File_Information_Req& req, std::string&, std::string& err);
    int serialize_default_values_for_type_req(Default_Values_For_Type_Req& req, std::string&, std::string& err);

    int serialize_xslt_policy_create_req(XSLT_Policy_Create_Req& req, std::string&, std::string& err);
    int serialize_policy_import_req(Policy_Import_Req& req, std::string&, std::string& err);
    int serialize_policy_remove_req(Policy_Remove_Req& req, std::string&, std::string& err);
    int serialize_policy_dump_req(Policy_Dump_Req& req, std::string&, std::string& err);
    int serialize_policy_save_req(Policy_Save_Req& req, std::string&, std::string& err);
    int serialize_policy_duplicate_req(Policy_Duplicate_Req& req, std::string&, std::string& err);
    int serialize_policy_move_req(Policy_Move_Req& req, std::string&, std::string& err);
    int serialize_policy_change_info_req(Policy_Change_Info_Req& req, std::string&, std::string& err);
    int serialize_policy_change_type_req(Policy_Change_Type_Req& req, std::string&, std::string& err);
    int serialize_policy_change_is_public_req(Policy_Change_Is_Public_Req& req, std::string&, std::string& err);
    int serialize_policy_get_req(Policy_Get_Req& req, std::string&, std::string& err);
    int serialize_policy_get_name_req(Policy_Get_Name_Req& req, std::string&, std::string& err);
    int serialize_policy_get_policies_count_req(Policy_Get_Policies_Count_Req& req, std::string&, std::string& err);
    int serialize_policy_clear_policies_req(Policy_Clear_Policies_Req& req, std::string&, std::string& err);
    int serialize_policy_get_policies_req(Policy_Get_Policies_Req& req, std::string&, std::string& err);
    int serialize_policy_get_public_policies_req(Policy_Get_Public_Policies_Req& req, std::string&, std::string& err);
    int serialize_policy_get_policies_names_list_req(Policy_Get_Policies_Names_List_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_create_from_file_req(XSLT_Policy_Create_From_File_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule_create_req(XSLT_Policy_Rule_Create_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule_get_req(XSLT_Policy_Rule_Get_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule(MediaConchLib::XSLT_Policy_Rule& rule, Container::Value&, std::string& err);
    int serialize_xslt_policy_rule_edit_req(XSLT_Policy_Rule_Edit_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule_duplicate_req(XSLT_Policy_Rule_Duplicate_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule_move_req(XSLT_Policy_Rule_Move_Req& req, std::string&, std::string& err);
    int serialize_xslt_policy_rule_delete_req(XSLT_Policy_Rule_Delete_Req& req, std::string&, std::string& err);

    // Serialize: Result
    int serialize_mediaconch_get_plugins_res(MediaConch_Get_Plugins_Res& res, std::string&, std::string& err);
    int serialize_mediaconch_watch_folder_res(MediaConch_Watch_Folder_Res& res, std::string&, std::string& err);
    int serialize_mediaconch_list_watch_folders_res(MediaConch_List_Watch_Folders_Res& res, std::string&, std::string& err);
    int serialize_mediaconch_edit_watch_folder_res(MediaConch_Edit_Watch_Folder_Res& res, std::string&, std::string& err);
    int serialize_mediaconch_remove_watch_folder_res(MediaConch_Remove_Watch_Folder_Res& res, std::string&, std::string& err);

    int serialize_checker_analyze_res(Checker_Analyze_Res& res, std::string&, std::string& err);
    int serialize_checker_status_res(Checker_Status_Res& res, std::string&, std::string& err);
    int serialize_checker_report_res(Checker_Report_Res& res, std::string&, std::string& err);
    int serialize_checker_clear_res(Checker_Clear_Res& res, std::string&, std::string& err);
    int serialize_checker_stop_res(Checker_Stop_Res& res, std::string&, std::string& err);
    int serialize_checker_list_res(Checker_List_Res& res, std::string&, std::string& err);
    int serialize_checker_validate_res(Checker_Validate_Res& res, std::string&, std::string& err);
    int serialize_checker_file_from_id_res(Checker_File_From_Id_Res& res, std::string&, std::string& err);
    int serialize_checker_id_from_filename_res(Checker_Id_From_Filename_Res& res, std::string&, std::string& err);
    int serialize_checker_file_information_res(Checker_File_Information_Res& res, std::string&, std::string& err);
    int serialize_default_values_for_type_res(Default_Values_For_Type_Res& res, std::string&, std::string& err);

    int serialize_xslt_policy_create_res(XSLT_Policy_Create_Res& res, std::string&, std::string& err);
    int serialize_policy_import_res(Policy_Import_Res& res, std::string&, std::string& err);
    int serialize_policy_remove_res(Policy_Remove_Res& res, std::string&, std::string& err);
    int serialize_policy_dump_res(Policy_Dump_Res& res, std::string&, std::string& err);
    int serialize_policy_save_res(Policy_Save_Res& res, std::string&, std::string& err);
    int serialize_policy_duplicate_res(Policy_Duplicate_Res& res, std::string&, std::string& err);
    int serialize_policy_move_res(Policy_Move_Res& res, std::string&, std::string& err);
    int serialize_policy_change_info_res(Policy_Change_Info_Res& res, std::string&, std::string& err);
    int serialize_policy_change_type_res(Policy_Change_Type_Res& res, std::string&, std::string& err);
    int serialize_policy_change_is_public_res(Policy_Change_Is_Public_Res& res, std::string&, std::string& err);
    int serialize_policy_get_res(Policy_Get_Res& res, std::string&, std::string& err);
    int serialize_policy_get_name_res(Policy_Get_Name_Res& res, std::string&, std::string& err);
    int serialize_policy_get_policies_count_res(Policy_Get_Policies_Count_Res& res, std::string&, std::string& err);
    int serialize_policy_clear_policies_res(Policy_Clear_Policies_Res& res, std::string&, std::string& err);
    int serialize_policy_get_policies_res(Policy_Get_Policies_Res& res, std::string&, std::string& err);
    int serialize_policy_get_public_policies_res(Policy_Get_Public_Policies_Res& res, std::string&, std::string& err);
    int serialize_policy_get_policies_names_list_res(Policy_Get_Policies_Names_List_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_create_from_file_res(XSLT_Policy_Create_From_File_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_create_res(XSLT_Policy_Rule_Create_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_get_res(XSLT_Policy_Rule_Get_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_edit_res(XSLT_Policy_Rule_Edit_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_duplicate_res(XSLT_Policy_Rule_Duplicate_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_move_res(XSLT_Policy_Rule_Move_Res& res, std::string&, std::string& err);
    int serialize_xslt_policy_rule_delete_res(XSLT_Policy_Rule_Delete_Res& res, std::string&, std::string& err);

    // Parse: Request
    MediaConch_Get_Plugins_Req          *parse_mediaconch_get_plugins_req(const std::string& data, std::string& err);
    MediaConch_Watch_Folder_Req         *parse_mediaconch_watch_folder_req(const std::string& data, std::string& err);
    MediaConch_List_Watch_Folders_Req   *parse_mediaconch_list_watch_folders_req(const std::string& data, std::string& err);
    MediaConch_Edit_Watch_Folder_Req    *parse_mediaconch_edit_watch_folder_req(const std::string& data, std::string& err);
    MediaConch_Remove_Watch_Folder_Req  *parse_mediaconch_remove_watch_folder_req(const std::string& data, std::string& err);

    Checker_Analyze_Req                 *parse_checker_analyze_req(const std::string& data, std::string& err);
    Checker_Status_Req                  *parse_checker_status_req(const std::string& data, std::string& err);
    Checker_Report_Req                  *parse_checker_report_req(const std::string& data, std::string& err);
    Checker_Clear_Req                   *parse_checker_clear_req(const std::string& data, std::string& err);
    Checker_Stop_Req                    *parse_checker_stop_req(const std::string& data, std::string& err);
    Checker_List_Req                    *parse_checker_list_req(const std::string& data, std::string& err);
    Checker_Validate_Req                *parse_checker_validate_req(const std::string& data, std::string& err);
    Checker_File_From_Id_Req            *parse_checker_file_from_id_req(const std::string& data, std::string& err);
    Checker_Id_From_Filename_Req        *parse_checker_id_from_filename_req(const std::string& data, std::string& err);
    Checker_File_Information_Req        *parse_checker_file_information_req(const std::string& data, std::string& err);
    Default_Values_For_Type_Req         *parse_default_values_for_type_req(const std::string& data, std::string& err);

    XSLT_Policy_Create_Req              *parse_xslt_policy_create_req(const std::string&, std::string& err);
    Policy_Import_Req                   *parse_policy_import_req(const std::string&, std::string& err);
    Policy_Remove_Req                   *parse_policy_remove_req(const std::string&, std::string& err);
    Policy_Dump_Req                     *parse_policy_dump_req(const std::string&, std::string& err);
    Policy_Save_Req                     *parse_policy_save_req(const std::string&, std::string& err);
    Policy_Duplicate_Req                *parse_policy_duplicate_req(const std::string&, std::string& err);
    Policy_Move_Req                     *parse_policy_move_req(const std::string&, std::string& err);
    Policy_Change_Info_Req              *parse_policy_change_info_req(const std::string&, std::string& err);
    Policy_Change_Type_Req              *parse_policy_change_type_req(const std::string&, std::string& err);
    Policy_Change_Is_Public_Req         *parse_policy_change_is_public_req(const std::string&, std::string& err);
    Policy_Get_Req                      *parse_policy_get_req(const std::string&, std::string& err);
    Policy_Get_Name_Req                 *parse_policy_get_name_req(const std::string&, std::string& err);
    Policy_Get_Policies_Count_Req       *parse_policy_get_policies_count_req(const std::string&, std::string& err);
    Policy_Clear_Policies_Req           *parse_policy_clear_policies_req(const std::string&, std::string& err);
    Policy_Get_Policies_Req             *parse_policy_get_policies_req(const std::string&, std::string& err);
    Policy_Get_Public_Policies_Req      *parse_policy_get_public_policies_req(const std::string&, std::string& err);
    Policy_Get_Policies_Names_List_Req  *parse_policy_get_policies_names_list_req(const std::string&, std::string& err);
    XSLT_Policy_Create_From_File_Req    *parse_xslt_policy_create_from_file_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Create_Req         *parse_xslt_policy_rule_create_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Get_Req            *parse_xslt_policy_rule_get_req(const std::string&, std::string& err);
    int                                  parse_xslt_policy_rule(Container::Value *val, MediaConchLib::XSLT_Policy_Rule *,
                                                                std::string& err);
    XSLT_Policy_Rule_Edit_Req           *parse_xslt_policy_rule_edit_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Duplicate_Req      *parse_xslt_policy_rule_duplicate_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Move_Req           *parse_xslt_policy_rule_move_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Delete_Req         *parse_xslt_policy_rule_delete_req(const std::string&, std::string& err);

    // Parse: URI Request
    MediaConch_Get_Plugins_Req          *parse_uri_mediaconch_get_plugins_req(const std::string& uri, std::string& err);
    MediaConch_Watch_Folder_Req         *parse_uri_mediaconch_watch_folder_req(const std::string& uri, std::string& err);
    MediaConch_List_Watch_Folders_Req   *parse_uri_mediaconch_list_watch_folders_req(const std::string& uri, std::string& err);
    MediaConch_Edit_Watch_Folder_Req    *parse_uri_mediaconch_edit_watch_folder_req(const std::string& uri, std::string& err);
    MediaConch_Remove_Watch_Folder_Req  *parse_uri_mediaconch_remove_watch_folder_req(const std::string& uri, std::string& err);

    Checker_Analyze_Req                 *parse_uri_checker_analyze_req(const std::string& uri, std::string& err);
    Checker_Status_Req                  *parse_uri_checker_status_req(const std::string& uri, std::string& err);
    Checker_Report_Req                  *parse_uri_checker_report_req(const std::string& uri, std::string& err);
    Checker_Clear_Req                   *parse_uri_checker_clear_req(const std::string& uri, std::string& err);
    Checker_Stop_Req                    *parse_uri_checker_stop_req(const std::string& uri, std::string& err);
    Checker_List_Req                    *parse_uri_checker_list_req(const std::string& uri, std::string& err);
    Checker_Validate_Req                *parse_uri_checker_validate_req(const std::string& uri, std::string& err);
    Checker_File_From_Id_Req            *parse_uri_checker_file_from_id_req(const std::string& uri, std::string& err);
    Checker_Id_From_Filename_Req        *parse_uri_checker_id_from_filename_req(const std::string& uri, std::string& err);
    Checker_File_Information_Req        *parse_uri_checker_file_information_req(const std::string& uri, std::string& err);
    Default_Values_For_Type_Req         *parse_uri_default_values_for_type_req(const std::string& uri, std::string& err);

    XSLT_Policy_Create_Req              *parse_uri_xslt_policy_create_req(const std::string&, std::string& err);
    Policy_Import_Req                   *parse_uri_policy_import_req(const std::string&, std::string& err);
    Policy_Remove_Req                   *parse_uri_policy_remove_req(const std::string&, std::string& err);
    Policy_Dump_Req                     *parse_uri_policy_dump_req(const std::string&, std::string& err);
    Policy_Save_Req                     *parse_uri_policy_save_req(const std::string&, std::string& err);
    Policy_Duplicate_Req                *parse_uri_policy_duplicate_req(const std::string&, std::string& err);
    Policy_Move_Req                     *parse_uri_policy_move_req(const std::string&, std::string& err);
    Policy_Change_Info_Req              *parse_uri_policy_change_info_req(const std::string&, std::string& err);
    Policy_Change_Type_Req              *parse_uri_policy_change_type_req(const std::string&, std::string& err);
    Policy_Change_Is_Public_Req         *parse_uri_policy_change_is_public_req(const std::string&, std::string& err);
    Policy_Get_Req                      *parse_uri_policy_get_req(const std::string&, std::string& err);
    Policy_Get_Name_Req                 *parse_uri_policy_get_name_req(const std::string&, std::string& err);
    Policy_Get_Policies_Count_Req       *parse_uri_policy_get_policies_count_req(const std::string&, std::string& err);
    Policy_Clear_Policies_Req           *parse_uri_policy_clear_policies_req(const std::string&, std::string& err);
    Policy_Get_Policies_Req             *parse_uri_policy_get_policies_req(const std::string&, std::string& err);
    Policy_Get_Public_Policies_Req      *parse_uri_policy_get_public_policies_req(const std::string&, std::string& err);
    Policy_Get_Policies_Names_List_Req  *parse_uri_policy_get_policies_names_list_req(const std::string&, std::string& err);
    XSLT_Policy_Create_From_File_Req    *parse_uri_xslt_policy_create_from_file_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Create_Req         *parse_uri_xslt_policy_rule_create_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Get_Req            *parse_uri_xslt_policy_rule_get_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Edit_Req           *parse_uri_xslt_policy_rule_edit_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Duplicate_Req      *parse_uri_xslt_policy_rule_duplicate_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Move_Req           *parse_uri_xslt_policy_rule_move_req(const std::string&, std::string& err);
    XSLT_Policy_Rule_Delete_Req         *parse_uri_xslt_policy_rule_delete_req(const std::string&, std::string& err);

    // Parse: Result
    MediaConch_Get_Plugins_Res         *parse_mediaconch_get_plugins_res(const std::string& data, std::string& err);
    MediaConch_Watch_Folder_Res        *parse_mediaconch_watch_folder_res(const std::string& data, std::string& err);
    MediaConch_List_Watch_Folders_Res  *parse_mediaconch_list_watch_folders_res(const std::string& data, std::string& err);
    MediaConch_Edit_Watch_Folder_Res   *parse_mediaconch_edit_watch_folder_res(const std::string& data, std::string& err);
    MediaConch_Remove_Watch_Folder_Res *parse_mediaconch_remove_watch_folder_res(const std::string& data, std::string& err);

    Checker_Analyze_Res                *parse_checker_analyze_res(const std::string& data, std::string& err);
    Checker_Status_Res                 *parse_checker_status_res(const std::string& data, std::string& err);
    Checker_Report_Res                 *parse_checker_report_res(const std::string& data, std::string& err);
    Checker_Clear_Res                  *parse_checker_clear_res(const std::string& data, std::string& err);
    Checker_Stop_Res                   *parse_checker_stop_res(const std::string& data, std::string& err);
    Checker_List_Res                   *parse_checker_list_res(const std::string& data, std::string& err);
    Checker_Validate_Res               *parse_checker_validate_res(const std::string& data, std::string& err);
    Checker_File_From_Id_Res           *parse_checker_file_from_id_res(const std::string& data, std::string& err);
    Checker_Id_From_Filename_Res       *parse_checker_id_from_filename_res(const std::string& data, std::string& err);
    Checker_File_Information_Res       *parse_checker_file_information_res(const std::string& data, std::string& err);
    Default_Values_For_Type_Res        *parse_default_values_for_type_res(const std::string& data, std::string& err);

    XSLT_Policy_Create_Res             *parse_xslt_policy_create_res(const std::string&, std::string& err);
    Policy_Import_Res                  *parse_policy_import_res(const std::string&, std::string& err);
    Policy_Remove_Res                  *parse_policy_remove_res(const std::string&, std::string& err);
    Policy_Dump_Res                    *parse_policy_dump_res(const std::string&, std::string& err);
    Policy_Save_Res                    *parse_policy_save_res(const std::string&, std::string& err);
    Policy_Duplicate_Res               *parse_policy_duplicate_res(const std::string&, std::string& err);
    Policy_Move_Res                    *parse_policy_move_res(const std::string&, std::string& err);
    Policy_Change_Info_Res             *parse_policy_change_info_res(const std::string&, std::string& err);
    Policy_Change_Type_Res             *parse_policy_change_type_res(const std::string&, std::string& err);
    Policy_Change_Is_Public_Res        *parse_policy_change_is_public_res(const std::string&, std::string& err);
    Policy_Get_Res                     *parse_policy_get_res(const std::string&, std::string& err);
    Policy_Get_Name_Res                *parse_policy_get_name_res(const std::string&, std::string& err);
    Policy_Get_Policies_Count_Res      *parse_policy_get_policies_count_res(const std::string&, std::string& err);
    Policy_Clear_Policies_Res          *parse_policy_clear_policies_res(const std::string&, std::string& err);
    Policy_Get_Policies_Res            *parse_policy_get_policies_res(const std::string&, std::string& err);
    Policy_Get_Public_Policies_Res     *parse_policy_get_public_policies_res(const std::string&, std::string& err);
    Policy_Get_Policies_Names_List_Res *parse_policy_get_policies_names_list_res(const std::string&, std::string& err);
    XSLT_Policy_Create_From_File_Res   *parse_xslt_policy_create_from_file_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Create_Res        *parse_xslt_policy_rule_create_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Get_Res           *parse_xslt_policy_rule_get_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Edit_Res          *parse_xslt_policy_rule_edit_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Duplicate_Res     *parse_xslt_policy_rule_duplicate_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Move_Res          *parse_xslt_policy_rule_move_res(const std::string&, std::string& err);
    XSLT_Policy_Rule_Delete_Res        *parse_xslt_policy_rule_delete_res(const std::string&, std::string& err);

    std::string                         get_error() const { return error; }

private:
    Container *model;

    std::string error;

    //Helper
    Container::Value serialize_mediaconch_nok(MediaConch_Nok* nok, std::string& err);
    Container::Value serialize_checker_analyze_args(std::vector<Checker_Analyze_Arg>& args, std::string& err);
    Container::Value serialize_ids(std::vector<long>& ids, std::string& err);
    Container::Value serialize_checker_report_reports(std::vector<Report>& args, std::string& err);
    Container::Value serialize_checker_report_string(const std::string& args, std::string& err);
    Container::Value serialize_checker_report_int(int val, std::string& err);
    Container::Value serialize_checker_report_arr_str(const std::vector<std::string>& reports, std::string& err);
    Container::Value serialize_checker_report_arr_long_u(const std::vector<size_t>& reports, std::string& err);
    Container::Value serialize_checker_analyze_oks(std::vector<Checker_Analyze_Ok*>& array, std::string& err);
    Container::Value serialize_checker_status_oks(std::vector<Checker_Status_Ok*>& array, std::string& err);
    int serialize_checker_report_ok(Checker_Report_Ok* ok, Container::Value&, std::string& err);
    Container::Value serialize_checker_list_file(const std::string& file, int id, std::string& err);
    Container::Value serialize_checker_validate_ok(Checker_Validate_Ok* ok, std::string& err);
    void serialize_policies_get_policies(const std::vector<MediaConchLib::Policy_Policy*>&, Container::Value& policies, std::string& err);
    void serialize_policy_public_policy(const std::vector<Policy_Public_Policy*>&, Container::Value& policies, std::string& err);
    void serialize_policies_get_policies_names(const std::vector<std::pair<int, std::string> >& policies, Container::Value &p, std::string& err);
    void serialize_a_policy(MediaConchLib::Policy_Policy* policy, Container::Value &ok_v, std::string& err);
    void serialize_a_xslt_policy_rule(MediaConchLib::XSLT_Policy_Rule* rule, Container::Value &ok_v, std::string& err);

    int parse_mediaconch_nok(Container::Value *v, MediaConch_Nok** n, std::string& err);
    int parse_checker_analyze_arg(Container::Value *v, std::vector<Checker_Analyze_Arg>& args, std::string& err);
    int parse_checker_report_reports(Container::Value *v, std::vector<Report>& reports, std::string& err);
    int parse_checker_analyze_ok(Container::Value *v, std::vector<Checker_Analyze_Ok*>& ok, std::string& err);
    int parse_checker_status_ok(Container::Value *v, std::vector<Checker_Status_Ok*>& ok, std::string& err);
    int parse_checker_report_ok(Container::Value *v, Checker_Report_Ok** ok, std::string& err);
    int parse_checker_list_file(Container::Value *v, std::string& file, long& id, std::string& err);
    int parse_checker_validate_ok(Container::Value *v, std::vector<Checker_Validate_Ok*>& oks, std::string& err);
    int parse_policies_get_policies(Container::Value* policies, std::vector<MediaConchLib::Policy_Policy*>&, std::string& err);
    int parse_policy_public_policy(Container::Value* policies, std::vector<Policy_Public_Policy*>&, std::string& err);
    int parse_policies_get_policies_names(Container::Value* policies, std::vector<std::pair<int, std::string> >&, std::string& err);
    MediaConchLib::Policy_Policy* parse_a_policy(Container::Value* policy, std::string& err);
    MediaConchLib::XSLT_Policy_Rule* parse_a_xslt_policy_rule(Container::Value* rule, std::string& err);

    RESTAPI (const RESTAPI&);
    RESTAPI& operator=(const RESTAPI&);
};

}

#endif
