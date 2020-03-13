#!/bin/sh
make
mv main target

cd module
make
insmod key.ko&
cd ..

./target