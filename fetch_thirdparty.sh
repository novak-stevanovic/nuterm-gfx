#!/bin/bash

rm -f include/shared/_uconv.h
rm -f include/shared/sarena.h

rm -rf include/shared/temp
mkdir -p include/shared/temp

cd include/shared/temp

git clone https://github.com/novak-stevanovic/uconv
git clone https://github.com/novak-stevanovic/sarena

cd ..

cp temp/uconv/include/uconv.h _uconv.h
cp temp/sarena/include/sarena.h sarena.h

rm -rf temp
