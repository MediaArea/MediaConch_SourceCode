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
#include "Help.h"
#include "Config.h"
#include "Daemon.h"
//---------------------------------------------------------------------------

//***************************************************************************
//
//***************************************************************************

//---------------------------------------------------------------------------
int Help()
{
    Version();
    Help_Usage();

    TEXTOUT("");
    TEXTOUT("Options:");
    TEXTOUT("Help:");
    TEXTOUT("  --Help, -h");
    TEXTOUT("      Display this help and exit");
    TEXTOUT("  --Version, -v");
    TEXTOUT("      Display the version and exit");
    TEXTOUT("");

    TEXTOUT("Configuration Element:");
    TEXTOUT("  --Configuration=ConfigurationFile, -c ConfigurationFile");
    TEXTOUT("      Use ConfigurationFile as configuration file");
    TEXTOUT("      Default, it uses the one in the user data application file");
    TEXTOUT("  --PluginsConfiguration=PluginsConfigurationFile, -pc PluginsConfigurationFile");
    TEXTOUT("      Use PluginsConfigurationFile as plugins configuration file");
    TEXTOUT("      Default, it uses the struct in the configuration file or");
    TEXTOUT("      no plugins configured if not inside.");
    TEXTOUT("");

    TEXTOUT("Implementation Checker:");
    TEXTOUT("  --ImplementationSchema=File, -i File");
    TEXTOUT("      Use the specified File for implementation validation");
    TEXTOUT("  --ImplementationVerbosity=V, -iv V");
    TEXTOUT("      Select verbosity (V) of the implementation check, default 5");
    TEXTOUT("      <= 4, show only fails and N/A");
    TEXTOUT("      >= 5, show fails, N/A and pass");
    TEXTOUT("");

    TEXTOUT("Daemon fork:");
    TEXTOUT("  --Fork=[Yes/No]");
    TEXTOUT("      If No, MediaConchd will not fork, default it forks");
    TEXTOUT("  -n");
    TEXTOUT("      Same as --Fork=No");
    TEXTOUT("");

    TEXTOUT("Daemon Debug:");
    TEXTOUT("  --OutputLog=File, -o File");
    TEXTOUT("      Log debug information to File");
    TEXTOUT("");

    TEXTOUT("Watch folder:");
    TEXTOUT("  --WatchFolder=folder, -wf folder");
    TEXTOUT("      Send to the server a folder to watch");
    TEXTOUT("  --WatchFolder-Reports=folder, -wfr folder");
    TEXTOUT("      Create reports of the selected watch folder to this folder");
    TEXTOUT("  --WatchFolder-Not-Recursive");
    TEXTOUT("      If watch folder is enabled, do not check recursively the folder");
    TEXTOUT("  --Plugin=PluginID");
    TEXTOUT("      By default, only format plugin are used.");
    TEXTOUT("      With this command, you can give the plugin ID you want to use.");
    TEXTOUT("      Plugin ID can be get using the --pluginslist.");
    TEXTOUT("  --PluginsList");
    TEXTOUT("      Output the plugins information ID");
    TEXTOUT("  --Policy=PolicyFile, -p PolicyFile");
    TEXTOUT("      Policy used by the watch folder manager.");
    TEXTOUT("");

    return DAEMON_RETURN_FINISH;
}

//---------------------------------------------------------------------------
int Help_Usage()
{
    TEXTOUT("Usage: \"MediaConchD [-Options...]\"");

    return DAEMON_RETURN_ERROR;
}

//---------------------------------------------------------------------------
int Version()
{
    TEXTOUT(std::string("MediaConch Daemon " + MediaConch::Daemon::version).c_str());

    return DAEMON_RETURN_FINISH;
}
