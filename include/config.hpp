#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(config,
    CONFIG_VALUE(rotfix, bool, "Z-Rot Fix", false);
    CONFIG_VALUE(disableBaseAdjustment, bool, "Disable Base Adjustment", false);
    CONFIG_VALUE(adjustmentOrder, bool, "Custom Adjustment Order", false);
    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(rotfix);
        CONFIG_INIT_VALUE(disableBaseAdjustment);
        CONFIG_INIT_VALUE(adjustmentOrder);
    )
);