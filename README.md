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

## Use AppImage

Download the corresponding version of AppImage according to the Ubuntu version you use from this repository's releases.  
Set the file to be executable.  
Run the LiDAR's driver.

> simple_lanhai-ubuntu2004-x86_64.AppImage LDS-E400-E.txt

## Set laser scanner address in ROKIT Locator

172.17.0.1:4242

# Related address links

ROS drive  
https://github.com/BlueSeaLidar/bluesea2  
https://github.com/BlueSeaLidar/bluesea-ros2

---

# Notes

End of line sequence in the .desktop file must be LF, otherwise an error will be thrown.

> RROR: Could not find suitable icon for Icon entry
