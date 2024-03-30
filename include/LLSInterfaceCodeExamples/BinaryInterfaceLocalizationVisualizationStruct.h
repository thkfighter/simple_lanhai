/*
 * Copyright © 2021 Bosch Rexroth AG
*/

#ifndef BINARYINTERFACELOCALIZATIONVISUALIZATIONSTRUCTS_H
#define BINARYINTERFACELOCALIZATIONVISUALIZATIONSTRUCTS_H

#include <cstdint>
#include <vector>

struct __attribute__ ((packed)) Position2D
{
    float x{0.0f};
    float y{0.0f};
};
struct __attribute__ ((packed)) PointCluster
{
    double x{0.0f};
    double y{0.0f};
    double yaw{0.0f};
    uint32_t from{0};
    uint32_t to{0};
};

/*
 * structure containing the exact message from the ClientLocalizationVisualization Interface
 */
struct LocalizationVisualizationMessage
{
    double timestamp{0.};
    uint64_t uniqueId{0};
    int32_t locState{0};
    double poseX{0.};
    double poseY{0.};
    double poseYaw{0.};
    double delay{0.};
    std::vector<Position2D> scan{};
    std::vector<uint64_t> sensorOffsets{};
    bool hasIntensities{false};
    float minIntensity{0.f};
    float maxIntensity{0.f};
    std::vector<float> intensities{};
    std::vector<PointCluster> reflectors{};
};

#endif /* BINARYINTERFACELOCALIZATIONVISUALIZATIONSTRUCTS_H */
