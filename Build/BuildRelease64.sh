mkdir _RELEASE_64_
cd _RELEASE_64_

cmake -DCMAKE_BUILD_TYPE=Release -DBIT:INT=64 .. -G "Unix Makefiles"
make
