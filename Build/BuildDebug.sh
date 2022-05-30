relative_path="$(dirname $(realpath $0))"
cd $relative_path

mkdir _DEBUG_
cd _DEBUG_

cmake -DCMAKE_BUILD_TYPE=DEBUG .. -G "Unix Makefiles"
make
