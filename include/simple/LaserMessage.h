#include <vector>
#include <cstdint>
// #include <chrono>
// #include <math.h>
// #include <boost/multiprecision/cpp_int.hpp>
// constexpr size_t nBeams = 400;

/*
 * structure containing the exact message from the ClientLocalizationPose Interface
 */

// struct __attribute__ ((packed)) LaserMessage
// warning: ignoring packed attribute because of unpacked non-POD field ‘std::vector<float> LaserMessage::ranges’
struct LaserMessage
{
    // LaserMessage(const size_t& n_beams):
    //    nBeams(n_beams) {} //initialize nBeams member

    //  size_t nBeams;

    uint16_t scanNum{0};         // Overwrite this before send, increase every time.
    double time_start{0.};      // Overwrite this before send, set to current time.
    uint64_t uniqueId{0};       // This feature is not used and therefore no uniqueId must be provided
    double duration_beam{0.};   // Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
    double duration_scan{0.};   // Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
    double duration_rotate{0.}; // Overwrite this before send, set constant (see constructor of BinaryInterfaceDummyLaserData)
    uint32_t numBeams{0};       // Constant
    float angleStart{0};        // Constant: initiate by laser opening angle is: -pi/2 to pi/2
    float angleEnd{0};          // Constant: initiate by laser opening angle is: -pi/2 to pi/2
    float angleInc{0};          // Constant: initiate by laser opening angle is: -pi/2 to pi/2
    float minRange{0.};         // Constant: no ranges smaller than 0
    float maxRange{60.};        // Constant: no ranges bigger that 60
    uint32_t rangeArraySize{0}; // Constant
    std::vector<float> ranges;  // Overwrite this before send
    bool hasIntensities{false}; // Constant: false
    float minIntensity{0.};   // Constant: 0
    float maxIntensity{1000.};   // Constant: 0
    uint32_t intensityArraySize{0};
    std::vector<float> intensities; //
};
