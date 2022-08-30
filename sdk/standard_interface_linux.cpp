﻿#ifndef __STANDARD_INTERFACE_LINUX_H__
#define __STANDARD_INTERFACE_LINUX_H__


#include "standard_interface.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include"third_party/mongoose/mongoose.h"
RunConfig* g_cfg;
LidarWebService* webservice;
bool read_config(const char* cfg_file_name, RunConfig& cfg)
{
	std::ifstream infile;
	infile.open(cfg_file_name);
	if (!infile.is_open())
		return false;

	std::string s;
	std::string lidar_ip_s, lidar_port_s, local_port_s;
	std::string unit_is_mm_s, with_confidence_s, resample_s;
	std::string with_deshadow_s, with_smooth_s, with_chk_s;
	std::string raw_bytes_s, rpm_s, output_scan_s, output_360_s;
	std::string from_zero_s;
	std::string output_file;
	std::string is_group_listener;
	std::string group_ip;
	std::string type;
	std::string baud_rate, port;
	std::string service_port, is_open_service;
	
	while (getline(infile, s))
	{
		
		std::string tmp;
		std::stringstream linestream(s);
		getline(linestream, tmp, ':');
		if (tmp == "type")
		{
			getline(linestream, type, ':');
			strcpy(cfg.type, type.c_str());
		}
		if (tmp == "baud_rate")
		{
			getline(linestream, baud_rate, ':');
			cfg.baud_rate = atoi(baud_rate.c_str());
		}
		if (tmp == "port")
		{
			getline(linestream, port, ':');
			strcpy(cfg.port, port.c_str());
		}
		if (tmp == "lidar_ip")
		{
			getline(linestream, lidar_ip_s, ':');
			strcpy(cfg.lidar_ip, lidar_ip_s.c_str());
		}
		else if (tmp == "lidar_port")
		{
			getline(linestream, lidar_port_s, ':');
			cfg.lidar_port = atoi(lidar_port_s.c_str());
		}
		else if (tmp == "local_port")
		{
			getline(linestream, local_port_s, ':');
			cfg.local_port = atoi(local_port_s.c_str());
		}
		else if (tmp == "with_confidence")
		{
			getline(linestream, with_confidence_s, ':');
			cfg.with_confidence = atoi(with_confidence_s.c_str());
		}
		else if (tmp == "raw_bytes")
		{
			getline(linestream, raw_bytes_s, ':');
			cfg.data_bytes = atoi(raw_bytes_s.c_str());
		}
		else if (tmp == "unit_is_mm")
		{
			getline(linestream, unit_is_mm_s, ':');
			cfg.unit_is_mm = atoi(unit_is_mm_s.c_str());
		}
		else if (tmp == "with_chk")
		{
			getline(linestream, with_chk_s, ':');
			cfg.with_chk = atoi(with_chk_s.c_str());
		}
		else if (tmp == "with_smooth")
		{
			getline(linestream, with_smooth_s, ':');
			cfg.with_smooth = atoi(with_smooth_s.c_str());
		}
		else if (tmp == "with_deshadow")
		{
			getline(linestream, with_deshadow_s, ':');
			cfg.with_deshadow = atoi(with_deshadow_s.c_str());
		}
		else if (tmp == "resample")
		{
			getline(linestream, resample_s, ':');
			cfg.resample = atoi(resample_s.c_str());
		}
		else if (tmp == "rpm")
		{
			getline(linestream, rpm_s, ':');
			cfg.rpm = atoi(rpm_s.c_str());
		}
		else if (tmp == "output_scan")
		{
			getline(linestream, output_scan_s, ':');
			cfg.output_scan = atoi(output_scan_s.c_str());
		}
		else if (tmp == "output_360")
		{
			getline(linestream, output_360_s, ':');
			cfg.output_360 = atoi(output_360_s.c_str());
		}
		else if (tmp == "from_zero")
		{
			getline(linestream, from_zero_s, ':');
			cfg.from_zero = atoi(from_zero_s.c_str());
		}
		else if (tmp == "output_file")
		{
			getline(linestream, output_file, ':');
			strcpy(cfg.output_file, output_file.c_str());
		}
		else if (tmp == "is_group_listener")
		{
			getline(linestream, is_group_listener, ':');
			cfg.is_group_listener = atoi(is_group_listener.c_str());
		}
		else if (tmp == "group_ip")
		{
			getline(linestream, group_ip, ':');
			strcpy(cfg.group_ip, group_ip.c_str());
		}
		else if (tmp == "service_port")
		{
			getline(linestream, service_port, ':');
			cfg.service_port = atoi(service_port.c_str());;
		}
		else if (tmp == "is_open_service")
		{
			getline(linestream, is_open_service, ':');
			cfg.is_open_service = atoi(is_open_service.c_str());;
		}
	}
	return true;
}

// 释放连接
void StopDrv(RunConfig* run)
{
	run->should_quit = true;
	sleep(2);
}


const char* getVersion()
{
	return SDKVERSION;
}
void* lidar_service(void* param)
{
	webservice->OpenLocalService();
	return SUCCESS;
}
// 打开本地服务
void  OpenLocalService(RunConfig & cfg)
{
	webservice = new LidarWebService(cfg.service_port);
	pthread_create(&cfg.thread, NULL, lidar_service, 0);
}
//关闭本地服务
void  CloseLocalService()
{

}
// 连接雷达，启动子线程
int openDev(RunConfig& cfg)
{
	int fd = -1;//保存句柄变量
	int res = -1;//保存函数返回值
	if (strcmp(cfg.type, "uart") == 0|| strcmp(cfg.type, "vpc") == 0)
	{
		fd = (int)open_serial_port(cfg);
		if (fd <= 0)
		{
			return OPEN_UART_FD_FAILED;
		}
		//传入设备句柄
		cfg.fd = fd;
		g_cfg = &cfg;
		cfg.thread_ID[1] = msgget(0x1234, IPC_CREAT | 0664);
		if (pthread_create(&cfg.thread, NULL, lidar_thread_proc_uart, &cfg) != 0)
			return THREAD_CREATE_FAILED;
		INFO_PR("Lidar is open success\n");
		return SUCCESS;
	}
	else if (strcmp(cfg.type, "udp") == 0)
	{
		fd = (int)open_socket_port(cfg);
		if (fd <= 0)
		{
			return OPEN_UDP_FD_FAILED;
		}
		//传入设备句柄
		cfg.fd = fd;
		g_cfg = &cfg;
		cfg.thread_ID[1] = msgget(0x1234, IPC_CREAT | 0664);
		if (pthread_create(&cfg.thread, NULL, lidar_thread_proc_udp, &cfg) != 0)
			return THREAD_CREATE_FAILED;

		INFO_PR("Lidar is open success\n");
		return SUCCESS;

	}
	else
	{
		return ARG_ERROR_TYPE;
	}
}
int thread_set_run(RunConfig& cfg)
{
	if (strcmp(cfg.type, "uart") == 0||strcmp(cfg.type, "vpc") == 0)
		return pthread_create(&cfg.thread, NULL, lidar_thread_proc_uart, &cfg);
	else if (strcmp(cfg.type, "udp") == 0)
		return pthread_create(&cfg.thread, NULL, lidar_thread_proc_udp, &cfg);
	else
		return ARG_ERROR_TYPE;

}

int getLidarData(long threadID, bool dataGet)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = Print_Point_MSG;
	msg.cmd.str[0] = dataGet;
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index--)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == Print_Point_MSG)
			{
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				if (msg2.cmd.str[0] - '0')
					return SUCCESS;
				else
					return GET_DEVTIMESTAMP_FAILED;
			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return GET_DEVPONIT_FAILED;
}
int GetDevInfo(long threadID, EEpromV101& data)
{

	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = GetDevInfo_MSG;
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index--)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == GetDevInfo_MSG)
			{
				// EEpromV101 *eepromv101 = new EEpromV101;
				// memset(eepromv101, 0, sizeof(EEpromV101));
				memcpy(&data, msg2.cmd.str, sizeof(EEpromV101));
				INFO_PR("GetDevInfo recv MSG\n");
				return SUCCESS;
			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return GET_DEVINFO_FAILED;
}

int SetDevInfo_extre(long threadID, DevData& data)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = SetDevInfo_MSG;
	memcpy(&msg.cmd.str, &data, sizeof(DevData));
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index--)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == SetDevInfo_MSG)
			{
				memcpy(&data, &msg2.cmd.str, sizeof(DevData));
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				for (int i = 0; i < sizeof(data.result); i++)
				{
					if (data.result[i] == 'O' || data.result[i] == 'N')
					{
						return SUCCESS;
					}

				}
				return SET_DEVINFO_FAILED;
			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return SET_DEVINFO_FAILED;
}
// 启停雷达测距
int ControlDrv(long threadID, const char* data)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = ctrl_MSG;
	strcpy(msg.cmd.str, data);
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index--)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == ctrl_MSG)
			{
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				return SUCCESS;
			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return SET_DEV_FAILED;
}
int GetLidarTimestamp(long threadID, bool dataGet)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = Print_TimeStamp_MSG;
	msg.cmd.str[0] = dataGet;
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index--)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == Print_TimeStamp_MSG)
			{
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				if (msg2.cmd.str[0] - '0')
					return SUCCESS;
				else
					return GET_DEVTIMESTAMP_FAILED;

			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return GET_DEVTIMESTAMP_FAILED;
}

int GetAlarmZone(long threadID, RecvZoneDatas& data)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = Get_ZONE_MSG;
	msg.cmd.str[0] = true;
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);
	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == Get_ZONE_MSG)
			{
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				memcpy(&data,msg2.cmd.str,sizeof(RecvZoneDatas));
				if (data.result == true)
					return SUCCESS;
				else
					return Get_ZONE_FAILED;

			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return Get_ZONE_FAILED;
}
int SetAlarmZone(long threadID, zones& data)
{
	USER_MSG msg;
	msg.type = 1;
	msg.cmd.type2 = Set_ZONE_MSG;
	memcpy(&msg.cmd.str, &data, sizeof(DevData));
	int res = msgsnd(threadID, &msg, sizeof(msg.cmd), 0);

	USER_MSG msg2;
	sleep(0.5);
	int index = 5;
	while (index)
	{
		if (msgrcv(threadID, &msg2, sizeof(msg2.cmd), 2, IPC_NOWAIT) >= 0)
		{
			if (msg2.cmd.type2 == Set_ZONE_MSG)
			{
				int *rev= new int;
				memcpy(rev, &msg2.cmd.str, sizeof(int));
				INFO_PR("%s recv MSG\n", __FUNCTION__);
				if(*rev == true)
					return SUCCESS;
				else
					return Set_ZONE_FAILED;
			}
			else {
				DEBUG_PR("%s msg error!\n", __FUNCTION__);
			}
		}
		sleep(1);
	}
	return Set_ZONE_FAILED;
}

#endif