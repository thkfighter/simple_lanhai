/*
 * File: TestClientLocalizationMap.cpp
 * Copyright © 2021 Bosch Rexroth AG
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
#include "BinaryInterfaceLocalizationMapStruct.h"
#include "BinaryInterfaceClient.h"


void checkReturnValue(int32_t returnValue)
{
    if (returnValue == 0)
    {
        std::cout << "Socket call __read__ returned zero." << '\n';
    }
}

/*
 * Run Tcp Test -> This accepts a hostname and (optionally) a filename 
 * Connect to the TCP Port 9009 (default port of localization map interface)
 */
void RunTcpTest (const std::string &hostName)
{
    std::cout << "------------------- Client test for unsecured (no-TLS) Binary Interface ---------------------" << '\n';
    try
    {
        // Create the client. It will connect to the server while construction
        // initializing all fields.
        BinaryInterfaceClient client(hostName, 9009);

        LocalizationMapMessage message;
        int32_t rv = 0;
        while (rv >= 0)
        {
            std::cout << "------------------ Localization Map Message --------------------------\n";
            std::cout << "------------------ Read legacy block. Original Data ------------------\n";
            uint32_t mapLength;
            rv = client.read(&mapLength, sizeof(mapLength));
            checkReturnValue(rv);
            message.map.resize(mapLength);
            for (uint32_t i = 0; i < mapLength; ++i)
            {
                rv = client.read(&message.map[i], sizeof(message.map[i]));
                checkReturnValue(rv);
            }

            std::cout << "------------------- Read extension. New Data Fields ------------------\n";
            uint32_t readBytes {0}; //From now on. Track readBytes allows full forward compatability
            uint32_t extensionSize {0};
            rv = client.read(&extensionSize, sizeof(uint32_t));
            checkReturnValue(rv);
            readBytes += rv;

            uint32_t offsetLength {0};
            rv = client.read(&offsetLength, sizeof(uint32_t));
            checkReturnValue(rv);
            readBytes += rv;

            constexpr uint32_t knownFields {1};
            std::vector<uint32_t> fieldOffsets(knownFields); //one known field
            if (knownFields > offsetLength) {
                std::cout << "Critical error: Not enough fields in message. Some content will be uninitialized!";
            }

            for (uint32_t i {0}; i < std::min(knownFields, offsetLength); ++i) {
                rv = client.read(&fieldOffsets[i], sizeof(uint32_t));
                readBytes += rv;
            }

            if (offsetLength > 0) {
                std::cout << "-------------------------- Go to first field -------------------------\n";
                if (readBytes < fieldOffsets[0]) {
                    const uint32_t skipBytes {fieldOffsets[0] - readBytes};
                    std::vector<char> buffer (skipBytes);
                    rv = client.read(buffer.data(), skipBytes);
                    checkReturnValue(rv);
                    readBytes += rv;
                }

                uint32_t nReflectors {0};
                rv = client.read(&nReflectors, sizeof(uint32_t));
                checkReturnValue(rv);
                readBytes += rv;
                message.reflectors.resize(nReflectors);
                std::cout << "reflectors.size(): " << message.reflectors.size() << "\n";

                for (auto& r: message.reflectors) {
                    rv = client.read(&r.x, sizeof(r.x));
                    checkReturnValue(rv);
                    readBytes += rv;
                    rv = client.read(&r.y, sizeof(r.y));
                    checkReturnValue(rv);
                    readBytes += rv;
                    rv = client.read(&r.yaw, sizeof(r.yaw));
                    checkReturnValue(rv);
                    readBytes += rv;
                    rv = client.read(&r.from, sizeof(r.from));
                    checkReturnValue(rv);
                    readBytes += rv;
                    rv = client.read(&r.to, sizeof(r.to));
                    checkReturnValue(rv);
                    readBytes += rv;

                    std::cout << "reflector: " << r.x << ", " << r.y << ", " << r.yaw << ", " << r.from << ", " << r.to << "\n";
                }
            }

            if (readBytes < extensionSize) {
                std::cout << "------------------------- Skip remaining bytes -----------------------\n";
                const uint32_t skipBytes {extensionSize - readBytes};
                std::vector<char> buffer (skipBytes);
                rv = client.read(buffer.data(), skipBytes);
                checkReturnValue(rv);
                readBytes += rv;
            }

            std::cout << std::flush;
        }

        std::cout << "Socket call __read__ returned with signal: Socket closed." << '\n';
    }
    catch(std::exception& ex)
    {
        std::cerr << "Error during TCP Receiving test: " << std::string(ex.what()) << '\n';
    }
}


int main(int argc, char **argv)
{
    // Ignore sigpipe
    signal(SIGPIPE, SIG_IGN);

    /* =================================
     * Command line parsing
     */
    int c;
    bool usage {false};
    std::string hostName{"localhost"};

    while ((c = getopt(argc, argv, "ha:")) >= 0)
    {
        switch (c)
        {
            case 'a':
                if (optarg)
                {
                    hostName = std::string(optarg);
                }
                break;
            case 'h':
                usage = true;
                break;
            default:
                usage = true;
        }
    }

    if (usage)
    {
        std::cout << "testinterface [-a <address>] [-h]" << '\n';
        return -1;
    }
    /* =================================
     */

    /* =================================
     * Run TCP Test 
     */
    try
    {
        RunTcpTest(hostName);
    }
    catch(std::exception& ex)
    {
        std::cerr << "TestClientLocalizationMap died with error!" << '\n';
        std::cerr << "What: " << std::string(ex.what()) << '\n';
    }
    /* =================================
     */
    return 0;
}
