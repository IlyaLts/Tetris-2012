mkdir _RELEASE_32_
cd _RELEASE_32_

cmake -DCMAKE_BUILD_TYPE=Release -DBIT:INT=32 .. -G "Unix Makefiles"
make
