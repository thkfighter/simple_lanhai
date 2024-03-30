/*
 * File: BinaryInterfaceDummyLaserData.h
 * Created On: 2019-09-14
 * Copyright © 2019 Bosch Rexroth AG
*/

#ifndef BINARYINTERFACEDUMMYLASERSCAN_H
#define BINARYINTERFACEDUMMYLASERSCAN_H 

#include <chrono>
#include <thread>
#include <math.h>

/*
 * Constants of this DummyLaserData provider
 */
constexpr size_t nBeams = 200;
constexpr double range = 4.;
constexpr double intensity = 100.;


/*
 * Dummy laser data -> class allows to get dummy (constant range) but valid
 * data as expected by the laser interface.
 *
 * Defines the ClientSensorLaserDatagram for the special case of constant number of beams.
 */
class BinaryInterfaceDummyLaserData
{
public:
    /*
     * structure containing the exact message from the ClientLocalizationPose Interface
     */
    struct __attribute__ ((packed)) ClientSensorLaserDatagram
    {
        uint16_t scanNum {0};                     //Overwrite this before send, increase every time.
        double   time_start{0.};                  //Overwrite this before send, set to current time.
        uint64_t uniqueId {0};                    //Overwrite this before send, set to arbitrary value.
        double   duration_beam{0.};               //Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
        double   duration_scan{0.};               //Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
        double   duration_rotate{0.};             //Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
        uint32_t numBeams {nBeams};               //Constant
        float    angleStart {-M_PI*0.5};          //Constant: laser opening angle is: -pi/2 to pi/2
        float    angleEnd {M_PI*0.5};             //Constant: laser opening angle is: -pi/2 to pi/2
        float    angleInc {M_PI / (nBeams-1)};    //Constant: laser opening angle is: -pi/2 to pi/2
        float    minRange {0.};                   //Constant: no ranges smaller than 0
        float    maxRange {60.};                  //Constant: no ranges bigger that 60
        uint32_t rangeArraySize {nBeams};         //Constant
        float    ranges[nBeams];                  //Overwrite this before send, set constant to range
        bool     hasIntensities {true};           //Constant: true
        float    minIntensity {0.};               //Constant: 0
        float    maxIntensity {600.};             //Constant: 600
        uint32_t intensitiesArraySize {nBeams};   //Constant
        float    intensities[nBeams];             //Overwrite this before send
    };

    /*
     * Constructor of DummyLaserData -> set rotation duration time (time between two scans)
     */
    BinaryInterfaceDummyLaserData(const double rotationDurationInSec)
        : rotationDurationInSec(rotationDurationInSec),  //One full rotation
          scanDurationInSec(rotationDurationInSec / 2.), //Part of the rotation scanning (180/360)
          beamDurationInSec(scanDurationInSec/nBeams),   //One beam time
          rotationDurationChrono((int)(rotationDurationInSec * 1000. * 1000.)), //wait constant
          lastScan(std::chrono::system_clock::now()){}

    /*
     * Sleep until the next scan should be send and return the appropriate (constant) scan
     */
    ClientSensorLaserDatagram getWaitConstantScan()
    {
        std::this_thread::sleep_until(lastScan + rotationDurationChrono);
        lastScan = std::chrono::system_clock::now();

        ClientSensorLaserDatagram result;
        result.scanNum = ++currentScanNum;
        result.uniqueId = uniqueId; uniqueId += 10;

        const std::chrono::duration<double> unixTimePoint = lastScan - std::chrono::system_clock::time_point ();
        result.time_start = unixTimePoint.count();

        result.duration_beam = beamDurationInSec;
        result.duration_scan = scanDurationInSec;
        result.duration_rotate = rotationDurationInSec;

        for (size_t i = 0; i < nBeams; ++i)
        {
            // Here you need to insert the actual readings from your sensor.
            result.ranges[i] = range;

            // Provide intensities if available.
            // If they are not available, set: result.hasIntensities = false;
            result.intensities[i] = intensity;
        }

        return result;
    }

private:
    double rotationDurationInSec;
    double scanDurationInSec;
    double beamDurationInSec;
    std::chrono::microseconds rotationDurationChrono;
    std::chrono::system_clock::time_point lastScan;
    uint16_t currentScanNum {0};
    uint64_t uniqueId {100};
};

#endif /* BINARYINTERFACEDUMMYLASERSCAN */
