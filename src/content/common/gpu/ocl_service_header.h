#define cl_pointer uint32

void
service_clGetPlatformIDs(
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_platforms, //[O] cl_platform_id * platforms
	cl_uint * msg_num_platforms, //[O] cl_uint * num_platforms
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetPlatformInfo(
	const cl_pointer& msg_platform, //[I] cl_platform_id platform
	const cl_platform_info& msg_param_name, //[I] cl_platform_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetDeviceIDs(
	const cl_pointer& msg_platform, //[I] cl_platform_id platform
	const cl_device_type& msg_device_type, //[I] cl_device_type device_type
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_devices, //[O] cl_device_id * devices
	cl_uint * msg_num_devices, //[O] cl_uint * num_devices
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetDeviceInfo(
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_device_info& msg_param_name, //[I] cl_device_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clReleaseContext(
	const cl_pointer& msg_context, //[I] cl_context context
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateCommandQueue(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_command_queue_properties& msg_properties, //[I] cl_command_queue_properties properties
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_command_queue
;
void
service_clReleaseCommandQueue(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetCommandQueueInfo(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_command_queue_info& msg_param_name, //[I] cl_command_queue_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateBuffer(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const size_t& msg_size, //[I] size_t size
	const std::vector<unsigned char>& msg_host_ptr, //[I] void * host_ptr
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
;
void
service_clCreateSubBuffer(
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const cl_buffer_create_type& msg_buffer_create_type, //[I] cl_buffer_create_type buffer_create_type
	const std::vector<unsigned char>& msg_buffer_create_info, //[I] const void * buffer_create_info
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
;
void
service_clCreateImage(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const std::vector<unsigned char>& msg_image_format, //[I] const cl_image_format * image_format
	const std::vector<unsigned char>& msg_image_desc, //[I] const cl_image_desc * image_desc
	const std::vector<unsigned char>& msg_host_ptr, //[I] void * host_ptr
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
;
void
service_clReleaseMemObject(
	const cl_pointer& msg_memobj, //[I] cl_mem memobj
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetSupportedImageFormats(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const cl_mem_object_type& msg_image_type, //[I] cl_mem_object_type image_type
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_image_formats, //[O] cl_image_format * image_formats
	cl_uint * msg_num_image_formats, //[O] cl_uint * num_image_formats
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetMemObjectInfo(
	const cl_pointer& msg_memobj, //[I] cl_mem memobj
	const cl_mem_info& msg_param_name, //[I] cl_mem_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetImageInfo(
	const cl_pointer& msg_image, //[I] cl_mem image
	const cl_image_info& msg_param_name, //[I] cl_image_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateSampler(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_bool& msg_normalized_coords, //[I] cl_bool normalized_coords
	const cl_addressing_mode& msg_addressing_mode, //[I] cl_addressing_mode addressing_mode
	const cl_filter_mode& msg_filter_mode, //[I] cl_filter_mode filter_mode
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_sampler
;
void
service_clReleaseSampler(
	const cl_pointer& msg_sampler, //[I] cl_sampler sampler
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetSamplerInfo(
	const cl_pointer& msg_sampler, //[I] cl_sampler sampler
	const cl_sampler_info& msg_param_name, //[I] cl_sampler_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateProgramWithSource(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_uint& msg_count, //[I] cl_uint count
	const std::vector<unsigned char>& msg_strings, //[I] const char ** strings
	const std::vector<unsigned char>& msg_lengths, //[I] const size_t * lengths
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_program
;
void
service_clReleaseProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	cl_pointer * func_ret) //! return cl_int
;
void
service_clBuildProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_uint& msg_num_devices, //[I] cl_uint num_devices
	const std::vector<unsigned char>& msg_device_list, //[I] const cl_device_id * device_list
	const std::vector<unsigned char>& msg_options, //[I] const char * options
	const cl_pointer& msg_callback, //[I] callback callback
	const cl_pointer& msg_user_data, //[I] void * user_data
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetProgramInfo(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_program_info& msg_param_name, //[I] cl_program_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetProgramBuildInfo(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_program_build_info& msg_param_name, //[I] cl_program_build_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateKernel(
	const cl_pointer& msg_program, //[I] cl_program program
	const std::vector<unsigned char>& msg_kernel_name, //[I] const char * kernel_name
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_kernel
;
void
service_clCreateKernelsInProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_uint& msg_num_kernels, //[I] cl_uint num_kernels
	std::vector<unsigned char> * msg_kernels, //[O] cl_kernel * kernels
	cl_uint * msg_num_kernels_ret, //[O] cl_uint * num_kernels_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clReleaseKernel(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	cl_pointer * func_ret) //! return cl_int
;
void
service_clSetKernelArg(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_uint& msg_arg_index, //[I] cl_uint arg_index
	const size_t& msg_arg_size, //[I] size_t arg_size
	const std::vector<unsigned char>& msg_arg_value, //[I] const void * arg_value
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetKernelInfo(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_kernel_info& msg_param_name, //[I] cl_kernel_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetKernelWorkGroupInfo(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_kernel_work_group_info& msg_param_name, //[I] cl_kernel_work_group_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clWaitForEvents(
	const cl_uint& msg_num_events, //[I] cl_uint num_events
	const std::vector<unsigned char>& msg_event_list, //[I] const cl_event * event_list
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetEventInfo(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_event_info& msg_param_name, //[I] cl_event_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clCreateUserEvent(
	const cl_pointer& msg_context, //[I] cl_context context
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_event
;
void
service_clReleaseEvent(
	const cl_pointer& msg_event, //[I] cl_event event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clSetUserEventStatus(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_int& msg_execution_status, //[I] cl_int execution_status
	cl_pointer * func_ret) //! return cl_int
;
void
service_clSetEventCallback(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_int& msg_command_exec_callback_type, //[I] cl_int command_exec_callback_type
	const cl_pointer& msg_callback, //[I] callback callback
	const cl_pointer& msg_user_data, //[I] void * user_data
	cl_pointer * func_ret) //! return cl_int
;
void
service_clGetEventProfilingInfo(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_profiling_info& msg_param_name, //[I] cl_profiling_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
;
void
service_clFlush(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
;
void
service_clFinish(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueReadBuffer(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_bool& msg_blocking_read, //[I] cl_bool blocking_read
	const size_t& msg_offset, //[I] size_t offset
	const size_t& msg_size, //[I] size_t size
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	std::vector<unsigned char> * msg_ptr, //[O] void * ptr
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueReadBufferRect(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_bool& msg_blocking_read, //[I] cl_bool blocking_read
	const std::vector<unsigned char>& msg_buffer_offset, //[I] const size_t * buffer_offset
	const std::vector<unsigned char>& msg_host_offset, //[I] const size_t * host_offset
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_buffer_row_pitch, //[I] size_t buffer_row_pitch
	const size_t& msg_buffer_slice_pitch, //[I] size_t buffer_slice_pitch
	const size_t& msg_host_row_pitch, //[I] size_t host_row_pitch
	const size_t& msg_host_slice_pitch, //[I] size_t host_slice_pitch
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	std::vector<unsigned char> * msg_ptr, //[O] void * ptr
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueWriteBuffer(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_bool& msg_blocking_write, //[I] cl_bool blocking_write
	const size_t& msg_offset, //[I] size_t offset
	const size_t& msg_size, //[I] size_t size
	const std::vector<unsigned char>& msg_ptr, //[I] const void * ptr
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueWriteBufferRect(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_bool& msg_blocking_write, //[I] cl_bool blocking_write
	const std::vector<unsigned char>& msg_buffer_offset, //[I] const size_t * buffer_offset
	const std::vector<unsigned char>& msg_host_offset, //[I] const size_t * host_offset
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_buffer_row_pitch, //[I] size_t buffer_row_pitch
	const size_t& msg_buffer_slice_pitch, //[I] size_t buffer_slice_pitch
	const size_t& msg_host_row_pitch, //[I] size_t host_row_pitch
	const size_t& msg_host_slice_pitch, //[I] size_t host_slice_pitch
	const std::vector<unsigned char>& msg_ptr, //[I] const void * ptr
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueCopyBuffer(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_src_buffer, //[I] cl_mem src_buffer
	const cl_pointer& msg_dst_buffer, //[I] cl_mem dst_buffer
	const size_t& msg_src_offset, //[I] size_t src_offset
	const size_t& msg_dst_offset, //[I] size_t dst_offset
	const size_t& msg_size, //[I] size_t size
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueCopyBufferRect(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_src_buffer, //[I] cl_mem src_buffer
	const cl_pointer& msg_dst_buffer, //[I] cl_mem dst_buffer
	const std::vector<unsigned char>& msg_src_origin, //[I] const size_t * src_origin
	const std::vector<unsigned char>& msg_dst_origin, //[I] const size_t * dst_origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_src_row_pitch, //[I] size_t src_row_pitch
	const size_t& msg_src_slice_pitch, //[I] size_t src_slice_pitch
	const size_t& msg_dst_row_pitch, //[I] size_t dst_row_pitch
	const size_t& msg_dst_slice_pitch, //[I] size_t dst_slice_pitch
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueReadImage(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_image, //[I] cl_mem image
	const cl_bool& msg_blocking_read, //[I] cl_bool blocking_read
	const std::vector<unsigned char>& msg_origin, //[I] const size_t * origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_row_pitch, //[I] size_t row_pitch
	const size_t& msg_slice_pitch, //[I] size_t slice_pitch
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	std::vector<unsigned char> * msg_ptr, //[O] void * ptr
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueWriteImage(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_image, //[I] cl_mem image
	const cl_bool& msg_blocking_write, //[I] cl_bool blocking_write
	const std::vector<unsigned char>& msg_origin, //[I] const size_t * origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_input_row_pitch, //[I] size_t input_row_pitch
	const size_t& msg_input_slice_pitch, //[I] size_t input_slice_pitch
	const std::vector<unsigned char>& msg_ptr, //[I] const void * ptr
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueCopyImage(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_src_image, //[I] cl_mem src_image
	const cl_pointer& msg_dst_image, //[I] cl_mem dst_image
	const std::vector<unsigned char>& msg_src_origin, //[I] const size_t * src_origin
	const std::vector<unsigned char>& msg_dst_origin, //[I] const size_t * dst_origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueCopyImageToBuffer(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_src_image, //[I] cl_mem src_image
	const cl_pointer& msg_dst_buffer, //[I] cl_mem dst_buffer
	const std::vector<unsigned char>& msg_src_origin, //[I] const size_t * src_origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const size_t& msg_dst_offset, //[I] size_t dst_offset
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueCopyBufferToImage(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_src_buffer, //[I] cl_mem src_buffer
	const cl_pointer& msg_dst_image, //[I] cl_mem dst_image
	const size_t& msg_src_offset, //[I] size_t src_offset
	const std::vector<unsigned char>& msg_dst_origin, //[I] const size_t * dst_origin
	const std::vector<unsigned char>& msg_region, //[I] const size_t * region
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueNDRangeKernel(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_uint& msg_work_dim, //[I] cl_uint work_dim
	const std::vector<unsigned char>& msg_global_work_offset, //[I] const size_t * global_work_offset
	const std::vector<unsigned char>& msg_global_work_size, //[I] const size_t * global_work_size
	const std::vector<unsigned char>& msg_local_work_size, //[I] const size_t * local_work_size
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueTask(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueMarkerWithWaitList(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueBarrierWithWaitList(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueMarker(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueWaitForEvents(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events, //[I] cl_uint num_events
	const std::vector<unsigned char>& msg_event_list, //[I] const cl_event * event_list
	cl_pointer * func_ret) //! return cl_int
;
void
service_clEnqueueBarrier(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
;
