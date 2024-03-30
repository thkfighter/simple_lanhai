# How to use

## Configure IP addresses

| Device   | IP             | Subnet mask   | Gateway       |
| -------- | -------------- | ------------- | ------------- |
| LiDAR    | 192.168.158.98 | 255.255.255.0 | 192.168.158.1 |
| Computer | 192.168.158.15 | 255.255.255.0 | 192.168.158.1 |

## Edit the LiDAR's configuration

Download the LiDAR's configuration file, LDS-E400-E.txt, from this repository's releases.  
| Item | Description |
| --- | --- |
| rpm: 1800 | revolution speed: 600~3000 rpm, minimum interval 300 rpm, you can set 600, 900, 1200... |
| direction: 1 | -1/0/1  inexecution/clockwise/counter-clockwise, valid for some models, including LDS-E400-E  不执行/顺时针/逆时针，仅部分雷达生效，包含LDS-E400-E | 

## Use AppImage

Download the corresponding version of AppImage according to the Ubuntu version you use from this repository's releases.  
Set the file to be executable.  
Run the LiDAR's driver.

For example, on Ubuntu 20.04, download simple_lanhai-ubuntu2004-x86_64.AppImage and configuration file LDS-E400-E.txt from the repository's releases to a local folder, like ~/rokit/service, 

```bash
cd ~/rokit/service
chmod +x ./simple_lanhai-ubuntu2004-x86_64.AppImage # no sudo
./simple_lanhai-ubuntu2004-x86_64.AppImage
Incorrect number of parameters: 1
Usage: ./simple_lanhai-x86_64.AppImage <log_level> <laser_datagram_port> <path/to/config/__.txt>
        laser_datagram_port: 0-65535
        log_level: trace, debug, info, warn, err, critical and off
Example: ./simple_lanhai-x86_64.AppImage 2112 info config/LDS-E400-E.txt

./simple_lanhai-x86_64.AppImage 2112 info ./LDS-E400-E.txt
```

## Set laser scanner in ROKIT Locator

- laser type: simple
- laser scanner address: 172.17.0.1:<laser_datagram_port>

## Package to AppImage
Make a single executable file including dependencies.

Refer to travis/build.sh and install dependencies.

If you want run travis/build.sh from line 6 to its end, use
```bash
sed -n '6,$p' ./travis/build.sh | bash # TODO something is wrong
```

```bash
sudo apt install libpoco-dev libboost-all-dev cmake
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage
sudo mv linuxdeploy-x86_64.AppImage /usr/bin/

mkdir build
pushd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)
make install DESTDIR=AppDir 
/usr/bin/linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage
popd
```

# Related address links

SDK2  
https://github.com/BlueSeaLidar/sdk2

ROS driver  
https://github.com/BlueSeaLidar/bluesea2  
https://github.com/BlueSeaLidar/bluesea-ros2

---

# Notes

End of line sequence in the .desktop file must be LF, otherwise an error will be thrown.

> RROR: Could not find suitable icon for Icon entry
