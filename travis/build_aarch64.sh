sudo apt install libpoco-dev libboost-all-dev cmake
sudo apt install strace patchelf fakeroot
pip install appimage-builder

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=AppDir
# https://appimage-builder.readthedocs.io/en/latest/intro/tutorial.html
appimage-builder --generate
appimage-builder --recipe AppImageBuilder.yml
# Error: packaging.version.InvalidVersion: Invalid version: '1.21.1ubuntu2.1'
# pip install packaging==21.3