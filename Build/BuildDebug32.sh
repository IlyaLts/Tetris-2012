mkdir _DEBUG_32_
cd _DEBUG_32_

cmake -DCMAKE_BUILD_TYPE=Debug -DBIT:INT=32 .. -G "Unix Makefiles"
make
