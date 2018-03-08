#!bin/bash

cd bin
rm -rf *
cmake ..
make
if [ "$#" -gt 0 ]; then
   ./src/bin/Face_Client $@
fi
