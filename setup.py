import os
import glob
from setuptools import setup, Extension

SOURCE_FILES = [os.path.join('http-parser', 'http_parser.c')] + \
               sorted(glob.glob(os.path.join('bjoern', '*.c')))

extra_compile_args = ['-std=c99', '-fno-strict-aliasing', '-fcommon',
                      '-fPIC', '-Wall', '-Wno-unused-parameter',
                      '-Wno-missing-field-initializers', '-g']

print(os.name)
if os.name == 'nt':
    libev = r"C:\xavierdupre\__home_\GitHub\pymyinstall\whl\windows\gevent\deps\libev"
    SOURCE_FILES += [_ for _ in sorted(glob.glob(os.path.join(libev, '*.c'))) if 'epoll' not in _]
    bjoern_extension = Extension(
        '_bjoern',
        sources       = SOURCE_FILES,
        include_dirs  = ['http-parser', libev],
        define_macros = [('WANT_SENDFILE', '1'),
                         ('WANT_SIGINT_HANDLING', '1')],
    )

else:
    extra_compile_args.append('-Wextra')
    bjoern_extension = Extension(
        '_bjoern',
        sources       = SOURCE_FILES,
        libraries     = ['libev'],
        include_dirs  = ['http-parser', '/usr/include/libev'],
        define_macros = [('WANT_SENDFILE', '1'),
                         ('WANT_SIGINT_HANDLING', '1')],
    )


setup(
    name         = 'bjoern',
    author       = 'Jonas Haag',
    author_email = 'jonas@lophus.org',
    license      = '2-clause BSD',
    url          = 'https://github.com/jonashaag/bjoern',
    description  = 'A screamingly fast Python 2 + 3 WSGI server written in C.',
    version      = '2.2.3',
    classifiers  = ['Development Status :: 4 - Beta',
                    'License :: OSI Approved :: BSD License',
                    'Programming Language :: C',
                    'Programming Language :: Python :: 2',
                    'Programming Language :: Python :: 3',
                    'Topic :: Internet :: WWW/HTTP :: WSGI :: Server'],
    py_modules   = ['bjoern'],
    ext_modules  = [bjoern_extension]
)
