# -*- Mode:Python; indent-tabs-mode:nil; tab-width:4 -*-

"""This plugin extends the qmake plugin to run from subdirectory in source tree.
In addition to the qmake and defaults keywords, this plugin uses the following
plugin-specific keyword:
    - make-directory:
      (string)
      Run qmake in the given path.
"""

import os.path

from snapcraft.plugins import qmake

class QmakeSubdirPlugin(qmake.QmakePlugin):
    @classmethod
    def schema(cls):
        schema = super().schema()
        schema['properties']['make-directory'] = {
            'type': 'string',
            'default':  ''
        }

        return schema

    def __init__(self, name, options, project):
        super().__init__(name, options, project)
        if self.options.make_directory:
            self.builddir = os.path.join(self.build_basedir, self.options.make_directory)

    def enable_cross_compilation(self):
        return

