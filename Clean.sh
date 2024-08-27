find . -name _DEBUG_32_ -exec rm -rf {} \;
find . -name _DEBUG_64_ -exec rm -rf {} \;
find . -name _RELEASE_32_ -exec rm -rf {} \;
find . -name _RELEASE_64_ -exec rm -rf {} \;

find -name '*.log' -delete;
