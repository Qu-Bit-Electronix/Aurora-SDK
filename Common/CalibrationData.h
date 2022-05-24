#pragma once
#include "aurora.h"

namespace aurora
{

/** @brief Address offset where calibration data is stored in the Aurora 
 *  default firmware.
 *  This data is calibrated from the Qu-Bit default Aurora firmware.
 *  It is advised not to save over this data unless you are prepared to recalibrate.
*/
static const uint32_t kCalibrationDataOffset = 4096;

/** @brief Calibration data container for Aurora 
 *  This data is calibrated from the Qu-Bit default Aurora firmware.
 *  It is advised not to save over this data unless you are prepared to recalibrate.
*/
struct CalibrationData
{
    CalibrationData() : warp_scale(60.f), warp_offset(0.f), cv_offset{0.f} {}
    float warp_scale, warp_offset;
    float cv_offset[Hardware::CV_LAST];

    /** @brief checks sameness */
    bool operator==(const CalibrationData& rhs)
    {
        if(warp_scale != rhs.warp_scale)
        {
            return false;
        }
        else if(warp_offset != rhs.warp_offset)
        {
            return false;
        }
        else
        {
            for(int i = 0; i < Hardware::CV_LAST; i++)
            {
                if(cv_offset[i] != rhs.cv_offset[i])
                    return false;
            }
        }
        return true;
    }

    /** @brief Not equal operator */
    bool operator!=(const CalibrationData& rhs) { return !operator==(rhs); }
};

} // namespace aurora
