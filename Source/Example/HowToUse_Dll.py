##  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 #
 #  Use of this source code is governed by a BSD-style license that can
 #  be found in the License.html file in the root of the source tree.
 ##

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#
# Python example
#
# To make this example working, you must put HowToUse_Dll.py, Example.xml
# and Example.ogg and in the same folder.
#
# MediaConch python module requires Python 3.0 or newer.
#
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

from mediaconch import *

MC = MediaConch()

# Add policy
MC.add_policy("Example.xml")

# Add file
file_id = MC.add_file("Example.ogg")

# Set display format (optionnal, default to xml)
MC.set_format(Format.text)

# Display report
print(MC.get_report(file_id))
