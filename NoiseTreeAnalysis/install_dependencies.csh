# Making sure that all prerequisites are there. This should
# work with Scientific Linux, Redhat Enterprise, fedora, etc.
# Run the following as root (or ask your administrator).
# Note that CMSSW already depends on all of these packages.
#
# yum install bzip2
# yum install bzip2-devel
# yum install zlib
# yum install zlib-devel
# yum install blas
# yum install lapack
# yum install fftw
# yum install fftw-devel

# Installing "Geners". See http://geners.hepforge.org
mkdir -p ${HOME}/local/src
cd ${HOME}/local/src
wget http://www.hepforge.org/archive/geners/geners-1.2.0.tar.gz
tar -xzf geners-1.2.0.tar.gz
rm -f geners-1.2.0.tar.gz
cd geners-1.2.0
setenv CXXFLAGS -std=c++0x
./configure --with-pic --prefix=${HOME}/local
make
make install
make check

# Installing "NPStat". See http://npstat.hepforge.org
cd ${HOME}/local/src
wget http://www.hepforge.org/archive/npstat/npstat-1.3.0.tar.gz
tar -xzf npstat-1.3.0.tar.gz
rm -f npstat-1.3.0.tar.gz
cd npstat-1.3.0
setenv CXXFLAGS -std=c++0x
setenv PKG_CONFIG_PATH ${HOME}/local/lib/pkgconfig
./configure --with-pic --prefix=${HOME}/local
make
make install

# The following will not work if you are installing these packages
# with CMSSW set up. This is because "NPStat" is using some Fortran
# libraries (lapack and blas) and "configure", due to some reason,
# wants to search for them on the local machine (e.g., in /usr/lib64).
# This search path interferes with the library search path from
# the CMSSW installation.
#
# To make sure things work as expected, you will need to edit the
# Makefile in the ${HOME}/local/src/npstat-1.3.0/examples/C++ directory
# and manually change the "FLIBS" variable so that it looks something
# like "FLIBS = -lgfortran -lm", without any search path specified.
# 
make check
