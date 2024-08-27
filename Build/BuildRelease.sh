relative_path="$(dirname $(realpath $0))"
cd $relative_path

mkdir _RELEASE_
cd _RELEASE_

cmake -DCMAKE_BUILD_TYPE=RELEASE .. -G "Unix Makefiles"
make
