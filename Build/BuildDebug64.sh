mkdir _DEBUG_64_
cd _DEBUG_64_

cmake -DCMAKE_BUILD_TYPE=Debug -DBIT:INT=64 .. -G "Unix Makefiles"
make
