#include <vector>
#include <cstdint>
#include <cmath>

/*
 * structure containing the exact message from the ClientSensorLaserDatagram Interface
 */

struct ClientSensorLaserDatagram
// struct __attribute__ ((packed)) ClientSensorLaserDatagram
// warning: ignoring packed attribute because of unpacked non-POD field ‘std::vector<float> LaserMessage::ranges’
{
    uint16_t scanNum{0};        // Overwrite this before send, increase every time.
    double time_start{0.};      // Overwrite this before send, set to current time.
    uint64_t uniqueId{0};       // This feature is not used and therefore no uniqueId must be provided
    double duration_beam{0.};   // Overwrite this before send, set constant
    double duration_scan{0.};   // Overwrite this before send, set constant
    double duration_rotate{0.}; // Overwrite this before send, set constant
    uint32_t numBeams{0};       // Overwrite this before send, set constant
    float angleStart{-M_PI};    // Constant
    float angleEnd{M_PI};       // Constant
    float angleInc{0.};         // Constant
    float minRange{0.};         // Constant: no ranges smaller than 0
    float maxRange{60.};        // Constant: no ranges bigger that 60
    uint32_t rangeArraySize{0}; // Overwrite this before send, set constant
    std::vector<float> ranges;  // Overwrite this before send
    bool hasIntensities{true};  // Constant
    float minIntensity{0.};     // Constant
    float maxIntensity{100.};   // Constant
    uint32_t intensityArraySize{0}; // Overwrite this before send, set constant
    std::vector<float> intensities; // Overwrite this before send
};
