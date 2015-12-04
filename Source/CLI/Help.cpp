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
    TEXTOUT("--Help, -h");
    TEXTOUT("                    Display this help and exit");
    TEXTOUT("--Help=Advanced, -ha");
    TEXTOUT("                    Display the advanced help and exit");
    TEXTOUT("--Version, -v");
    TEXTOUT("                    Display the version and exit");
    TEXTOUT("");

    TEXTOUT("Reporting Elements:");
    TEXTOUT("--Mediaconch, -mc");
    TEXTOUT("                    Output MediaConch report (default)");
    TEXTOUT("                    (MediaConch contains default verbosity of implementationChecks plus any provided policy checks.)");
    TEXTOUT("--Mediainfo, -mi");
    TEXTOUT("                    Output MediaInfo report");
    TEXTOUT("--Mediatrace, -mt");
    TEXTOUT("                    Output a trace of the file");
    TEXTOUT("");

    // TEXTOUT("Implementation Checker:");
    // TEXTOUT("--Implementation-verbosity, -iv");
    // TEXTOUT("                    Select verbosity of the implementation check (not yet implemented)");
    // TEXTOUT("");

    TEXTOUT("Policy Checker:");
    TEXTOUT("--Policy=PolicyFileName, -p PolicyFileName");
    TEXTOUT("                    Apply the policy (XSL or Schematron) ");
    TEXTOUT("");

    TEXTOUT("Output Formats:");
    TEXTOUT("--Format=text -ft");
    TEXTOUT("                    Output in Text format (default)");
    TEXTOUT("--Format=xml -fx");
    TEXTOUT("                    Output in MediaInfo/MediaConch/MediaTrace XML format");
    TEXTOUT("                    (changed to -fa if there are more than 1 tool or more than 1 file)");
    TEXTOUT("--Format=maxml -fa");
    TEXTOUT("                    Output in MediaArea XML format");
    TEXTOUT("--Format=html -fh");
    TEXTOUT("                    Output MediaConch report in HTML format");
    TEXTOUT("--Display=DisplayFileName, -d DisplayFileName");
    TEXTOUT("                    Apply the display transformation (XSL)");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Usage()
{
    TEXTOUT("Usage: \"MediaConch [-Options...] FileName1 [Filename2...]\"");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Nothing()
{
    Help_Usage();
    TEXTOUT("\"MediaConch --Help\" for displaying more information");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Advanced()
{
    TEXTOUT("--LogFile=...");
    TEXTOUT("                    Save the output in the specified file");
    TEXTOUT("--Help=Ssl");
    TEXTOUT("                    More details about SSL specific options (e.g. for HTTPS or FTPS)");
    TEXTOUT("--Help=Ssh");
    TEXTOUT("                    More details about SSH specific options (e.g. for SFTP)");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Ssl()
{
    TEXTOUT("--Ssl_CertificateFileName=...");
    TEXTOUT("                    File name of the SSL certificate.");
    TEXTOUT("                    The default format is \"PEM\" and can be changed");
    TEXTOUT("                    with --Ssl_CertificateFormat.");
    TEXTOUT("--Ssl_CertificateFormat=...");
    TEXTOUT("                    File format of the SSL certificate.");
    TEXTOUT("                    Supported formats are \"PEM\" and \"DER\"");
    TEXTOUT("--Ssl_PrivateKeyFileName=...");
    TEXTOUT("                    File name of the SSL private key.");
    TEXTOUT("                    The default format is \"PEM\" and can be changed");
    TEXTOUT("                    with --Ssl_PrivateKeyFormat.");
    TEXTOUT("                    Note: private key with a password is not supported.");
    TEXTOUT("--Ssl_PrivateKeyFormat=...");
    TEXTOUT("                    File format of the SSL private key.");
    TEXTOUT("                    Supported formats are \"PEM\" and \"DER\"");
    TEXTOUT("--Ssl_CertificateAuthorityFileName=...");
    TEXTOUT("                    File name of the SSL certificate authorities");
    TEXTOUT("                    to verify the peer with.");
    TEXTOUT("--Ssl_CertificateAuthorityPath=...");
    TEXTOUT("                    Path of the SSL certificate authorities");
    TEXTOUT("                    to verify the peer with.");
    TEXTOUT("--Ssl_CertificateRevocationListFileName=...");
    TEXTOUT("                    File name of the SSL certificate revocation list.");
    TEXTOUT("                    The format is \"PEM\"");
    TEXTOUT("--Ssl_IgnoreSecurity=...");
    TEXTOUT("                    Does not verify the authenticity of the peer's certificate");
    TEXTOUT("                    Use it at your own risks");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Ssh()
{
    TEXTOUT("--Ssh_PublicKeyFileName=...");
    TEXTOUT("                    File name of the SSH private key.");
    TEXTOUT("                    Default is $HOME/.ssh/id_rsa.pub or $HOME/.ssh/id_dsa.pub");
    TEXTOUT("                    if the HOME environment variable is set, and just");
    TEXTOUT("                    \"id_rsa.pub\" or \"id_dsa.pub\" in the current directory");
    TEXTOUT("                    if HOME is not set.");
    TEXTOUT("                    Note: you need to set both public and private key.");
    TEXTOUT("--Ssh_PrivateKeyFileName=...");
    TEXTOUT("                    File name of the SSH private key.");
    TEXTOUT("                    Default is $HOME/.ssh/id_rsa or $HOME/.ssh/id_dsa");
    TEXTOUT("                    if the HOME environment variable is set, and just");
    TEXTOUT("                    \"id_rsa\" or \"id_dsa\" in the current directory");
    TEXTOUT("                    if HOME is not set.");
    TEXTOUT("                    Note: you need to set both public and private key.");
    TEXTOUT("                    Note: private key with a password is not supported.");
    TEXTOUT("--Ssh_KnownHostsFileName=...");
    TEXTOUT("                    File name of the known hosts");
    TEXTOUT("                    The format is the OpenSSH file format (libssh2)");
    TEXTOUT("                    Default is $HOME/.ssh/known_hosts");
    TEXTOUT("                    if the HOME environment variable is set, and just");
    TEXTOUT("                    \"known_hosts\" in the current directory");
    TEXTOUT("                    if HOME is not set.");
    TEXTOUT("--Ssh_IgnoreSecurity");
    TEXTOUT("                    Does not verify the authenticity of the peer");
    TEXTOUT("                    (you don't need to accept the key with ssh first)");
    TEXTOUT("                    Use it at your own risks");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Policy()
{
    TEXTOUT("--Policy=...  Specify a schema to validate");
    TEXTOUT("Usage: \"MediaConch --Policy=FileName\"");
    TEXTOUT("");
    TEXTOUT("FileName is the Schematron file used to validate");
    TEXTOUT("");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Xslt()
{
    TEXTOUT("--Xslt=...  Specify a schema to validate");
    TEXTOUT("Usage: \"MediaConch --Xslt=FileName\"");
    TEXTOUT("");
    TEXTOUT("FileName is the XSLT file used to validate");
    TEXTOUT("");

    return -1;
}

//---------------------------------------------------------------------------
int Version()
{
    TEXTOUT("MediaConch Command Line Interface 15.11");

    return -1;
}
