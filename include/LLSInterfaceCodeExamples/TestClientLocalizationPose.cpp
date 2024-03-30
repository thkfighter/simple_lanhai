/*
 * File: TestClientLocalizationPose.cpp
 * Created On: 2019-03-13
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
#include "BinaryInterfacePoseStruct.h"
#include "BinaryInterfaceClient.h"


/*
 * File test -> read in binary file and prints the content. Binary file must be
 * formatted without padding and like written by TCP test.
 */
void RunFileTest (const std::string &infilename, const std::string &filename = "")
{
    std::cout << "------------------- Filestream validity test for Binary Pose Interface ---------------------" << std::endl;

    std::ifstream infile(infilename, std::ios::binary);
    if (!infile.is_open())
        return;


    //If Fileout is requested open file in binary io mode
    std::ofstream outfile;
    if (filename != "")
    {
        outfile.open(filename, std::ios::binary);
    }

    //Initialize PoseMessage
    PoseMessage poseMessage;

    try
    {
        while (infile.read(reinterpret_cast<char*>(&poseMessage), sizeof(poseMessage)))
        {
            //Write to file
            if (filename != "")
            {
                try
                {
                    outfile.write(reinterpret_cast<char*>(&poseMessage), sizeof(poseMessage)); 
                    outfile.flush();
                }
                catch (std::ios_base::failure& ex)
                {
                    throw std::runtime_error("TestClientLocalizationPose::RunFileTest: io-error while writing binary outfile: " + std::string(ex.what()));
                }
            }

            std::cout << "age               = " << poseMessage.age              << std::endl;
            std::cout << "timestamp         = " << poseMessage.timestamp        << std::endl;
            std::cout << "uniqueId          = " << poseMessage.uniqueId         << std::endl;
            std::cout << "state             = " << poseMessage.state            << std::endl;
            std::cout << "errorFlags        = " << poseMessage.errorFlags       << std::endl;
            std::cout << "infoFlags         = " << poseMessage.infoFlags        << std::endl;
            std::cout << "poseX             = " << poseMessage.poseX            << std::endl;
            std::cout << "poseY             = " << poseMessage.poseY            << std::endl;
            std::cout << "poseYaw           = " << poseMessage.poseYaw          << std::endl;
            std::cout << "covariance_1_1    = " << poseMessage.covariance_1_1   << std::endl;
            std::cout << "covariance_1_2    = " << poseMessage.covariance_1_2   << std::endl;
            std::cout << "covariance_1_3    = " << poseMessage.covariance_1_3   << std::endl;
            std::cout << "covariance_2_2    = " << poseMessage.covariance_2_2   << std::endl;
            std::cout << "covariance_2_3    = " << poseMessage.covariance_2_3   << std::endl;
            std::cout << "covariance_3_3    = " << poseMessage.covariance_3_3   << std::endl;
            std::cout << "poseZ             = " << poseMessage.poseZ            << std::endl;
            std::cout << "quaternion_w      = " << poseMessage.quaternion_w     << std::endl;
            std::cout << "quaternion_x      = " << poseMessage.quaternion_x     << std::endl;
            std::cout << "quaternion_y      = " << poseMessage.quaternion_y     << std::endl;
            std::cout << "quaternion_z      = " << poseMessage.quaternion_z     << std::endl;
            std::cout << "epoch             = " << poseMessage.epoch            << std::endl;
            std::cout << "lidarOdoPoseX     = " << poseMessage.lidarOdoPoseX    << std::endl;
            std::cout << "lidarOdoPoseY     = " << poseMessage.lidarOdoPoseY    << std::endl;
            std::cout << "lidarOdoPoseYaw   = " << poseMessage.lidarOdoPoseYaw  << std::endl;
            std::cout << "====================================="                << std::endl;
        }
    }
    catch (std::ios_base::failure& ex)
    {
        throw std::runtime_error("TestClientLocalizationPose::RunFileTest: io-error while reading binary infile: " + std::string(ex.what()));
    }
}


/*
 * Run Tcp Test -> This accepts an hostname and (optionally) a filename 
 * Connect to the TCP Port 9011 (default port of pose interface)
 * if a filename is set,
 *  -> this will be an output file, where the binary contenct from the socket is written to
 */
void RunTcpTest (const std::string &hostName, const std::string &filename = "")
{
    std::cout << "------------------- Client test for unsecured (no-TLS) Binary Pose Interface ---------------------" << std::endl;
    try
    {
        //Create the client. It will connect to the server while construction
        //initializing all fields.
        BinaryInterfaceClient client(hostName, 9011);

        //If Fileout is requested open file in binary io mode
        std::ofstream outfile;
        if (filename != "")
        {
            outfile.open(filename, std::ios::binary);
        }

        PoseMessage poseMessage;
        int32_t rv = 0;
        while (rv >= 0)
        {
            rv = client.read(&poseMessage, sizeof(poseMessage));

            //Write to file
            if (filename != "")
            {
                try
                {
                    outfile.write(reinterpret_cast<char*>(&poseMessage), sizeof(poseMessage)); 
                    outfile.flush();
                }
                catch (std::ios_base::failure& ex)
                {
                    throw std::runtime_error("TestClientLocalizationPose::RunFileTest: io-error while writing binary outfile: " + std::string(ex.what()));
                }
            }

            if (rv > 0)
            {
                std::cout << "age               = " << poseMessage.age              << std::endl;
                std::cout << "timestamp         = " << poseMessage.timestamp        << std::endl;
                std::cout << "uniqueId          = " << poseMessage.uniqueId         << std::endl;
                std::cout << "state             = " << poseMessage.state            << std::endl;
                std::cout << "errorFlags        = " << poseMessage.errorFlags       << std::endl;
                std::cout << "infoFlags         = " << poseMessage.infoFlags        << std::endl;
                std::cout << "poseX             = " << poseMessage.poseX            << std::endl;
                std::cout << "poseY             = " << poseMessage.poseY            << std::endl;
                std::cout << "poseYaw           = " << poseMessage.poseYaw          << std::endl;
                std::cout << "covariance_1_1    = " << poseMessage.covariance_1_1   << std::endl;
                std::cout << "covariance_1_2    = " << poseMessage.covariance_1_2   << std::endl;
                std::cout << "covariance_1_3    = " << poseMessage.covariance_1_3   << std::endl;
                std::cout << "covariance_2_2    = " << poseMessage.covariance_2_2   << std::endl;
                std::cout << "covariance_2_3    = " << poseMessage.covariance_2_3   << std::endl;
                std::cout << "covariance_3_3    = " << poseMessage.covariance_3_3   << std::endl;
                std::cout << "poseZ             = " << poseMessage.poseZ            << std::endl;
                std::cout << "quaternion_w      = " << poseMessage.quaternion_w     << std::endl;
                std::cout << "quaternion_x      = " << poseMessage.quaternion_x     << std::endl;
                std::cout << "quaternion_y      = " << poseMessage.quaternion_y     << std::endl;
                std::cout << "quaternion_z      = " << poseMessage.quaternion_z     << std::endl;
                std::cout << "epoch             = " << poseMessage.epoch            << std::endl;
                std::cout << "lidarOdoPoseX     = " << poseMessage.lidarOdoPoseX    << std::endl;
                std::cout << "lidarOdoPoseY     = " << poseMessage.lidarOdoPoseY    << std::endl;
                std::cout << "lidarOdoPoseYaw   = " << poseMessage.lidarOdoPoseYaw  << std::endl;
                std::cout << "====================================="                << std::endl;
            }
            else if  (rv == 0)
            {
                std::cout << "Socket call __read__ returned zero." << std::endl;
            }
        }

        std::cout << "Socket call __read__ returned with signal: Socket closed." << std::endl;
    }
    catch(std::exception& ex)
    {
        std::cerr << "Error during TCP Receiving test: " << std::string(ex.what()) << std::endl;
    }
}


int main(int argc, char **argv)
{
    //Ignore sigpipe
    signal(SIGPIPE, SIG_IGN);

    /* =================================
     * Command line parsing
     */
    int c;
    bool usage {false};
    std::string hostName{""};
    std::string infile{""};
    std::string outfile{""};

    while ((c = getopt(argc, argv, "hi:o:a:")) >= 0)
    {
        switch (c)
        {
            case 'a':
                if (optarg)
                {
                    hostName = std::string(optarg);
                }
                break;
            case 'i':
                if (optarg)
                {
                    infile = std::string(optarg);
                }
                break;
            case 'o':
                if (optarg)
                {
                    outfile = std::string(optarg);
                }
                break;
            case 'h':
                usage = true;
                break;
            default:
                usage = true;
        }
    }

    if (infile != "" && hostName != "")
        usage = true;


    if (usage)
    {
        std::cout << "testinterface [-a <address> ]  [-o <outputfile> ] [-i <inputfile> ] [-h ]" << std::endl;
        std::cout << "     -a/-i address and inputfile cannot be used at the same time!" << std::endl;
        return -1;
    }
    /* =================================
     */


    /* =================================
     * Run file or TCP Test 
     */
    try
    {
        if (infile != "")
            RunFileTest(infile, outfile);
        else
        {
            if (hostName == "")
                hostName = "localhost";

            RunTcpTest(hostName, outfile);
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
