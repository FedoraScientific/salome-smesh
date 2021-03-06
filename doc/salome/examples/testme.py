# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

#!/usr/bin/env python

import unittest, sys, os

class SalomeSession(object):
    def __init__(self, script):
        import runSalome
        sys.argv  = ["runSalome.py"]
        sys.argv += ["--terminal"]
        sys.argv += ["--modules=GEOM,MED,SMESH"]
        sys.argv += ["%s" % script]
        clt, d = runSalome.main()
        return

    def __del__(self):
        port = os.getenv('NSPORT')
        import killSalomeWithPort
        killSalomeWithPort.killMyPort(port)
        return
    pass

class MyTest(unittest.TestCase):
    def testFunction(self):
        SalomeSession(sys.argv[1])
    pass

unittest.main(argv=sys.argv[:1])
