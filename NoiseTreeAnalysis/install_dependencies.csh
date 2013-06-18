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
wget http://www.hepforge.org/archive/geners/geners-1.3.0.tar.gz
tar -xzf geners-1.3.0.tar.gz
rm -f geners-1.3.0.tar.gz
cd geners-1.3.0
setenv CXXFLAGS -std=c++0x
./configure --with-pic --prefix=${HOME}/local
make
make install
make check

# Installing "FFTJet". See http://fftjet.hepforge.org
cd ${HOME}/local/src
wget http://www.hepforge.org/archive/fftjet/fftjet-1.4.1.tar.gz
tar -xzf fftjet-1.4.1.tar.gz
rm -f fftjet-1.4.1.tar.gz
cd fftjet-1.4.1
setenv CXXFLAGS -std=c++0x
setenv F77 gfortran
setenv PKG_CONFIG_PATH ${HOME}/local/lib/pkgconfig
./configure --with-pic --prefix=${HOME}/local
make
make install
make check

# Installing "NPStat". See http://npstat.hepforge.org
cd ${HOME}/local/src
wget http://www.hepforge.org/archive/npstat/npstat-2.0.0.tar.gz
tar -xzf npstat-2.0.0.tar.gz
rm -f npstat-2.0.0.tar.gz
cd npstat-2.0.0
setenv CXXFLAGS -std=c++0x
setenv F77 gfortran
setenv PKG_CONFIG_PATH ${HOME}/local/lib/pkgconfig
./configure --with-pic --prefix=${HOME}/local
make
make install
make check
