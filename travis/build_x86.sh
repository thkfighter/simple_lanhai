sudo apt install libpoco-dev libboost-all-dev cmake
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20230711-2/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
sudo mv linuxdeploy-x86_64.AppImage /usr/bin/

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=AppDir
linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage