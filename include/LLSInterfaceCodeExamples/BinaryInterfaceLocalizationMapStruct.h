/*
 * Copyright © 2021 Bosch Rexroth AG
*/

#ifndef BINARYINTERFACELOCALIZATIONMAPSTRUCTS_H
#define BINARYINTERFACELOCALIZATIONMAPSTRUCTS_H

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
 * structure containing the exact message from the ClientLocalizationMap Interface
 */
struct LocalizationMapMessage
{
    std::vector<Position2D> map{};
    std::vector<PointCluster> reflectors{};
};

#endif /* BINARYINTERFACELOCALIZATIONMAPSTRUCTS_H */
