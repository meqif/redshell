#!/bin/bash
cmockery_version="0.1.2"
cmockery_url="http://cmockery.googlecode.com/files/cmockery-${cmockery_version}.tar.gz"

mkdir ./build

if [ -f "build/cmockery/lib/libcmockery.a" ]; then
	exit
fi

if [ -f "/usr/local/lib/libcmockery.a" ]; then
	mkdir -p `pwd`/build/cmockery/{lib,include}
	cp /usr/local/lib/libcmockery.a "$(pwd)/build/cmockery/lib"
	cp -r /usr/local/include/google "$(pwd)/build/cmockery/include"
elif [ -f "/usr/local/lib/libcmockery.a" ]; then
	mkdir -p `pwd`/build/cmockery/{lib,include}
	cp /usr/lib/libcmockery.a "$(pwd)/build/cmockery/lib"
	cp -r /usr/include/google "$(pwd)/build/cmockery/include"
else
	pushd build
	prefix=`pwd`/cmockery
	wget -c $cmockery_url
	tar -xzf cmockery-${cmockery_version}.tar.gz
	pushd cmockery-${cmockery_version}
	./configure --prefix=$prefix && make && make install
	popd
	popd
fi
