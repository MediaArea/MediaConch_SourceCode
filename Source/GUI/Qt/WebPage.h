/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef WEBPAGE_H
#define WEBPAGE_H

#include "WebChoose.h"

#if defined(WEB_MACHINE_ENGINE)
#include "WebEnginePage.h"
#endif
#if defined(WEB_MACHINE_KIT)
#include "WebKitPage.h"
#endif

#endif // WEBPAGE_H
