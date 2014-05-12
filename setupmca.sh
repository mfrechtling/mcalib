#!/bin/bash

if ["$(id -u)" != "0"]; then
    echo "Attempting to install as non-root user"
fi

# apt-get the relevant libraries.
apt-get update
apt-get install -y build-essential
apt-get install -y ocaml
apt-get install -y ocaml-findlib
apt-get install -y m4
apt-get install -y make
apt-get install -y libmpfr-dev
apt-get install -y automake
apt-get install -y texinfo
apt-get install -y vim

ROOT=${PWD}

# Install Cil 1.7.1
cd $ROOT && wget http://downloads.sourceforge.net/project/cil/cil/cil-1.7.1.tar.gz
tar xf cil-1.7.1.tar.gz && mv cil-1.7.1 cil
cp ./cil_ext/* cil/src/ext/.
cd cil && ./configure EXTRAFEATURES='fpsimplify fphooks'
make && make install

# Install MCALIB 2.0.0
cd $ROOT && ./configure
make && make check && make install
ldconfig

