# Clean.sh

find . -name _DEBUG_32_ -exec rm -rf {} \;
find . -name _DEBUG_64_ -exec rm -rf {} \;
find . -name _RELEASE_32_ -exec rm -rf {} \;
find . -name _RELEASE_64_ -exec rm -rf {} \;

find -name '*.log' -delete;

# MSVC Files
find -name '*.ncb' -delete;
find -name '*.sdf' -delete;
find -name '*.suo' -delete;
find -name '*.user' -delete;
find -name '*.opensdf' -delete;
