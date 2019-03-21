//
// Created by Tonny on 3/18/2019.
//

#include "apex.h"
#include <partition.h>

return_code_e apex_set_partition_mode(operating_mode_e ps) {
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
    } else if (ps == opm_warm_start || ps == opm_cold_start) {
        // inhibit process scheduling and switch back to initialization mode
    } else /*if (ps == opm_normal)*/ {
        /*set first release points of all previously started periodic processes
        to their next partition period;
        set first release points of all previously delay started periodic processes
        to their next partition period, including their delay times;
        calculate the DEADLINE_TIME of all non-dormant processes in the partition;
        activate the process scheduling;*/
    }
    return r_no_error;
}

return_code_e apex_get_partition_mode(operating_mode_e *pps) {
    *pps = partition_current->operating_mode;
    return r_no_error;
}


