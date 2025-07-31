/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Wrapper for MediaConch Library
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef MediaConchDLLH
#define MediaConchDLLH

#ifndef __cplusplus
#include <stdbool.h>
#endif /* __cplusplus */

/*
typedef enum MediaConch_compression
{
    MediaConch_compression_None = 0,
    MediaConch_compression_ZLib,
    MediaConch_compression_Max,
} MediaConch_compression;
*/

typedef enum MediaConch_format_t
{
    MediaConch_format_Text = 0,
    MediaConch_format_Xml,         // XML corresponding to only one of MediaConch, MediaInfo, MediaTrace
    MediaConch_format_MaXml,       // MAXML, can contain one or more of MediaConch, MediaInfo, MediaTrace
    MediaConch_format_JsTree,
    MediaConch_format_Html,
    MediaConch_format_OrigXml,
    MediaConch_format_Simple,
    MediaConch_format_CSV,
    MediaConch_format_Json,
    MediaConch_format_Max
} MediaConch_format_c;

typedef enum MediaConch_report_t
{
    report_MediaConch = 0,
    report_MediaInfo,
    report_MediaTrace,
    report_MicroMediaTrace = 5,
    report_Max,
} MediaConch_report_c;

#if defined(_MSC_VER)
#define MEDIACONCH_EXPORT __declspec(dllexport)
#else
#define MEDIACONCH_EXPORT __attribute__ ((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

MEDIACONCH_EXPORT void*       MediaConch_new                          ();
MEDIACONCH_EXPORT void        MediaConch_delete                       (void* handle);
MEDIACONCH_EXPORT const char* MediaConch_get_last_error               (void* handle);
MEDIACONCH_EXPORT long        MediaConch_add_file                     (void* handle, const char* filename);
MEDIACONCH_EXPORT void        MediaConch_add_report                   (void* handle, MediaConch_report_c report);
MEDIACONCH_EXPORT void        MediaConch_add_policy                   (void* handle, const char* filename);
MEDIACONCH_EXPORT void        MediaConch_set_format                   (void* handle, MediaConch_format_c format);
MEDIACONCH_EXPORT void        MediaConch_set_display                  (void* handle, const char* filename);
MEDIACONCH_EXPORT void        MediaConch_set_mil_option               (void* handle, const char* name, const char* value);
MEDIACONCH_EXPORT void        MediaConch_set_force_analyze            (void* handle, bool force);
MEDIACONCH_EXPORT void        MediaConch_set_policy_verbosity         (void* handle, int verbosity);
MEDIACONCH_EXPORT void        MediaConch_set_implementation_verbosity (void* handle, int verbosity);
MEDIACONCH_EXPORT const char* MediaConch_get_report                   (void* handle, long file_id);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

// C++ wrapper for C functions

#ifdef __cplusplus

//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------

namespace MediaConch
{
    class MediaConch
    {
      public :
        MediaConch  () { handle = MediaConch_new(); };
        ~MediaConch () { MediaConch_delete(handle); };

        //API
        const std::string get_last_error               ()                                                  { return std::string(MediaConch_get_last_error(handle)); };
        long              add_file                     (const std::string& filename)                       { return MediaConch_add_file(handle, filename.c_str()); };
        void              add_report                   (MediaConch_report_t report)                        { MediaConch_add_report(handle, (MediaConch_report_c)report); };
        void              add_policy                   (const std::string& filename)                       { MediaConch_add_policy(handle, filename.c_str()); };
        void              set_format                   (MediaConch_format_t format)                        { MediaConch_set_format(handle, (MediaConch_format_c)format); };
        void              set_display                  (const std::string& filename)                       { MediaConch_set_display(handle, filename.c_str()); };
        void              set_mil_option               (const std::string& name, const std::string& value) { MediaConch_set_mil_option(handle, name.c_str(), value.c_str()); };
        void              set_force_analyze            (bool force)                                        { MediaConch_set_force_analyze(handle, force); };
        void              set_policy_verbosity         (int verbosity)                                     { MediaConch_set_policy_verbosity(handle, verbosity); };
        void              set_implementation_verbosity (int verbosity)                                     { MediaConch_set_implementation_verbosity(handle, verbosity); };
        const std::string get_report                   (long file_id)                                      { return std::string(MediaConch_get_report(handle, file_id)); };
      private :
        void* handle;
    };
}
#endif /*__cplusplus*/

#endif
