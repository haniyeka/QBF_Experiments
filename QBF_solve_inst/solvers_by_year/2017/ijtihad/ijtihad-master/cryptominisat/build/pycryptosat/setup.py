#
# CryptoMiniSat
#
# Copyright (c) 2009-2014, Mate Soos. All rights reserved.
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.


import sys
from distutils.core import setup, Extension
from distutils import sysconfig

cconf = """-I/usr/include/python3.5m -I/usr/include/python3.5m  -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O2 -pipe -fstack-protector-strong -DNDEBUG -g -fwrapv -O3 -Wall -Wstrict-prototypes
""".split(" ")
ldconf = """ -L/usr/lib -lpython3.5m -lpthread -ldl  -lutil -lm  -Xlinker -export-dynamic
""".split(" ")
is_apple = """"""

def cleanup(dat):
    ret = []
    for elem in dat:
        elem = elem.strip()
        if elem != "":
            ret.append(elem)

    if is_apple != "":
        for x in ret:
            if x == "-lpython" or x == "-lframework":
                x = "-undefined dynamic_lookup"

    return ret
    # return []

cconf = cleanup(cconf)
ldconf = cleanup(ldconf)
# print "Extra C flags from python-config:", cconf
# print "Extra libraries from python-config:", ldconf


def _init_posix(init):
    """
    Forces g++ instead of gcc on most systems
    credits to eric jones (eric@enthought.com) (found at Google Groups)
    """
    def wrapper():
        init()

        config_vars = sysconfig.get_config_vars()  # by reference
        if config_vars["MACHDEP"].startswith("sun"):
            # Sun needs forced gcc/g++ compilation
            config_vars['CC'] = 'gcc'
            config_vars['CXX'] = 'g++'

        # FIXME raises hardening-no-fortify-functions lintian warning.
        else:
            # Non-Sun needs linkage with g++
            config_vars['LDSHARED'] = 'g++ -shared -g -W -Wall -Wno-deprecated'

        config_vars['CFLAGS'] = '-g -W -Wall -Wno-deprecated -std=c++11'
        config_vars['OPT'] = '-g -W -Wall -Wno-deprecated -std=c++11'

    return wrapper

sysconfig._init_posix = _init_posix(sysconfig._init_posix)

__version__ = '5.0.1'

ext_kwds = dict(
    name = "pycryptosat",
    sources = ["/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/python/pycryptosat.cpp"],
    define_macros = [],
    extra_compile_args = cconf + ['-I/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat', '-I/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/cmsat5-src'],
    extra_link_args = ldconf,
    language = "c++",
    library_dirs=['.', '/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/build/lib'],
    libraries = ['cryptominisat5']
)

setup(
    name = "pycryptosat",
    version = __version__,
    author = "Mate Soos",
    author_email = "soos.mate@gmail.com",
    url = "https://github.com/msoos/cryptominisat",
    license = "LGPLv2",
    classifiers = [
        "Development Status :: 4 - Beta",
        "Intended Audience :: Developers",
        "Operating System :: OS Independent",
        "Programming Language :: C++",
        "Programming Language :: Python :: 2",
        "Programming Language :: Python :: 2.5",
        "Programming Language :: Python :: 2.6",
        "Programming Language :: Python :: 2.7",
        "Topic :: Utilities",
    ],
    ext_modules = [Extension(**ext_kwds)],
    py_modules = ['pycryptosat'],
    description = "bindings to CryptoMiniSat (a SAT solver)",
    long_description = open('/home/vedad/Informatik/ijtihad-dev/ijtihad/cryptominisat/python/README.rst').read(),
)
