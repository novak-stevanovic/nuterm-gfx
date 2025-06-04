#ifndef __NTG_SHARED_H__
#define __NTG_SHARED_H__

#define _return(ret_val, out_status_param, out_status)                         \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return (ret_val);                                                      \
    } while(0);                                                                \

#define _vreturn(out_status_param, out_status)                                 \
    do                                                                         \
    {                                                                          \
        if((out_status_param) != NULL)                                         \
            (*out_status_param) = (out_status);                                \
        return;                                                                \
    } while(0);                                                                \

#endif // __NTG_SHARED_H__
