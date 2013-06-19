# Run this script with tcsh (or source it in your interactive tcsh session).
#
# If you plan to install the software using compilers distributed
# with CMSSW (for example, in order to get the exact root version
# from the release), make sure to set up the appropriate CMSSW
# environment before running this script.
#
# If you are not using CMSSW, make sure that all prerequisites
# are there. This should work with Scientific Linux, Redhat Enterprise,
# fedora, etc. Run the following as root (or ask your administrator).
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

# Package versions
set geners_version = 1.3.0
set fftjet_version = 1.4.1
set npstat_version = 2.0.0

set geners_package = geners-${geners_version}
set fftjet_package = fftjet-${fftjet_version}
set npstat_package = npstat-${npstat_version}

# Download the software packages to install
mkdir -p ${HOME}/local/src
cd ${HOME}/local/src
rm -f ${geners_package}.tar.gz ${fftjet_package}.tar.gz ${npstat_package}.tar.gz
wget http://www.hepforge.org/archive/geners/${geners_package}.tar.gz
wget http://www.hepforge.org/archive/fftjet/${fftjet_package}.tar.gz
wget http://www.hepforge.org/archive/npstat/${npstat_package}.tar.gz

# Uninstall and cleanup previous versions of the software
foreach dir (geners-*.*.* fftjet-*.*.* npstat-*.*.*)
    cd ${HOME}/local/src
    if (-d $dir) then
        echo Uninstalling $dir
        cd $dir
        make uninstall
        cd ..
        rm -fr $dir
    endif
end

# Proper environmental setup for "configure"
setenv CXXFLAGS -std=c++0x
setenv F77 gfortran
setenv PKG_CONFIG_PATH ${HOME}/local/lib/pkgconfig

# Install the packages
foreach package ($geners_package $fftjet_package $npstat_package)
    echo
    echo '**** Installing' $package '****'
    echo
    cd ${HOME}/local/src
    tar -xzf ${package}.tar.gz
    rm -f ${package}.tar.gz
    cd $package
    ./configure --with-pic --prefix=${HOME}/local
    make
    make check
    make install
end

echo
echo Successfully installed $geners_package $fftjet_package $npstat_package
