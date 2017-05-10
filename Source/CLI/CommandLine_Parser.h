/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

//---------------------------------------------------------------------------
#ifndef CommandLine_ParserH
#define CommandLine_ParserH
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#include "CLI.h"
#include "Config.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

int Parse(MediaConch::CLI* cli, std::string& argument);


//---------------------------------------------------------------------------
#define CL_METHOD(_NAME) \
    int _NAME(MediaConch::CLI* cli, const std::string& argument)

#define CL_OPTION(_NAME) \
    int _NAME(MediaConch::CLI* cli, const std::string& argument)

CL_OPTION(Help);
CL_OPTION(Inform);
CL_OPTION(User);
CL_OPTION(List);
CL_OPTION(Report);
CL_OPTION(Language);
CL_OPTION(Format);
CL_OPTION(LogFile);
CL_OPTION(Bom);
CL_OPTION(Version);
CL_OPTION(PolicyOption);
CL_OPTION(Display);
CL_OPTION(LogFile);
CL_OPTION(Configuration);
CL_OPTION(ImplementationSchema);
CL_OPTION(ImplementationVerbosity);
CL_OPTION(PolicyReferenceFile);
CL_OPTION(Compression);
CL_OPTION(Force);
CL_OPTION(NoMilAnalyze);
CL_OPTION(Asynchronous);
CL_OPTION(UsePlugin);
CL_OPTION(PluginsList);
CL_OPTION(PluginsConfiguration);
CL_OPTION(DefaultValuesForType);
CL_OPTION(CreatePolicy);
CL_OPTION(FileInformation);
CL_OPTION(WatchFoldersList);
CL_OPTION(WatchFolder);
CL_OPTION(WatchFolderReports);
CL_OPTION(WatchFolderNotRecursive);
CL_OPTION(WatchFolderUser);
CL_OPTION(Default);

//***************************************************************************
// Options which need actions
//***************************************************************************

void LogFile_Action(ZenLib::Ztring Inform);
void CallBack_Set(MediaConch::CLI* cli, void* Event_CallBackFunction);

#endif
