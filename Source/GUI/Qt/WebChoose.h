/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBCHOOSE_H
#define WEBCHOOSE_H

#include <QtGlobal>

#if defined(WEB_MACHINE_ENGINE) && defined(WEB_MACHINE_KIT)
#pragma message ("WEB_MACHINE_ENGINE and WEB_MACHINE_KIT defined, you must choose one of them")
#undef WEB_MACHINE_ENGINE
#undef WEB_MACHINE_KIT
#endif

#if !defined(WEB_MACHINE_ENGINE) && !defined(WEB_MACHINE_KIT)
#if (QT_VERSION >= 0x050600)
#define WEB_MACHINE_ENGINE
#else
#define WEB_MACHINE_KIT
#endif
#endif

#endif // WEBCHOOSE_H
