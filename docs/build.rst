Compilation
============

CHM uses `conan <https://conan.io/>`__ to manage and build all
dependencies. Because of the various requirements on build
configuration, versions, and interdependencies, using system libraries
it not supported.

All of the CHM dependencies are built on Travis-CI and uploaded to the
`bintray <https://bintray.com/chrismarsh/CHM>`__ repository to serve
prebuilt binaries. This means that *if* the CHM build is done with
supported compilers and operating system (described later), the
dependencies do not need to be built by the end user.

Build requirements
*******************

Linux and MacOS are the only supported environments. The following have been tested

=======  =====  ========  =====
   Linux          MacOS
--------------  ---------------
Ubuntu   18.04  Moajave   10.x
  -      20.04  Catalina  15.15
=======  =====  ========  =====        


gcc (libc 2.27+): 7.x, 8.x

If bintray binaries can be used, the only requirements are: 

   - conan >=1.21 
   - cmake >=3.16 
   - C++14 compiler (gcc 7.2+)
   - Fortran 90+ compiler (gfortran)

If some dependencies need to be built from source, ensure the following
are also installed 

   - autotools 
   - m4

.. note::
   For distributed MPI support, optionally ensure MPI is installed


On MacOS, `homebrew <https://brew.sh/>`__ should be used to install
cmake and conan. Macport based installs likely work, but have not been
tested.


Compile
********

Throughout, this document assumes a working development environment, but
a blank conan environment. 

Setup conan
-----------

::

   conan profile new default --detect
   conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
   conan remote add CHM https://api.bintray.com/conan/chrismarsh/CHM
   conan profile update settings.compiler.cppstd=14 default  

conan needs to be told to use new C++11 ABI. If using clang (e.g.,
MacOs), do

::

   conan profile update settings.compiler.libcxx=libc++ default  #with clang

and if using gcc, do

::

   conan profile update settings.compiler.libcxx=libstdc++11 default  #with gcc

If you change compilers, such as on a cluster with a modules system, you
can rerun

::

   conan profile new default --detect --force

to detect the new compiler settings. The ``cppstd`` and ``libcxx``
settings need to be reapplied once this is done.

Intel compiler
~~~~~~~~~~~~~~

If the Intel compiler is being used (this is optional), ensure the Intel compilervars is sourced, e.g.,

::

   source /opt/intel/bin/compilervars.sh intel64

prior to running the conan. Use the above gcc settings for conan.

Setup CHM source folders
------------------------

An out of source build should be used. That is, build in a seperate folder removed from the CHM source. This makes it easier to clean up
and start from scratch. An example is given below:

::

   cd ~/
   git clone https://github.com/Chrismarsh/CHM
   (cd CHM && git submodule update --init --recursive)

   mkdir ~/build-CHM && cd ~/build-CHM

.. note::
   The follow instructions assume that they are invoked from within ``~/build-CHM`` (or your equivalent).

Setup dependencies
------------------

Install the dependencies into your local conan cache (``~/.conan/data``)

::
   
   conan install ~/CHM -if=.

Further, this command will produce the ``FindXXX.cmake`` files required for the
CHM build.

If you need to build dependencies from source (this is likely), use the
``--build missing`` option like:

::

   conan install ~/CHM -if=. --build missing

MPI
~~~

If MPI is to be used, the prebuilt Boost dependency from Conan will not
have it enabled. Thus, it should be rebuilt locally to ensure the local
MPI configuration is correctly used:

::

   conan install ~/CHM -if=. --build boost



OpenMP
~~~~~~

On MacOS, the openmp library should be installed via homebrew:

::

   brew install libomp

Run cmake
---------

You can set the install prefix to be anywhere, such as shown in the
example below

::

   cmake ~/CHM -DCMAKE_INSTALL_PREFIX=/opt/chm-install

This should complete without any errors. Both ``ninja`` and ``make``
(this is the default) are supported. To use ``ninja``, add

::

   cmake ~/CHM -DCMAKE_INSTALL_PREFIX=/opt/chm-install -G "Ninja"

Ninja speeds up compilation of CHM by ~6%.

The default build option creates an optimizted “release” build. To build
a debug build, use ``-DCMAKE_BUILD_TYPE=Debug``.


Intel compiler
~~~~~~~~~~~~~~

If the Intel compiler is used, add the following cmake flags:

::

   -DCMAKE_CXX_COMPILER=icpc -DCMAKE_C_COMPILER=icc -DCMAKE_FORTRAN_COMPILER=ifort

Building
--------

Using make

::

   make -jN CHM

where N is the number of parallel jobs (e.g., total core count).

Using Ninja

::

   ninja -C . 

Run tests
---------

Tests can be enabled with ``-DBUILD_TESTS=TRUE`` and run with
``make check``/ ``ninja check``

Install
-------

``make install``/``ninja install``

Troubleshooting
***************

TCMALLOC
--------

TCmalloc may need to be disabled and can be done via
``-DUSE_TCMALLOC=FALSE``

gepertool heap profiler & libunwnd
----------------------------------

Some machines do not build gperftools with the heap profiling correctly.
This can be disabled when building gperftools

::

   conan install ~/code/CHM/ -if=. --build missing -o gperftools:heapprof=False

Matlab
------

OSX
~~~

-  Create a symbolic link from /usr/bin to the matlab install
-  ``sudo ln -s /Applications/MATLAB_R2013a.app/bin/matlab /usr/bin/matlab``

Linux:
~~~~~~

Usage of the matlab engine requires installing ``csh``


Building on WestGrid
*********************

To build on WestGrid’s Graham machine, all dependencies must be built
from source to ensure the correct optimizations are used. As well, Conan
detects libc versions via compiler version, however on the CentOS 7
system on Graham, the libc is much older than the compiler would
suggest, thus the prebuilt libraries will not link correctly.


Setup Conan
-----------

::

   module load gcc/7.3.0
   module load python/3.7.4
   module load cmake/3.16
   virtualenv ~/conan_env
   source ~/conan_env/bin/activate
   pip install conan
   conan profile new default --detect
   conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
   conan remote add CHM https://api.bintray.com/conan/chrismarsh/CHM
   conan profile update settings.compiler.cppstd=14 default  
   conan profile update settings.compiler.libcxx=libstdc++11 default  #with gcc

If a different gcc version is used,

::

   conan profile new default --detect --force 
   conan profile update settings.compiler.cppstd=14 default  
   conan profile update settings.compiler.libcxx=libstdc++11 default  #with gcc

Needs to be re-run. Doing so will require a full rebuilt of all
dependencies.

Building CHM
------------

Ensure the environment is correctly setup

::

   module load cmake/3.16
   module load gcc/7.3.0
   module load python/3.7.4
   source ~/conan_env/bin/activate

then build dependencies and CHM as described above with the following
change

::

   conan install ~/CHM -if=. --build 

To ensure *all* dependencies are built from source.
