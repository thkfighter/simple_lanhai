# Notes
End of line sequence in the .desktop file must be LF, otherwise an error will be thrown.
> RROR: Could not find suitable icon for Icon entry


# Interface
| Device | IP | Subnet mask | Gateway |
| --- | --- | --- | --- |
| LiDAR | 192.168.158.98 | 255.255.255.0 | 192.168.158.1 |
| Computer | 192.168.158.15 | 255.255.255.0 | 192.168.158.1 |

LDS-E400-E

revolution speed: 600~3000 rpm (minimum interval 300 rpm)

# USB-LIDAR DESCRIPTION
windows/linux SDK and Demo programs for lanhai 2d lidar

#HOW TO BUILD AND USE#

## LINUX
Prerequisite: g++ and gcc must be installed

	cmake CMakeList.txt
	make
	make install
	cd tools
	./demo ../config/xxxx.txt  

## WINDOWS ##
windows powershell

    use cmake_gui  build ,open project and compile
    need mfc support
	Detailed operation reference  docs/SDK开发手册.docx
open project by  visual stdio 2013  or greater than this version 
Generate a solution

**Special instructions: Please refer to the corresponding product's documentation to select the specified configuration file, and ensure that the software can be called normally only after the hardware works normally.**

## config file from windows to linux ##
	vim xxxx.txt
	set ff=unix

#  Related address links #

[downland ros drive](https://github.com/BlueSeaLidar/sdk2)




