/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBCHOOSE_H
#define WEBCHOOSE_H

#include <QtGlobal>

#if !defined(WEB_MACHINE_ENGINE) && !defined(WEB_MACHINE_KIT)
#if (QT_VERSION >= 0x050600)
#define WEB_MACHINE_ENGINE
#else
#define WEB_MACHINE_KIT
#endif
#endif

#endif // WEBCHOOSE_H
