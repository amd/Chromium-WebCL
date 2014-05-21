// PPA_REGISTER_CPU_EVENT(x)
/*
*  @brief register a CPU event
*  @param x CPU register events
*/


// PPA_REGISTER_CPU_EVENT2GROUP(x,y)
/*
*  @brief register CPU events with group
*  @param x CPU register events
*  @param y group name if y is "NoGrp" it means there is no group
*/

#define USE_PPA 0

PPA_REGISTER_CPU_EVENT(vp9_cpy_mi_time)
PPA_REGISTER_CPU_EVENT(INTER_CPU_PART)
PPA_REGISTER_CPU_EVENT(INTER_TIME_OCL)
PPA_REGISTER_CPU_EVENT(INTER_TIME_CPU)
PPA_REGISTER_CPU_EVENT(para_prepare_time)
PPA_REGISTER_CPU_EVENT(inter_pred_time)
PPA_REGISTER_CPU_EVENT(inter_pred_calcu_ocl_time)
PPA_REGISTER_CPU_EVENT(data_cpy_to_cpu_time)
PPA_REGISTER_CPU_EVENT(para_copy_set_time)
PPA_REGISTER_CPU_EVENT(para_set_time)
PPA_REGISTER_CPU_EVENT(inter_pred_calcu_gpu_all_frame)
PPA_REGISTER_CPU_EVENT(inter_pred_calcu_cpu_tile)
PPA_REGISTER_CPU_EVENT(inter_pred_cpu)
PPA_REGISTER_CPU_EVENT(entropy_decode_time)
PPA_REGISTER_CPU_EVENT(inter_idct_time)
PPA_REGISTER_CPU_EVENT(intra_pred_time)
PPA_REGISTER_CPU_EVENT(loop_filter_wpp_time)
PPA_REGISTER_CPU_EVENT(vp9_output_file_time)
PPA_REGISTER_CPU_EVENT(inter_param_write_time)
PPA_REGISTER_CPU_EVENT(update_gpu_buffer_pool)
PPA_REGISTER_CPU_EVENT(all_of_frame_time)
PPA_REGISTER_CPU_EVENT(decode_fps_calcu_time)
PPA_REGISTER_CPU_EVENT(inter_pred_index_time)
