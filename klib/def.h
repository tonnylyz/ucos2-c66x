//
// Created by Tonny on 3/18/2019.
//

#ifndef UCOS2_C66X_DEF_H
#define UCOS2_C66X_DEF_H

typedef enum {
    sc_normal_start,
    sc_partition_restart,
    sc_hm_module_restart,
    sc_hm_partition_restart,
} start_condition_t;

typedef enum {
    opm_idle,
    opm_normal,
    opm_cold_start,
    opm_warm_start,
} operating_mode_t;

typedef enum {
    r_no_error,
    r_no_action,
    r_not_available,
    r_invalid_param,
    r_invalid_config,
    r_invalid_mode,
    r_timed_out,
} return_code_t;

typedef enum {
    pt_system,
    pt_normal,
} partition_type_t;

#endif //UCOS2_C66X_DEF_H
