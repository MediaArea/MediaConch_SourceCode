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
    TEXTOUT("Usage: \"MediaConch [-Options...] FileName1 [Filename2...]\"");
    TEXTOUT("");
    TEXTOUT("Options:");
    TEXTOUT("--Help, -h");
    TEXTOUT("                    Display this help and exit");
    TEXTOUT("--Tool=Info, -ti");
    TEXTOUT("                    Output MediaInfo report");
    TEXTOUT("--Tool=Trace, -tt");
    TEXTOUT("                    Output a trace of the file");
    TEXTOUT("--Format=XML -fx");
    TEXTOUT("                    Output in XML format (only MediaInfo report for the moment)");
    TEXTOUT("--Format=JSTREE -fj");
    TEXTOUT("                    Output in JS-TREE JSON format (only trace report for the moment)");
    TEXTOUT("--Policy=SchematronFileName, -p SchematronFileName");
    TEXTOUT("                    Validate the XML report");
    TEXTOUT("--Xslt=XsltFileName, -x XsltFileName");
    TEXTOUT("                    Validate the XML report");
    TEXTOUT("--MAXML_Fields=Type");
    TEXTOUT("                    Give the possibe fields for the given type");
    TEXTOUT("--MAXML_StreamKinds");
    TEXTOUT("                    Give the stream possible kinds");
    TEXTOUT("--LogFile=...");
    TEXTOUT("                    Save the output in the specified file");
    TEXTOUT("--BOM");
    TEXTOUT("                    Byte order mark for UTF-8 output");
    TEXTOUT("");
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
int Help_Nothing()
{
    TEXTOUT("Usage: \"MediaConch [-Options...] FileName1 [Filename2...]\"");
    TEXTOUT("\"MediaConch --Help\" for displaying more information");

    return -1;
}

//---------------------------------------------------------------------------
int Help_Output()
{
    TEXTOUT("--Output=...  Specify a template (BETA)");
    TEXTOUT("Usage: \"MediaConch --Output=[xxx;]Text FileName\"");
    TEXTOUT("");
    TEXTOUT("xxx can be: General, Video, Audio, Text, Chapter, Image, Menu");
    TEXTOUT("Text can be the template text, or a filename");
    TEXTOUT("     Filename must be in the form file://filename");
    TEXTOUT("");
    TEXTOUT("See --Info-Parameters for available parameters in the text");
    TEXTOUT("(Parameters must be surrounded by \"%\" sign)");
    TEXTOUT("");
    TEXTOUT("Example: \"MediaInfo --Output=Video;%AspectRatio% FileName\"");
    TEXTOUT("");
    TEXTOUT("Example: \"MediaInfo --Output=Video;file://Video.txt FileName\"");
    TEXTOUT("and Video.txt contains ");
    TEXTOUT("\"%DisplayAspectRatio%\"        for Video Aspect Ratio.");
    TEXTOUT("");
    TEXTOUT("Example: \"MediaInfo --Output=file://Text.txt FileName\"");
    TEXTOUT("and Text.txt contains");
    TEXTOUT("\"Video;%DisplayAspectRatio%\"  for Video Aspect Ratio.");
    TEXTOUT("\"Audio;%Format%\"              for Audio Format.");

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
int Help_MAXML_Fields()
{
    TEXTOUT("--MAXML_Fields=...  Specify a type to get the possible fields");
    TEXTOUT("Usage: \"MediaConch --MAXML_Fields=Type\"");
    TEXTOUT("");
    TEXTOUT("Type can be: Audio, General, Image, Menu, Other, Text, Video");
    TEXTOUT("");

    return -1;
}

//---------------------------------------------------------------------------
int Version()
{
    TEXTOUT("MediaConch Command Line Interface 15.09");

    return -1;
}
