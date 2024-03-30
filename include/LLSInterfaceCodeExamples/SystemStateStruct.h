/*
 * File: SystemStateStruct.h
 * Created On: 2021-03-31
 * Copyright © 2021 Bosch Rexroth AG
*/

#ifndef SYSTEMSTATESTRUCT_H
#define SYSTEMSTATESTRUCT_H

struct __attribute__ ((packed)) SystemStateMessage
{
    int32_t state{0};
};

#endif /* SYSTEMSTATESTRUCT_H */
