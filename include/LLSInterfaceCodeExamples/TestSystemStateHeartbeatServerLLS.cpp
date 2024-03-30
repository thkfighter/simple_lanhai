/*
 * File: TestSystemStateHeartbeat.cpp
 * Created On: 2021-03-31
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
#include "SystemStateStruct.h"
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

    SystemStateMessage stateMessage;

    try
    {
        while (infile.read(reinterpret_cast<char*>(&stateMessage), sizeof(stateMessage)))
        {
            //Write to file
            if (filename != "")
            {
                try
                {
                    outfile.write(reinterpret_cast<char*>(&stateMessage), sizeof(stateMessage));
                    outfile.flush();
                }
                catch (std::ios_base::failure& ex)
                {
                    throw std::runtime_error("TestSystemStateHeartbeat::RunFileTest: io-error while writing binary outfile: " + std::string(ex.what()));
                }
            }

            std::cout << "state  = " << stateMessage.state << std::endl;
            std::cout << "=====================================" << std::endl;
        }
    }
    catch (std::ios_base::failure& ex)
    {
        throw std::runtime_error("TestSystemStateHeartbeat::RunFileTest: io-error while reading binary infile: " + std::string(ex.what()));
    }
}


/*
 * Run Tcp Test -> This accepts an hostname and (optionally) a filename
 * Connect to the TCP Port 9000 (default port of  Heartbeat Server)
 * if a filename is set,
 *  -> this will be an output file, where the binary content from the socket is written to
 */
void RunTcpTest (const std::string &hostName, const std::string &filename = "")
{
    std::cout << "------------------- Client test for unsecured (no-TLS) SystemState Heartbeat Server ---------------------" << std::endl;
    try
    {
        //Create the client. It will connect to the server while construction
        //initializing all fields.
        BinaryInterfaceClient client(hostName, 9000);

        //If Fileout is requested open file in binary io mode
        std::ofstream outfile;
        if (filename != "")
        {
            outfile.open(filename, std::ios::binary);
        }

        SystemStateMessage stateMessage;
        int32_t rv = 0;
        while (rv >= 0)
        {
            rv = client.read(&stateMessage, sizeof(stateMessage));

            //Write to file
            if (filename != "")
            {
                try
                {
                    outfile.write(reinterpret_cast<char*>(&stateMessage), sizeof(stateMessage));
                    outfile.flush();
                }
                catch (std::ios_base::failure& ex)
                {
                    throw std::runtime_error("TestSystemStateHeartbeat::RunFileTest: io-error while writing binary outfile: " + std::string(ex.what()));
                }
            }

            if (rv > 0)
            {
                std::cout << "state = " << stateMessage.state << std::endl;
                std::cout << "====================================="                << std::endl;
            }
            else if  (rv == 0)
            {
                std::cout << "Socket call __read__ returned zero." << std::endl;
                return;
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
        std::cerr << "TestSystemStateHeartbeat died with error!" << std::endl;
        std::cerr << "What: " << std::string(ex.what()) << std::endl;
    }
    /* =================================
     */


    return 0;
}
