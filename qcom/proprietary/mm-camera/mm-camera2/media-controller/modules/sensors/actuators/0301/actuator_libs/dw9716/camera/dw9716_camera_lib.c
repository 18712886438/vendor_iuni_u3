/*============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

============================================================================*/

#include "af_tuning.h"
#include "actuator.h"

static actuator_ctrl_t dw9716_camera_lib_ptr = {
#include "dw9716_camera_lib.h"
};

void *dw9716_camera_open_lib(void)
{
  return &dw9716_camera_lib_ptr;
}
