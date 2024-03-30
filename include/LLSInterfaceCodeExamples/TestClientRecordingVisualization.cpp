/*
 * File: TestClientRecordingVisualization.cpp
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
#include "BinaryInterfaceRecordingVisualizationStruct.h"
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
 * Connect to the TCP Port 9008 (default port of recording visualization interface)
 */
void RunTcpTest (const std::string &hostName)
{
    std::cout << "------------------- Client test for unsecured (no-TLS) Binary Interface ---------------------" << '\n';
    try
    {
        // Create the client. It will connect to the server while construction
        // initializing all fields.
        BinaryInterfaceClient client(hostName, 9008);

        RecordingVisualizationMessage message;
        int32_t rv = 0;
        while (rv >= 0)
        {
            std::cout << "------------------ Recording Visualization Message -------------------\n";
            std::cout << "------------------ Read legacy block. Original Data ------------------\n";
            rv = client.read(&message.timestamp, sizeof(message.timestamp));
            checkReturnValue(rv);
            rv = client.read(&message.visualizationId, sizeof(message.visualizationId));
            checkReturnValue(rv);
            rv = client.read(&message.status, sizeof(message.status));
            checkReturnValue(rv);
            rv = client.read(&message.poseX, sizeof(message.poseX));
            checkReturnValue(rv);
            rv = client.read(&message.poseY, sizeof(message.poseY));
            checkReturnValue(rv);
            rv = client.read(&message.poseYaw, sizeof(message.poseYaw));
            checkReturnValue(rv);
            rv = client.read(&message.distanceToLastLC, sizeof(message.distanceToLastLC));
            checkReturnValue(rv);
            rv = client.read(&message.delay, sizeof(message.delay));
            checkReturnValue(rv);
            rv = client.read(&message.progress, sizeof(message.progress));
            checkReturnValue(rv);

            uint32_t scanLength;
            rv = client.read(&scanLength, sizeof(scanLength));
            checkReturnValue(rv);
            message.scan.resize(scanLength);
            for (uint32_t i = 0; i < scanLength; ++i)
            {
                rv = client.read(&message.scan[i], sizeof(message.scan[i]));
                checkReturnValue(rv);
            }

            uint32_t pathPosesLength;
            rv = client.read(&pathPosesLength, sizeof(pathPosesLength));
            checkReturnValue(rv);
            message.pathPoses.resize(pathPosesLength);
            for (uint32_t i = 0; i < pathPosesLength; ++i)
            {
                rv = client.read(&message.pathPoses[i], sizeof(message.pathPoses[i]));
                checkReturnValue(rv);
            }

            uint32_t pathTypesLength;
            rv = client.read(&pathTypesLength, sizeof(pathTypesLength));
            checkReturnValue(rv);
            message.pathTypes.resize(pathTypesLength);
            for (uint32_t i = 0; i < pathTypesLength; ++i)
            {
                rv = client.read(&message.pathTypes[i], sizeof(message.pathTypes[i]));
                checkReturnValue(rv);
            }

            uint32_t sensorOffsetsLength;
            rv = client.read(&sensorOffsetsLength, sizeof(sensorOffsetsLength));
            checkReturnValue(rv);
            message.sensorOffsets.resize(sensorOffsetsLength);
            for (uint32_t i = 0; i < sensorOffsetsLength; ++i)
            {
                rv = client.read(&message.sensorOffsets[i], sizeof(message.sensorOffsets[i]));
                checkReturnValue(rv);
            }

            rv = client.read(&message.hasIntensities, sizeof(message.hasIntensities));
            checkReturnValue(rv);
            rv = client.read(&message.minIntensity, sizeof(message.minIntensity));
            checkReturnValue(rv);
            rv = client.read(&message.maxIntensity, sizeof(message.maxIntensity));
            checkReturnValue(rv);

            uint32_t intensitiesLength;
            rv = client.read(&intensitiesLength, sizeof(intensitiesLength));
            checkReturnValue(rv);
            message.intensities.resize(intensitiesLength);
            for (uint32_t i = 0; i < intensitiesLength; ++i)
            {
                rv = client.read(&message.intensities[i], sizeof(message.intensities[i]));
                checkReturnValue(rv);
            }

            std::cout << "timestamp = " << message.timestamp << '\n';
            std::cout << "visualizationId = " << message.visualizationId << '\n';
            std::cout << "status = " << message.status << '\n';
            std::cout << "pose = (" << message.poseX << ',' << message.poseY << ',' << message.poseYaw << ")\n";
            std::cout << "distanceToLastLC = " << message.distanceToLastLC << '\n';
            std::cout << "delay = " << message.delay << '\n';
            std::cout << "progress = " << message.progress << '\n';
            std::cout << "path poses length = " << pathPosesLength << '\n';
            std::cout << "path poses = [";
            for (const auto pose : message.pathPoses)
            {
                std::cout << "(" << pose.x << "," << pose.y << "," << pose.yaw << "),";
            }
            std::cout << "]\n";
            std::cout << "path types length = " << pathTypesLength << '\n';
            std::cout << "path types = [";
            for (const auto type : message.pathTypes)
            {
                std::cout << type << ",";
            }
            std::cout << "]\n";
            
            std::cout << "sensors = " << sensorOffsetsLength << '\n';
            std::cout << "scan total length = " << scanLength << '\n';
            std::cout << "intensities total length = " << intensitiesLength << '\n';

            std::cout << "has intensities = " << (message.hasIntensities?"true":"false") << '\n';
            std::cout << "minIntensity = " << message.minIntensity << '\n';
            std::cout << "maxIntensity = " << message.maxIntensity << '\n';

            std::cout << "sensor offsets = [";
            for (const auto offset : message.sensorOffsets)
            {
                std::cout << offset  << ",";
            }
            std::cout << "]\n";

            message.sensorOffsets.push_back(scanLength);
            for (uint32_t i = 0; i < message.sensorOffsets.size() - 1; ++i)
            {
                const uint64_t start{message.sensorOffsets[i]};
                const uint64_t end{message.sensorOffsets[i + 1] - 1};
                std::vector<Position2D> currentScanPoints{
                    message.scan.begin() + start, 
                    message.scan.begin() + end};
                std::cout << "Sensor Nr. " << i << ", start index = " << start << ", end index = " << end << '\n';
                std::cout << "scan points = [";
                for (const auto position : currentScanPoints)
                {
                    std::cout << "(" << position.x << "," << position.y << "),";
                }
                std::cout << "]\n";

                if (message.hasIntensities)
                {
                    std::vector<float> currentIntensities{
                        message.intensities.begin() + start, 
                        message.intensities.begin() + end};
                    std::cout << "intensities = [";
                    for (const auto intensity : currentIntensities)
                    {
                        std::cout << intensity << ",";
                    }
                    std::cout << "]\n";
                }
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
        std::cerr << "TestClientRecordingVisualization died with error!" << '\n';
        std::cerr << "What: " << std::string(ex.what()) << '\n';
    }
    /* =================================
     */
    return 0;
}
