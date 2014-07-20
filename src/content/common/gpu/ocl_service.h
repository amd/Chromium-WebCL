void
GpuChannel::service_clGetPlatformIDs(
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_platforms, //[O] cl_platform_id * platforms
	cl_uint * msg_num_platforms, //[O] cl_uint * num_platforms
	cl_pointer * func_ret) //! return cl_int
{
  msg_platforms->resize(msg_num_entries * sizeof(cl_platform_id)); //WATCH2
  *func_ret = (cl_pointer)  clGetPlatformIDs(	(cl_uint)msg_num_entries,
	msg_platforms->size() ? (cl_platform_id *)&(*msg_platforms)[0] : NULL,
	(cl_uint *)msg_num_platforms);
}
void
GpuChannel::service_clGetPlatformInfo(
	const cl_pointer& msg_platform, //[I] cl_platform_id platform
	const cl_platform_info& msg_param_name, //[I] cl_platform_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetPlatformInfo(	(cl_platform_id)msg_platform,
	(cl_platform_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clGetDeviceIDs(
	const cl_pointer& msg_platform, //[I] cl_platform_id platform
	const cl_device_type& msg_device_type, //[I] cl_device_type device_type
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_devices, //[O] cl_device_id * devices
	cl_uint * msg_num_devices, //[O] cl_uint * num_devices
	cl_pointer * func_ret) //! return cl_int
{
  msg_devices->resize(msg_num_entries * sizeof(cl_device_id)); //WATCH2
  *func_ret = (cl_pointer)  clGetDeviceIDs(	(cl_platform_id)msg_platform,
	(cl_device_type)msg_device_type,
	(cl_uint)msg_num_entries,
	msg_devices->size() ? (cl_device_id *)&(*msg_devices)[0] : NULL,
	(cl_uint *)msg_num_devices);
}
void
GpuChannel::service_clGetDeviceInfo(
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_device_info& msg_param_name, //[I] cl_device_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetDeviceInfo(	(cl_device_id)msg_device,
	(cl_device_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clReleaseContext(
	const cl_pointer& msg_context, //[I] cl_context context
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseContext(	(cl_context)msg_context);
}
void
GpuChannel::service_clCreateCommandQueue(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_command_queue_properties& msg_properties, //[I] cl_command_queue_properties properties
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_command_queue
{
  *func_ret = (cl_pointer)  clCreateCommandQueue(	(cl_context)msg_context,
	(cl_device_id)msg_device,
	(cl_command_queue_properties)msg_properties,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clReleaseCommandQueue(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseCommandQueue(	(cl_command_queue)msg_command_queue);
}
void
GpuChannel::service_clGetCommandQueueInfo(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_command_queue_info& msg_param_name, //[I] cl_command_queue_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetCommandQueueInfo(	(cl_command_queue)msg_command_queue,
	(cl_command_queue_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clCreateBuffer(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const size_t& msg_size, //[I] size_t size
	const std::vector<unsigned char>& msg_host_ptr, //[I] void * host_ptr
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
{
  *func_ret = (cl_pointer)  clCreateBuffer(	(cl_context)msg_context,
	(cl_mem_flags)msg_flags,
	(size_t)msg_size,
	msg_host_ptr.size() ? (void *)&(msg_host_ptr)[0] : NULL,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clCreateSubBuffer(
	const cl_pointer& msg_buffer, //[I] cl_mem buffer
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const cl_buffer_create_type& msg_buffer_create_type, //[I] cl_buffer_create_type buffer_create_type
	const std::vector<unsigned char>& msg_buffer_create_info, //[I] const void * buffer_create_info
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
{
  *func_ret = (cl_pointer)  clCreateSubBuffer(	(cl_mem)msg_buffer,
	(cl_mem_flags)msg_flags,
	(cl_buffer_create_type)msg_buffer_create_type,
	msg_buffer_create_info.size() ? (const void *)&(msg_buffer_create_info)[0] : NULL,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clCreateImage(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const std::vector<unsigned char>& msg_image_format, //[I] const cl_image_format * image_format
	const std::vector<unsigned char>& msg_image_desc, //[I] const cl_image_desc * image_desc
	const std::vector<unsigned char>& msg_host_ptr, //[I] void * host_ptr
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_mem
{
  *func_ret = (cl_pointer)  clCreateImage(	(cl_context)msg_context,
	(cl_mem_flags)msg_flags,
	msg_image_format.size() ? (const cl_image_format *)&(msg_image_format)[0] : NULL,
	msg_image_desc.size() ? (const cl_image_desc *)&(msg_image_desc)[0] : NULL,
	msg_host_ptr.size() ? (void *)&(msg_host_ptr)[0] : NULL,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clReleaseMemObject(
	const cl_pointer& msg_memobj, //[I] cl_mem memobj
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseMemObject(	(cl_mem)msg_memobj);
}
void
GpuChannel::service_clGetSupportedImageFormats(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_mem_flags& msg_flags, //[I] cl_mem_flags flags
	const cl_mem_object_type& msg_image_type, //[I] cl_mem_object_type image_type
	const cl_uint& msg_num_entries, //[I] cl_uint num_entries
	std::vector<unsigned char> * msg_image_formats, //[O] cl_image_format * image_formats
	cl_uint * msg_num_image_formats, //[O] cl_uint * num_image_formats
	cl_pointer * func_ret) //! return cl_int
{
  msg_image_formats->resize(msg_num_entries * sizeof(cl_image_format)); //WATCH2
  *func_ret = (cl_pointer)  clGetSupportedImageFormats(	(cl_context)msg_context,
	(cl_mem_flags)msg_flags,
	(cl_mem_object_type)msg_image_type,
	(cl_uint)msg_num_entries,
	msg_image_formats->size() ? (cl_image_format *)&(*msg_image_formats)[0] : NULL,
	(cl_uint *)msg_num_image_formats);
}
void
GpuChannel::service_clGetMemObjectInfo(
	const cl_pointer& msg_memobj, //[I] cl_mem memobj
	const cl_mem_info& msg_param_name, //[I] cl_mem_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetMemObjectInfo(	(cl_mem)msg_memobj,
	(cl_mem_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clGetImageInfo(
	const cl_pointer& msg_image, //[I] cl_mem image
	const cl_image_info& msg_param_name, //[I] cl_image_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetImageInfo(	(cl_mem)msg_image,
	(cl_image_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clCreateSampler(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_bool& msg_normalized_coords, //[I] cl_bool normalized_coords
	const cl_addressing_mode& msg_addressing_mode, //[I] cl_addressing_mode addressing_mode
	const cl_filter_mode& msg_filter_mode, //[I] cl_filter_mode filter_mode
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_sampler
{
  *func_ret = (cl_pointer)  clCreateSampler(	(cl_context)msg_context,
	(cl_bool)msg_normalized_coords,
	(cl_addressing_mode)msg_addressing_mode,
	(cl_filter_mode)msg_filter_mode,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clReleaseSampler(
	const cl_pointer& msg_sampler, //[I] cl_sampler sampler
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseSampler(	(cl_sampler)msg_sampler);
}
void
GpuChannel::service_clGetSamplerInfo(
	const cl_pointer& msg_sampler, //[I] cl_sampler sampler
	const cl_sampler_info& msg_param_name, //[I] cl_sampler_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetSamplerInfo(	(cl_sampler)msg_sampler,
	(cl_sampler_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clCreateProgramWithSource(
	const cl_pointer& msg_context, //[I] cl_context context
	const cl_uint& msg_count, //[I] cl_uint count
	const std::vector<unsigned char>& msg_strings, //[I] const char ** strings
	const std::vector<unsigned char>& msg_lengths, //[I] const size_t * lengths
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_program
{
  const unsigned char *msg_str[] = {&msg_strings[0]};
  *func_ret = (cl_pointer)  clCreateProgramWithSource(	(cl_context)msg_context,
	(cl_uint)msg_count,
	msg_strings.size() ? (const char **)&(msg_str)[0] : NULL,
	msg_lengths.size() ? (const size_t *)&(msg_lengths)[0] : NULL,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clReleaseProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseProgram(	(cl_program)msg_program);
}
void
GpuChannel::service_clBuildProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_uint& msg_num_devices, //[I] cl_uint num_devices
	const std::vector<unsigned char>& msg_device_list, //[I] const cl_device_id * device_list
	const std::vector<unsigned char>& msg_options, //[I] const char * options
	const cl_pointer& msg_callback, //[I] callback callback
	const cl_pointer& msg_user_data, //[I] void * user_data
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clBuildProgram(	(cl_program)msg_program,
	(cl_uint)msg_num_devices,
	msg_device_list.size() ? (const cl_device_id *)&(msg_device_list)[0] : NULL,
	msg_options.size() ? (const char *)&(msg_options)[0] : NULL,
	NULL, //callback
	(void *)msg_user_data);
}
void
GpuChannel::service_clGetProgramInfo(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_program_info& msg_param_name, //[I] cl_program_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetProgramInfo(	(cl_program)msg_program,
	(cl_program_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clGetProgramBuildInfo(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_program_build_info& msg_param_name, //[I] cl_program_build_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetProgramBuildInfo(	(cl_program)msg_program,
	(cl_device_id)msg_device,
	(cl_program_build_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clCreateKernel(
	const cl_pointer& msg_program, //[I] cl_program program
	const std::vector<unsigned char>& msg_kernel_name, //[I] const char * kernel_name
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_kernel
{
  *func_ret = (cl_pointer)  clCreateKernel(	(cl_program)msg_program,
	msg_kernel_name.size() ? (const char *)&(msg_kernel_name)[0] : NULL,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clCreateKernelsInProgram(
	const cl_pointer& msg_program, //[I] cl_program program
	const cl_uint& msg_num_kernels, //[I] cl_uint num_kernels
	std::vector<unsigned char> * msg_kernels, //[O] cl_kernel * kernels
	cl_uint * msg_num_kernels_ret, //[O] cl_uint * num_kernels_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_kernels->resize(msg_num_kernels * sizeof(cl_kernel)); //WATCH2
  *func_ret = (cl_pointer)  clCreateKernelsInProgram(	(cl_program)msg_program,
	(cl_uint)msg_num_kernels,
	msg_kernels->size() ? (cl_kernel *)&(*msg_kernels)[0] : NULL,
	(cl_uint *)msg_num_kernels_ret);
}
void
GpuChannel::service_clReleaseKernel(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseKernel(	(cl_kernel)msg_kernel);
}
void
GpuChannel::service_clSetKernelArg(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_uint& msg_arg_index, //[I] cl_uint arg_index
	const size_t& msg_arg_size, //[I] size_t arg_size
	const std::vector<unsigned char>& msg_arg_value, //[I] const void * arg_value
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clSetKernelArg(	(cl_kernel)msg_kernel,
	(cl_uint)msg_arg_index,
	(size_t)msg_arg_size,
	msg_arg_value.size() ? (const void *)&(msg_arg_value)[0] : NULL);
}
void
GpuChannel::service_clGetKernelInfo(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_kernel_info& msg_param_name, //[I] cl_kernel_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetKernelInfo(	(cl_kernel)msg_kernel,
	(cl_kernel_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clGetKernelWorkGroupInfo(
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_pointer& msg_device, //[I] cl_device_id device
	const cl_kernel_work_group_info& msg_param_name, //[I] cl_kernel_work_group_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetKernelWorkGroupInfo(	(cl_kernel)msg_kernel,
	(cl_device_id)msg_device,
	(cl_kernel_work_group_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clWaitForEvents(
	const cl_uint& msg_num_events, //[I] cl_uint num_events
	const std::vector<unsigned char>& msg_event_list, //[I] const cl_event * event_list
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clWaitForEvents(	(cl_uint)msg_num_events,
	msg_event_list.size() ? (const cl_event *)&(msg_event_list)[0] : NULL);
}
void
GpuChannel::service_clGetEventInfo(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_event_info& msg_param_name, //[I] cl_event_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetEventInfo(	(cl_event)msg_event,
	(cl_event_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clCreateUserEvent(
	const cl_pointer& msg_context, //[I] cl_context context
	cl_int * msg_errcode_ret, //[O] cl_int * errcode_ret
	cl_pointer * func_ret) //! return cl_event
{
  *func_ret = (cl_pointer)  clCreateUserEvent(	(cl_context)msg_context,
	(cl_int *)msg_errcode_ret);
}
void
GpuChannel::service_clReleaseEvent(
	const cl_pointer& msg_event, //[I] cl_event event
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clReleaseEvent(	(cl_event)msg_event);
}
void
GpuChannel::service_clSetUserEventStatus(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_int& msg_execution_status, //[I] cl_int execution_status
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clSetUserEventStatus(	(cl_event)msg_event,
	(cl_int)msg_execution_status);
}
void
GpuChannel::service_clSetEventCallback(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_int& msg_command_exec_callback_type, //[I] cl_int command_exec_callback_type
	const cl_pointer& msg_callback, //[I] callback callback
	const cl_pointer& msg_user_data, //[I] void * user_data
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clSetEventCallback(	(cl_event)msg_event,
	NULL, //callback
	NULL, //callback
	(void *)msg_user_data);
}
void
GpuChannel::service_clGetEventProfilingInfo(
	const cl_pointer& msg_event, //[I] cl_event event
	const cl_profiling_info& msg_param_name, //[I] cl_profiling_info param_name
	const size_t& msg_param_value_size, //[I] size_t param_value_size
	std::vector<unsigned char> * msg_param_value, //[O] void * param_value
	size_t * msg_param_value_size_ret, //[O] size_t * param_value_size_ret
	cl_pointer * func_ret) //! return cl_int
{
  msg_param_value->resize(msg_param_value_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clGetEventProfilingInfo(	(cl_event)msg_event,
	(cl_profiling_info)msg_param_name,
	(size_t)msg_param_value_size,
	msg_param_value->size() ? (void *)&(*msg_param_value)[0] : NULL,
	(size_t *)msg_param_value_size_ret);
}
void
GpuChannel::service_clFlush(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clFlush(	(cl_command_queue)msg_command_queue);
}
void
GpuChannel::service_clFinish(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clFinish(	(cl_command_queue)msg_command_queue);
}
void
GpuChannel::service_clEnqueueReadBuffer(
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
{
  msg_ptr->resize(msg_size * sizeof(unsigned char)); //WATCH2
  *func_ret = (cl_pointer)  clEnqueueReadBuffer(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_buffer,
	(cl_bool)msg_blocking_read,
	(size_t)msg_offset,
	(size_t)msg_size,
	msg_ptr->size() ? (void *)&(*msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueReadBufferRect(
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
{
msg_ptr->resize(g_hostPtrSize); //WATCH1
  *func_ret = (cl_pointer)  clEnqueueReadBufferRect(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_buffer,
	(cl_bool)msg_blocking_read,
	msg_buffer_offset.size() ? (const size_t *)&(msg_buffer_offset)[0] : NULL,
	msg_host_offset.size() ? (const size_t *)&(msg_host_offset)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_buffer_row_pitch,
	(size_t)msg_buffer_slice_pitch,
	(size_t)msg_host_row_pitch,
	(size_t)msg_host_slice_pitch,
	msg_ptr->size() ? (void *)&(*msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueWriteBuffer(
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
{
  *func_ret = (cl_pointer)  clEnqueueWriteBuffer(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_buffer,
	(cl_bool)msg_blocking_write,
	(size_t)msg_offset,
	(size_t)msg_size,
	msg_ptr.size() ? (const void *)&(msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueWriteBufferRect(
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
{
  *func_ret = (cl_pointer)  clEnqueueWriteBufferRect(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_buffer,
	(cl_bool)msg_blocking_write,
	msg_buffer_offset.size() ? (const size_t *)&(msg_buffer_offset)[0] : NULL,
	msg_host_offset.size() ? (const size_t *)&(msg_host_offset)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_buffer_row_pitch,
	(size_t)msg_buffer_slice_pitch,
	(size_t)msg_host_row_pitch,
	(size_t)msg_host_slice_pitch,
	msg_ptr.size() ? (const void *)&(msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueCopyBuffer(
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
{
  *func_ret = (cl_pointer)  clEnqueueCopyBuffer(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_src_buffer,
	(cl_mem)msg_dst_buffer,
	(size_t)msg_src_offset,
	(size_t)msg_dst_offset,
	(size_t)msg_size,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueCopyBufferRect(
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
{
  *func_ret = (cl_pointer)  clEnqueueCopyBufferRect(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_src_buffer,
	(cl_mem)msg_dst_buffer,
	msg_src_origin.size() ? (const size_t *)&(msg_src_origin)[0] : NULL,
	msg_dst_origin.size() ? (const size_t *)&(msg_dst_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_src_row_pitch,
	(size_t)msg_src_slice_pitch,
	(size_t)msg_dst_row_pitch,
	(size_t)msg_dst_slice_pitch,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueReadImage(
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
{
msg_ptr->resize(g_hostPtrSize); //WATCH1
  *func_ret = (cl_pointer)  clEnqueueReadImage(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_image,
	(cl_bool)msg_blocking_read,
	msg_origin.size() ? (const size_t *)&(msg_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_row_pitch,
	(size_t)msg_slice_pitch,
	msg_ptr->size() ? (void *)&(*msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueWriteImage(
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
{
  *func_ret = (cl_pointer)  clEnqueueWriteImage(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_image,
	(cl_bool)msg_blocking_write,
	msg_origin.size() ? (const size_t *)&(msg_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_input_row_pitch,
	(size_t)msg_input_slice_pitch,
	msg_ptr.size() ? (const void *)&(msg_ptr)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueCopyImage(
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
{
  *func_ret = (cl_pointer)  clEnqueueCopyImage(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_src_image,
	(cl_mem)msg_dst_image,
	msg_src_origin.size() ? (const size_t *)&(msg_src_origin)[0] : NULL,
	msg_dst_origin.size() ? (const size_t *)&(msg_dst_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueCopyImageToBuffer(
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
{
  *func_ret = (cl_pointer)  clEnqueueCopyImageToBuffer(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_src_image,
	(cl_mem)msg_dst_buffer,
	msg_src_origin.size() ? (const size_t *)&(msg_src_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(size_t)msg_dst_offset,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueCopyBufferToImage(
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
{
  *func_ret = (cl_pointer)  clEnqueueCopyBufferToImage(	(cl_command_queue)msg_command_queue,
	(cl_mem)msg_src_buffer,
	(cl_mem)msg_dst_image,
	(size_t)msg_src_offset,
	msg_dst_origin.size() ? (const size_t *)&(msg_dst_origin)[0] : NULL,
	msg_region.size() ? (const size_t *)&(msg_region)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueNDRangeKernel(
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
{
  *func_ret = (cl_pointer)  clEnqueueNDRangeKernel(	(cl_command_queue)msg_command_queue,
	(cl_kernel)msg_kernel,
	(cl_uint)msg_work_dim,
	msg_global_work_offset.size() ? (const size_t *)&(msg_global_work_offset)[0] : NULL,
	msg_global_work_size.size() ? (const size_t *)&(msg_global_work_size)[0] : NULL,
	msg_local_work_size.size() ? (const size_t *)&(msg_local_work_size)[0] : NULL,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueTask(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_pointer& msg_kernel, //[I] cl_kernel kernel
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueTask(	(cl_command_queue)msg_command_queue,
	(cl_kernel)msg_kernel,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueMarkerWithWaitList(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueMarkerWithWaitList(	(cl_command_queue)msg_command_queue,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueBarrierWithWaitList(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events_in_wait_list, //[I] cl_uint num_events_in_wait_list
	const std::vector<unsigned char>& msg_event_wait_list, //[I] const cl_event * event_wait_list
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueBarrierWithWaitList(	(cl_command_queue)msg_command_queue,
	(cl_uint)msg_num_events_in_wait_list,
	msg_event_wait_list.size() ? (const cl_event *)&(msg_event_wait_list)[0] : NULL,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueMarker(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * msg_event, //[O] cl_event * event
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueMarker(	(cl_command_queue)msg_command_queue,
	(cl_event *)msg_event);
}
void
GpuChannel::service_clEnqueueWaitForEvents(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	const cl_uint& msg_num_events, //[I] cl_uint num_events
	const std::vector<unsigned char>& msg_event_list, //[I] const cl_event * event_list
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueWaitForEvents(	(cl_command_queue)msg_command_queue,
	(cl_uint)msg_num_events,
	msg_event_list.size() ? (const cl_event *)&(msg_event_list)[0] : NULL);
}
void
GpuChannel::service_clEnqueueBarrier(
	const cl_pointer& msg_command_queue, //[I] cl_command_queue command_queue
	cl_pointer * func_ret) //! return cl_int
{
  *func_ret = (cl_pointer)  clEnqueueBarrier(	(cl_command_queue)msg_command_queue);
}
