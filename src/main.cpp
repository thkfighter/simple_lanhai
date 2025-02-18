﻿/**
 * CN:
 * 主程序入口
 * 1.将SDK文件夹拷贝到自己的工程源码的当前目录
 * 2.引用standard_interface.h头文件
 *
 * EN:
 * main program entry
 * 1. Copy the files of the specified platform and the common basic files (data.h, parse.cpp.user.cpp) under the SDK to the current directory of your own project source code
 * 2. Reference data.h,udp_linux.h header file
 */

#include "standard_interface.h"
#include "simple/BinaryInterfaceServer.h"
#include "simple/LaserMessage.h"
#include <Poco/BinaryWriter.h>
#include <stdio.h>
#include <string.h>

bool debug_{false};
LaserMessage simple_datagram;
BinaryInterfaceServer *server;

bool serialize_and_send_datagram_poco(LaserMessage &simple_datagram)
{
	const size_t resulting_msg_size = 2											// scanNum
									  + 5 * 8									// time_start, uniqueId, duration_beam, duration_scan, duration_rotate
									  + 6 * 4									// numBeams, angleStart, angleEnd, angleInc, minRange, maxRange
									  + 4										// ranges->length
									  + simple_datagram.rangeArraySize * 4		// ranges->elements
									  + 1										// hasIntensities
									  + 2 * 4									// minIntensity, maxIntensity
									  + 4										// intensities->length
									  + simple_datagram.intensityArraySize * 4; // intensities->elements
	Poco::Buffer<char> buffer(resulting_msg_size);
	Poco::MemoryBinaryWriter writer(buffer, Poco::BinaryWriter::StreamByteOrder::LITTLE_ENDIAN_BYTE_ORDER);

	writer << simple_datagram.scanNum;
	writer << simple_datagram.time_start;
	writer << simple_datagram.uniqueId;
	writer << simple_datagram.duration_beam;
	writer << simple_datagram.duration_scan;
	writer << simple_datagram.duration_rotate;
	writer << simple_datagram.numBeams;
	writer << simple_datagram.angleStart;
	writer << simple_datagram.angleEnd;
	writer << simple_datagram.angleInc;
	writer << simple_datagram.minRange;
	writer << simple_datagram.maxRange;

	// writer.writeRaw(simple_datagram.ranges, simple_datagram.ranges.size() * sizeof(float));
	writer << simple_datagram.rangeArraySize;

	if (debug_)
		std::cout << "\nRanges:\n";
	for (const auto range : simple_datagram.ranges)
	{
		writer << (std::isnan(range) ? -1e4f : range);
		if (debug_)
			std::cout << range << ", ";
	}

	writer << static_cast<char>(simple_datagram.hasIntensities);
	writer << simple_datagram.minIntensity;
	writer << simple_datagram.maxIntensity;
	writer << simple_datagram.intensityArraySize;

	if (debug_)
		std::cout << "\nIntensities:\n";
	for (const auto intensity : simple_datagram.intensities)
	{
		writer << intensity;
		if (debug_)
			std::cout << intensity << ", ";
	}

	writer.flush();
	if (resulting_msg_size != buffer.size())
	{
		std::cerr << "Message sizes mismatch!" << std::endl;
		return false;
	}

	// server->write(&buffer, sizeof(buffer));
	server->write(buffer.begin(), buffer.size());
	// TODO Return a variable by reference
	return true;
}

// 传入回调指针的方式打印
void CallBackMsg(int msgtype, void *param, int length)
{
	// static uint16_t scan_num{0};
	// static uint64_t unique_id{0};
	// static double last_timestamp{0.0};
	// static double current_timestamp{0.0};
	static int last_idx{-1};

	switch (msgtype)
	{
	// 实时雷达点云数据
	case 1:
	{
		UserData *pointdata = (UserData *)param;
		if (pointdata->type == FRAMEDATA)
		{
			if (pointdata->idx == last_idx)
				break;
			last_idx = pointdata->idx;

			// simple_datagram.scanNum = scan_num++;
			// simple_datagram.uniqueId = unique_id++;
			simple_datagram.scanNum = pointdata->idx;
			simple_datagram.uniqueId = pointdata->idx;

			simple_datagram.numBeams = pointdata->data.framedata.N;
			simple_datagram.rangeArraySize = simple_datagram.numBeams;
			simple_datagram.intensityArraySize = simple_datagram.numBeams;
			simple_datagram.time_start = pointdata->data.framedata.ts[0] + static_cast<double>(pointdata->data.framedata.ts[1]) / 10e6;
			simple_datagram.angleInc = 2 * M_PI / (simple_datagram.numBeams - 1);
			simple_datagram.ranges.resize(simple_datagram.numBeams);
			simple_datagram.intensities.resize(simple_datagram.numBeams);
			simple_datagram.duration_beam = simple_datagram.duration_scan / simple_datagram.numBeams;

			// std::cout << "FRAMEDATA";
			// printf("frame idx:%d  %s\t%d \t num:%d timestamp:%d.%d\n", pointdata->idx, pointdata->connectArg1, pointdata->connectArg2, pointdata->data.framedata.N, pointdata->data.framedata.ts[0], pointdata->data.framedata.ts[1]);
			printf("frame idx:%d  %s:%d  numBeams:%d  timestamp:%d.%06d\n", pointdata->idx, pointdata->connectArg1, pointdata->connectArg2, pointdata->data.framedata.N, pointdata->data.framedata.ts[0], pointdata->data.framedata.ts[1]);

			for (int i = 0; i < pointdata->data.framedata.N; i++)
			{
				// printf("%s\t%d \t%.5f\t%.3f\t%d\n", pointdata->connectArg1, pointdata->connectArg2, pointdata->data.framedata.data[i].angle, pointdata->data.framedata.data[i].distance, pointdata->data.framedata.data[i].confidence);
				// printf("%.5f\t%.3f\t%d,\t", pointdata->data.framedata.data[i].angle, pointdata->data.framedata.data[i].distance, pointdata->data.framedata.data[i].confidence);

				simple_datagram.ranges[i] = pointdata->data.framedata.data[i].distance;
				simple_datagram.intensities[i] = pointdata->data.framedata.data[i].confidence;
				// std::cout << simple_datagram.ranges[i] << ", " << simple_datagram.intensities[i] << "; ";
			}
			serialize_and_send_datagram_poco(simple_datagram);
		}
		else
		{
			std::cout << "SPANDATA";
			// printf("frame idx:%d  %s\t%d \t num:%d timestamp:%d.%d\n", pointdata->idx, pointdata->connectArg1, pointdata->connectArg2, pointdata->data.spandata.data.N, pointdata->data.spandata.data.ts[0], pointdata->data.spandata.data.ts[1]);
			// for (int i = 0; i < pointdata->data.spandata.data.N; i++)
			// {
			// 	printf("%s\t%d \t%.5f\t%.3f\t%d\n", pointdata->connectArg1, pointdata->connectArg2, pointdata->data.spandata.data.points[i].angle, pointdata->data.spandata.data.points[i].distance, pointdata->data.spandata.data.points[i].confidence);
			// }
		}
		break;
	}
	// 实时报警数据
	case 2:
	{
		LidarMsgHdr *zone = (LidarMsgHdr *)param;
		uint32_t event = zone->events;
		std::string text;
		if (zone->flags % 2 == 1)
		{
			// 硬件报警信息
			if (getbit(event, 0) == 1)
				text += "供电不足";
			if (getbit(event, 1) == 1)
				text += "电机堵转足";
			if (getbit(event, 2) == 1)
				text += "测距模块温度过高";
			if (getbit(event, 3) == 1)
				text += "网络错误";
			if (getbit(event, 4) == 1)
				text += "测距模块无输出";
			// printf("alarm MSG:%s\n", text.c_str());
		}
		if (zone->flags >= 0x100)
		{
			// 防区报警信息
			if (getbit(event, 12) == 1)
				text += "观察！！！";
			if (getbit(event, 13) == 1)
				text += "警戒！！！";
			if (getbit(event, 14) == 1)
				text += "报警！！！";
			if (getbit(event, 15) == 1)
				text += "遮挡！";
			if (getbit(event, 16) == 1)
				text += "无数据";
			if (getbit(event, 17) == 1)
				text += "无防区设置";
			if (getbit(event, 18) == 1)
				text += "系统内部错误";
			if (getbit(event, 19) == 1)
				text += "系统运行异常";
			if (getbit(event, 20) == 1)
				// 和上面的第四项重复，这里屏蔽
				// text+='网络错误\n'
				if (getbit(event, 21) == 1)
					text += "设备更新中";
			if (getbit(event, 22) == 1)
				text += "零位输出";
			// printf("Active zone:%d\tMSG:%s\n", zone->zone_actived, text.c_str());
		}
		break;
	}
	// 获取网络款雷达的全局参数
	case 3:
	{
		EEpromV101 *eepromv101 = (EEpromV101 *)param;
		// 类型，编号，序列号
		printf("dev info: 设备编号:%d\t 序列号:%s\t 类型:%s\n", eepromv101->dev_id, eepromv101->dev_sn, eepromv101->dev_type);
		// ip地址 子网掩码 网关地址 默认目标IP  默认目标udp端口号  默认UDP对外服务端口号
		char tmp_IPv4[16] = {0};
		char tmp_mask[16] = {0};
		char tmp_gateway[16] = {0};
		char tmp_srv_ip[16] = {0};

		sprintf(tmp_IPv4, "%d.%d.%d.%d", eepromv101->IPv4[0], eepromv101->IPv4[1], eepromv101->IPv4[2], eepromv101->IPv4[3]);
		sprintf(tmp_mask, "%d.%d.%d.%d", eepromv101->mask[0], eepromv101->mask[1], eepromv101->mask[2], eepromv101->mask[3]);
		sprintf(tmp_gateway, "%d.%d.%d.%d", eepromv101->gateway[0], eepromv101->gateway[1], eepromv101->gateway[2], eepromv101->gateway[3]);
		sprintf(tmp_srv_ip, "%d.%d.%d.%d", eepromv101->srv_ip[0], eepromv101->srv_ip[1], eepromv101->srv_ip[2], eepromv101->srv_ip[3]);

		printf("dev info: ip地址:%s 子网掩码:%s 网关地址:%s 默认目标IP:%s  默认目标udp端口号:%d   默认UDP对外服务端口号:%d\n",
			   tmp_IPv4, tmp_mask, tmp_gateway, tmp_srv_ip, eepromv101->srv_port, eepromv101->local_port);

		/*char tmp_ranger_bias[8] = {0};
		memcpy(tmp_ranger_bias, eepromv101->ranger_bias, sizeof(eepromv101->ranger_bias) - 1);*/
		// 转速 ,电机启动参数,FIR滤波阶数，圈数，分辨率，开机自动上传，固定上传，数据点平滑，去拖点，记录校正系数，网络心跳，记录IO口极性
		printf("dev info: 转速:%d 电机启动参数:%d FIR滤波阶数:%d 圈数:%d  分辨率:%d   开机自动上传:%d 固定上传:%d  数据点平滑:%d 去拖点:%d   网络心跳:%d  记录IO口极性:%d\n",
			   eepromv101->RPM, eepromv101->RPM_pulse, eepromv101->fir_filter, eepromv101->cir, eepromv101->with_resample, eepromv101->auto_start,
			   eepromv101->target_fixed, eepromv101->with_smooth, eepromv101->with_filter, eepromv101->net_watchdog, eepromv101->pnp_flags);

		printf("dev info:平滑系数：%d  激活防区：%d  上传数据类型：%d\n", eepromv101->deshadow, eepromv101->zone_acted, eepromv101->should_post);
		break;
	}

	// 获取雷达时间戳打印信息(网络款为雷达返回时间戳，串口款为本机接收到的时间戳)
	case 4:
	{
		DevTimestamp *devtimestamp = (DevTimestamp *)param;
		printf("DevTimestamp: lidar_ip:%s lidar_port:%d time:%d delay:%d\n", devtimestamp->ip, devtimestamp->port, devtimestamp->timestamp, devtimestamp->delay);
		break;
	}
	// 打印信息(也可以作为日志写入)
	case 8:
	{
		char result[512];
		memcpy(result, param, length);
		printf("info: %s\n", result);
		break;
	}
	case 9:
	{
		char result[512];
		memcpy(result, param, length);
		printf("error: %s\n", result);
		break;
	}
	}
	fflush(stdout);
}
int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Incorrect number of parameters  %d\n usage : ./demo  ../../config/xxx.txt   At least one txt of Lidar\n", argc);
		return ARG_ERROR_NUM;
	}
	server = new BinaryInterfaceServer(4242);
	BlueSeaLidarSDK *lidarSDK = BlueSeaLidarSDK::getInstance();
	int lidar_sum = argc - 1;
	for (int i = 0; i < lidar_sum; i++)
	{
		const char *cfg_file_name = argv[i + 1];
		// 根据配置文件路径添加相关的雷达
		int lidarID = lidarSDK->addLidarByPath(cfg_file_name);
		if (!lidarID)
		{
			printf("config file %s does not exist\n", cfg_file_name);
			return -1;
		}

		// 读取雷达的全局参数
		EEpromV101 eepromv101;
		if (lidarSDK->GetDevInfo(lidarID, &eepromv101))
			CallBackMsg(3, &eepromv101, sizeof(EEpromV101));
		simple_datagram.duration_rotate = 60.0 / eepromv101.RPM;
		simple_datagram.duration_scan = simple_datagram.duration_rotate;

		// 传入数据信息的回调函数
		lidarSDK->setCallBackPtr(lidarID, CallBackMsg);

		// 连接指定雷达，以及相关的线程池
		if (!lidarSDK->openDev(lidarID))
		{
			printf("open lidar failed:%d\n", lidarID);
			return -2;
		}
		printf("SDK version:%s\n", lidarSDK->getVersion());
		// sleep(2);

		// 切换防区(仅防区款)
		// char zone = '9';
		// if (!lidarSDK->ZoneSection(lidarID, zone))
		//{
		//	printf("switch the specified zone failed!\n");
		//}

		// 控制雷达的启动，停止，重新旋转
		//  lidarSDK->ControlDrv(lidarID,6,"LSTARH");
		//  sleep(2);
		//  lidarSDK->ControlDrv(lidarID,6,"LSTOPH");
		//  sleep(2);
		//  lidarSDK->ControlDrv(lidarID,6,"LRESTH");
		//  sleep(3);

		// 设置雷达的网络参数(仅限网络款/防区款)(掉电保存)(修改后需要重新调整目标雷达的ip)
		// lidarSDK->SetUDP(lidarID,"192.168.0.140","255.255.255.0","192.168.0.1",6543);

		// 设置雷达的上传网络参数地址(仅限网络款/防区款)(掉电保存)
		// lidarSDK->SetDST(lidarID,"192.168.0.49",6543);

		// bool ret,ret2,ret3;
		////转速修改(掉电不保存)
		// ret=lidarSDK->SetRPM(lidarID,3000);
		////滤波(掉电不保存)
		// ret2=lidarSDK->SetDSW(lidarID,true);
		////去拖点(掉电不保存)
		// ret3=lidarSDK->SetSMT(lidarID,true);
		// printf("rpm:%d fitter:%d  smooth:%d\n",ret,ret2,ret3);

		// ////固定上传(仅限网络款/防区款)(掉电保存)
		// ret = lidarSDK->SetTFX(lidarID,false);
		// //数据上传类型(仅限网络款/防区款)(掉电保存)
		// ret2=lidarSDK->SetPST(lidarID,3);
		// //设置雷达编号(仅限网络款/防区款)(掉电保存)
		// ret3=lidarSDK->SetDID(lidarID,999);
		// printf("tfx:%d post:%d num:%d\n",ret,ret2,ret3);
	}

	simple_datagram.angleStart = static_cast<float>(-M_PI);
	simple_datagram.angleEnd = static_cast<float>(M_PI);
	simple_datagram.minRange = 0.0f;
	simple_datagram.maxRange = 60.0f;
	simple_datagram.hasIntensities = true;
	simple_datagram.minIntensity = 0.0f;
	simple_datagram.maxIntensity = 100.0f;

	while (1)
	{
		// RunConfig* one = BlueSeaLidarSDK::BlueSeaLidarSDK::getInstance()->getLidar(1);
		// RunConfig* two = BlueSeaLidarSDK::BlueSeaLidarSDK::getInstance()->getLidar(2);
		// 保持进程不退出
		msleep(100);
	}
	return 0;
}
