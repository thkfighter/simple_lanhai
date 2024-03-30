/*
 * File: TestClientSensorLaser.cpp
 * Created On: 2019-09-14
 * Copyright © 2019 Bosch Rexroth AG
*/

/*
 * Parser for command line parameters
 */
#include <getopt.h>

/*
 * C++ Standard Library headers
 */
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <exception>
#include <signal.h>


/*
 * MessageStructs and BinaryClient Objects
 */
#include "BinaryInterfaceServer.h"
#include "BinaryInterfaceDummyLaserData.h"


int main(int argc, char **argv)
{
    //Ignore sigpipe
    signal(SIGPIPE, SIG_IGN);

    /* =================================
     * Command line parsing
     */
    int c;
    int port = 0;
    bool usage = false;

    while ((c = getopt(argc, argv, "hp:")) >= 0)
    {
        switch (c)
        {
            case 'p':
                if (optarg)
                {
                    port = std::stoi(optarg);
                }
                break;
            case 'h':
                usage = true;
                break;
            default:
                usage = true;
        }
    }

    if (port <= 0)
        usage = true;

    if (usage)
    {
        std::cout << "testinterface -p <port>  [-h ]" << std::endl;
        return -1;
    }
    /* =================================
     */

    /* =================================
     * Open server and start sending data
     */
    try
    {
        BinaryInterfaceServer server(port);
        BinaryInterfaceDummyLaserData laserData(0.01);

        while (true)
        {
            auto data = laserData.getWaitConstantScan();
            server.write(&data, sizeof(data));
        }
    }
    catch(std::exception& ex)
    {
        std::cerr << "TestClientLocalizationPose died with error!" << std::endl;
        std::cerr << "What: " << std::string(ex.what()) << std::endl;
    }
    /* =================================
     */

    return 0;
}
