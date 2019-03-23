//
// Created by Tonny on 3/18/2019.
//

#include "apex.h"
#include "printf.h"
#include <partition.h>

return_code_t apex_set_partition_mode(operating_mode_t ps) {
    if (ps != opm_cold_start && ps != opm_idle && ps != opm_normal && ps != opm_warm_start) {
        return r_invalid_param;
    }
    if (ps == opm_normal && partition_current->operating_mode == opm_normal) {
        return r_no_action;
    }
    if (ps == opm_warm_start && partition_current->operating_mode == opm_cold_start) {
        return r_invalid_mode;
    }
    partition_current->operating_mode = ps;
    if (ps == opm_idle) {
        // shutdown the partition
        partition_current->target_core = 255u;
    } else {
        printf("Warning: setting partition_mode except for idle mode deal no change to the system");
    }
    return r_no_error;
}

return_code_t apex_get_partition_mode(operating_mode_t *pps) {
    *pps = partition_current->operating_mode;
    return r_no_error;
}


