#include "content/common/gpu/client/gpu_channel_host.h"
#include "content/common/gpu/ocl_msg.h"
extern content::GpuChannelHost *__ocl_gpu_channel_host;

cl_int
client_clGetPlatformIDs(
    cl_uint num_entries,
    cl_platform_id * platforms,
    cl_uint * num_platforms) {
  cl_uint msg_num_entries;
  std::vector<unsigned char> msg_platforms;
  cl_uint msg_num_platforms;
  msg_num_entries = num_entries;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetPlatformIDs(
		msg_num_entries,
		&msg_platforms,
		&msg_num_platforms,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (platforms)  memcpy(platforms, &msg_platforms[0], msg_platforms.size());
  if (num_platforms) *num_platforms = *(cl_uint *)&msg_num_platforms;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetPlatformInfo(
    cl_platform_id platform,
    cl_platform_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_platform;
  cl_platform_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_platform = (cl_pointer)platform;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetPlatformInfo(
		msg_platform,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetDeviceIDs(
    cl_platform_id platform,
    cl_device_type device_type,
    cl_uint num_entries,
    cl_device_id * devices,
    cl_uint * num_devices) {
  cl_pointer msg_platform;
  cl_device_type msg_device_type;
  cl_uint msg_num_entries;
  std::vector<unsigned char> msg_devices;
  cl_uint msg_num_devices;
  msg_platform = (cl_pointer)platform;
  msg_device_type = device_type;
  msg_num_entries = num_entries;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetDeviceIDs(
		msg_platform,
		msg_device_type,
		msg_num_entries,
		&msg_devices,
		&msg_num_devices,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (devices)  memcpy(devices, &msg_devices[0], msg_devices.size());
  if (num_devices) *num_devices = *(cl_uint *)&msg_num_devices;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetDeviceInfo(
    cl_device_id device,
    cl_device_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_device;
  cl_device_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_device = (cl_pointer)device;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetDeviceInfo(
		msg_device,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_context
client_clCreateContext(
    const cl_context_properties * properties,
    cl_uint num_devices,
    const cl_device_id * devices,
    const void* callback,
    void * user_data,
    cl_int * errcode_ret) {
  std::vector<unsigned char> msg_properties;
  cl_uint msg_num_devices;
  std::vector<unsigned char> msg_devices;
  cl_pointer msg_callback;
  cl_pointer msg_user_data;
  cl_int msg_errcode_ret;
{const cl_context_properties *tmp = properties; int n=0; if (tmp) { while (*tmp != 0) n++; }
  if (n) { msg_properties.resize(n*4); memcpy(&msg_properties[0], properties, n*4); } else { msg_properties.push_back(0); } }
  msg_num_devices = num_devices;
  if (devices && num_devices*4) { msg_devices.resize(num_devices*4); memcpy(&msg_devices[0], devices, num_devices*4); }
  msg_callback = (cl_pointer)callback;
  msg_user_data = (cl_pointer)user_data;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateContext(
		msg_properties,
		msg_num_devices,
		msg_devices,
		msg_callback,
		msg_user_data,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_context)msg_func_ret;
}


cl_int
client_clReleaseContext(
    cl_context context) {
  cl_pointer msg_context;
  msg_context = (cl_pointer)context;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseContext(
		msg_context,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_command_queue
client_clCreateCommandQueue(
    cl_context context,
    cl_device_id device,
    cl_command_queue_properties properties,
    cl_int * errcode_ret) {
  cl_pointer msg_context;
  cl_pointer msg_device;
  cl_command_queue_properties msg_properties;
  cl_int msg_errcode_ret;
  msg_context = (cl_pointer)context;
  msg_device = (cl_pointer)device;
  msg_properties = properties;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateCommandQueue(
		msg_context,
		msg_device,
		msg_properties,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_command_queue)msg_func_ret;
}


cl_int
client_clReleaseCommandQueue(
    cl_command_queue command_queue) {
  cl_pointer msg_command_queue;
  msg_command_queue = (cl_pointer)command_queue;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseCommandQueue(
		msg_command_queue,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetCommandQueueInfo(
    cl_command_queue command_queue,
    cl_command_queue_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_command_queue;
  cl_command_queue_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_command_queue = (cl_pointer)command_queue;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetCommandQueueInfo(
		msg_command_queue,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_mem
client_clCreateBuffer(
    cl_context context,
    cl_mem_flags flags,
    size_t size,
    void * host_ptr,
    cl_int * errcode_ret) {
  cl_pointer msg_context;
  cl_mem_flags msg_flags;
  size_t msg_size;
  cl_pointer msg_host_ptr;
  cl_int msg_errcode_ret;
  msg_context = (cl_pointer)context;
  msg_flags = flags;
  msg_size = size;
  msg_host_ptr = (cl_pointer)host_ptr;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateBuffer(
		msg_context,
		msg_flags,
		msg_size,
		msg_host_ptr,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_mem)msg_func_ret;
}


cl_mem
client_clCreateSubBuffer(
    cl_mem buffer,
    cl_mem_flags flags,
    cl_buffer_create_type buffer_create_type,
    const void * buffer_create_info,
    cl_int * errcode_ret) {
  cl_pointer msg_buffer;
  cl_mem_flags msg_flags;
  cl_buffer_create_type msg_buffer_create_type;
  std::vector<unsigned char> msg_buffer_create_info;
  cl_int msg_errcode_ret;
  msg_buffer = (cl_pointer)buffer;
  msg_flags = flags;
  msg_buffer_create_type = buffer_create_type;
  { int tmp=sizeof(cl_buffer_region);
  msg_buffer_create_info.resize(tmp); memcpy(&msg_buffer_create_info[0], buffer_create_info, tmp);}
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateSubBuffer(
		msg_buffer,
		msg_flags,
		msg_buffer_create_type,
		msg_buffer_create_info,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_mem)msg_func_ret;
}


cl_int
client_clReleaseMemObject(
    cl_mem memobj) {
  cl_pointer msg_memobj;
  msg_memobj = (cl_pointer)memobj;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseMemObject(
		msg_memobj,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetSupportedImageFormats(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    cl_image_format * image_formats,
    cl_uint * num_image_formats) {
  cl_pointer msg_context;
  cl_mem_flags msg_flags;
  cl_mem_object_type msg_image_type;
  cl_uint msg_num_entries;
  std::vector<unsigned char> msg_image_formats;
  cl_uint msg_num_image_formats;
  msg_context = (cl_pointer)context;
  msg_flags = flags;
  msg_image_type = image_type;
  msg_num_entries = num_entries;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetSupportedImageFormats(
		msg_context,
		msg_flags,
		msg_image_type,
		msg_num_entries,
		&msg_image_formats,
		&msg_num_image_formats,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (image_formats)  memcpy(image_formats, &msg_image_formats[0], msg_image_formats.size());
  if (num_image_formats) *num_image_formats = *(cl_uint *)&msg_num_image_formats;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetMemObjectInfo(
    cl_mem memobj,
    cl_mem_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_memobj;
  cl_mem_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_memobj = (cl_pointer)memobj;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetMemObjectInfo(
		msg_memobj,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetImageInfo(
    cl_mem image,
    cl_image_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_image;
  cl_image_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_image = (cl_pointer)image;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetImageInfo(
		msg_image,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_sampler
client_clCreateSampler(
    cl_context context,
    cl_bool normalized_coords,
    cl_addressing_mode addressing_mode,
    cl_filter_mode filter_mode,
    cl_int * errcode_ret) {
  cl_pointer msg_context;
  cl_bool msg_normalized_coords;
  cl_addressing_mode msg_addressing_mode;
  cl_filter_mode msg_filter_mode;
  cl_int msg_errcode_ret;
  msg_context = (cl_pointer)context;
  msg_normalized_coords = normalized_coords;
  msg_addressing_mode = addressing_mode;
  msg_filter_mode = filter_mode;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateSampler(
		msg_context,
		msg_normalized_coords,
		msg_addressing_mode,
		msg_filter_mode,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_sampler)msg_func_ret;
}


cl_int
client_clReleaseSampler(
    cl_sampler sampler) {
  cl_pointer msg_sampler;
  msg_sampler = (cl_pointer)sampler;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseSampler(
		msg_sampler,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetSamplerInfo(
    cl_sampler sampler,
    cl_sampler_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_sampler;
  cl_sampler_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_sampler = (cl_pointer)sampler;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetSamplerInfo(
		msg_sampler,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_program
client_clCreateProgramWithSource(
    cl_context context,
    cl_uint count,
    const char ** strings,
    const size_t * lengths,
    cl_int * errcode_ret) {
  cl_pointer msg_context;
  cl_uint msg_count;
  std::vector<unsigned char> msg_strings;
  std::vector<unsigned char> msg_lengths;
  cl_int msg_errcode_ret;
  msg_context = (cl_pointer)context;
  msg_count = count;
 if (lengths != NULL || count != 1) throw "clCreateProgramSource impl accepts only 1 string";
{int tmp=strlen(*strings) + 1;
  msg_strings.resize(tmp); memcpy(&msg_strings[0], *strings, tmp);}
  if (lengths) {msg_lengths.resize(3*4); memcpy(&msg_lengths[0], lengths, 3*4);}
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateProgramWithSource(
		msg_context,
		msg_count,
		msg_strings,
		msg_lengths,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_program)msg_func_ret;
}


cl_int
client_clReleaseProgram(
    cl_program program) {
  cl_pointer msg_program;
  msg_program = (cl_pointer)program;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseProgram(
		msg_program,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clBuildProgram(
    cl_program program,
    cl_uint num_devices,
    const cl_device_id * device_list,
    const char * options,
    const void* callback,
    void * user_data) {
  cl_pointer msg_program;
  cl_uint msg_num_devices;
  std::vector<unsigned char> msg_device_list;
  std::vector<unsigned char> msg_options;
  cl_pointer msg_callback;
  cl_pointer msg_user_data;
  msg_program = (cl_pointer)program;
  msg_num_devices = num_devices;
  if (device_list && num_devices*4) { msg_device_list.resize(num_devices*4); memcpy(&msg_device_list[0], device_list, num_devices*4); }
  {int tmp=options ? strlen(options) + 1 : 1;
  if (tmp > 1) { msg_options.resize(tmp); memcpy(&msg_options[0], options, tmp);} else {msg_options.push_back(0);} }
  msg_callback = (cl_pointer)callback;
  msg_user_data = (cl_pointer)user_data;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clBuildProgram(
		msg_program,
		msg_num_devices,
		msg_device_list,
		msg_options,
		msg_callback,
		msg_user_data,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetProgramInfo(
    cl_program program,
    cl_program_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_program;
  cl_program_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_program = (cl_pointer)program;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetProgramInfo(
		msg_program,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetProgramBuildInfo(
    cl_program program,
    cl_device_id device,
    cl_program_build_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_program;
  cl_pointer msg_device;
  cl_program_build_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_program = (cl_pointer)program;
  msg_device = (cl_pointer)device;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetProgramBuildInfo(
		msg_program,
		msg_device,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_kernel
client_clCreateKernel(
    cl_program program,
    const char * kernel_name,
    cl_int * errcode_ret) {
  cl_pointer msg_program;
  std::vector<unsigned char> msg_kernel_name;
  cl_int msg_errcode_ret;
  msg_program = (cl_pointer)program;
  {int tmp=kernel_name ? strlen(kernel_name) + 1 : 1;
  if (tmp > 1) { msg_kernel_name.resize(tmp); memcpy(&msg_kernel_name[0], kernel_name, tmp);} else {msg_kernel_name.push_back(0);} }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateKernel(
		msg_program,
		msg_kernel_name,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_kernel)msg_func_ret;
}


cl_int
client_clCreateKernelsInProgram(
    cl_program program,
    cl_uint num_kernels,
    cl_kernel * kernels,
    cl_uint * num_kernels_ret) {
  cl_pointer msg_program;
  cl_uint msg_num_kernels;
  std::vector<unsigned char> msg_kernels;
  cl_uint msg_num_kernels_ret;
  msg_program = (cl_pointer)program;
  msg_num_kernels = num_kernels;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateKernelsInProgram(
		msg_program,
		msg_num_kernels,
		&msg_kernels,
		&msg_num_kernels_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (kernels)  memcpy(kernels, &msg_kernels[0], msg_kernels.size());
  if (num_kernels_ret) *num_kernels_ret = *(cl_uint *)&msg_num_kernels_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clReleaseKernel(
    cl_kernel kernel) {
  cl_pointer msg_kernel;
  msg_kernel = (cl_pointer)kernel;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseKernel(
		msg_kernel,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clSetKernelArg(
    cl_kernel kernel,
    cl_uint arg_index,
    size_t arg_size,
    const void * arg_value) {
  cl_pointer msg_kernel;
  cl_uint msg_arg_index;
  size_t msg_arg_size;
  std::vector<unsigned char> msg_arg_value;
  msg_kernel = (cl_pointer)kernel;
  msg_arg_index = arg_index;
  msg_arg_size = arg_size;
  if (arg_value && arg_size) { msg_arg_value.resize(arg_size); memcpy(&msg_arg_value[0], arg_value, arg_size); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clSetKernelArg(
		msg_kernel,
		msg_arg_index,
		msg_arg_size,
		msg_arg_value,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetKernelInfo(
    cl_kernel kernel,
    cl_kernel_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_kernel;
  cl_kernel_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_kernel = (cl_pointer)kernel;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetKernelInfo(
		msg_kernel,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetKernelWorkGroupInfo(
    cl_kernel kernel,
    cl_device_id device,
    cl_kernel_work_group_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_kernel;
  cl_pointer msg_device;
  cl_kernel_work_group_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_kernel = (cl_pointer)kernel;
  msg_device = (cl_pointer)device;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetKernelWorkGroupInfo(
		msg_kernel,
		msg_device,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clWaitForEvents(
    cl_uint num_events,
    const cl_event * event_list) {
  cl_uint msg_num_events;
  std::vector<unsigned char> msg_event_list;
  msg_num_events = num_events;
  if (event_list && num_events*4) { msg_event_list.resize(num_events*4); memcpy(&msg_event_list[0], event_list, num_events*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clWaitForEvents(
		msg_num_events,
		msg_event_list,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetEventInfo(
    cl_event event,
    cl_event_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_event;
  cl_event_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_event = (cl_pointer)event;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetEventInfo(
		msg_event,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_event
client_clCreateUserEvent(
    cl_context context,
    cl_int * errcode_ret) {
  cl_pointer msg_context;
  cl_int msg_errcode_ret;
  msg_context = (cl_pointer)context;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clCreateUserEvent(
		msg_context,
		&msg_errcode_ret,
		 &msg_func_ret
  ))) {
	*errcode_ret = CL_SEND_IPC_MESSAGE_FAILURE;
	return NULL;
  }
  if (errcode_ret) *errcode_ret = *(cl_int *)&msg_errcode_ret;
  return (cl_event)msg_func_ret;
}


cl_int
client_clReleaseEvent(
    cl_event event) {
  cl_pointer msg_event;
  msg_event = (cl_pointer)event;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clReleaseEvent(
		msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clSetUserEventStatus(
    cl_event event,
    cl_int execution_status) {
  cl_pointer msg_event;
  cl_int msg_execution_status;
  msg_event = (cl_pointer)event;
  msg_execution_status = execution_status;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clSetUserEventStatus(
		msg_event,
		msg_execution_status,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clSetEventCallback(
    cl_event event,
    cl_int command_exec_callback_type,
    const void* callback,
    void * user_data) {
  cl_pointer msg_event;
  cl_int msg_command_exec_callback_type;
  cl_pointer msg_callback;
  cl_pointer msg_user_data;
  msg_event = (cl_pointer)event;
  msg_command_exec_callback_type = command_exec_callback_type;
  msg_callback = (cl_pointer)callback;
  msg_user_data = (cl_pointer)user_data;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clSetEventCallback(
		msg_event,
		msg_command_exec_callback_type,
		msg_callback,
		msg_user_data,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clGetEventProfilingInfo(
    cl_event event,
    cl_profiling_info param_name,
    size_t param_value_size,
    void * param_value,
    size_t * param_value_size_ret) {
  cl_pointer msg_event;
  cl_profiling_info msg_param_name;
  size_t msg_param_value_size;
  std::vector<unsigned char> msg_param_value;
  size_t msg_param_value_size_ret;
  msg_event = (cl_pointer)event;
  msg_param_name = param_name;
  msg_param_value_size = param_value_size;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clGetEventProfilingInfo(
		msg_event,
		msg_param_name,
		msg_param_value_size,
		&msg_param_value,
		&msg_param_value_size_ret,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (param_value)  memcpy(param_value, &msg_param_value[0], msg_param_value.size());
  if (param_value_size_ret) *param_value_size_ret = *(size_t *)&msg_param_value_size_ret;
  return (cl_int)msg_func_ret;
}


cl_int
client_clFlush(
    cl_command_queue command_queue) {
  cl_pointer msg_command_queue;
  msg_command_queue = (cl_pointer)command_queue;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clFlush(
		msg_command_queue,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clFinish(
    cl_command_queue command_queue) {
  cl_pointer msg_command_queue;
  msg_command_queue = (cl_pointer)command_queue;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clFinish(
		msg_command_queue,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueReadBuffer(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_read,
    size_t offset,
    size_t cb,
    void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_buffer;
  cl_bool msg_blocking_read;
  size_t msg_offset;
  size_t msg_cb;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_buffer = (cl_pointer)buffer;
  msg_blocking_read = blocking_read;
  msg_offset = offset;
  msg_cb = cb;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueReadBuffer(
		msg_command_queue,
		msg_buffer,
		msg_blocking_read,
		msg_offset,
		msg_cb,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_ptr,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (ptr)  memcpy(ptr, &msg_ptr[0], msg_ptr.size());
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueReadBufferRect(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_read,
    const size_t * buffer_origin,
    const size_t * host_origin,
    const size_t * region,
    size_t buffer_row_pitch,
    size_t buffer_slice_pitch,
    size_t host_row_pitch,
    size_t host_slice_pitch,
    void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_buffer;
  cl_bool msg_blocking_read;
  std::vector<unsigned char> msg_buffer_origin;
  std::vector<unsigned char> msg_host_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_buffer_row_pitch;
  size_t msg_buffer_slice_pitch;
  size_t msg_host_row_pitch;
  size_t msg_host_slice_pitch;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_buffer = (cl_pointer)buffer;
  msg_blocking_read = blocking_read;
  if (buffer_origin) {msg_buffer_origin.resize(3*4); memcpy(&msg_buffer_origin[0], buffer_origin, 3*4);}
  if (host_origin) {msg_host_origin.resize(3*4); memcpy(&msg_host_origin[0], host_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_buffer_row_pitch = buffer_row_pitch;
  msg_buffer_slice_pitch = buffer_slice_pitch;
  msg_host_row_pitch = host_row_pitch;
  msg_host_slice_pitch = host_slice_pitch;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueReadBufferRect(
		msg_command_queue,
		msg_buffer,
		msg_blocking_read,
		msg_buffer_origin,
		msg_host_origin,
		msg_region,
		msg_buffer_row_pitch,
		msg_buffer_slice_pitch,
		msg_host_row_pitch,
		msg_host_slice_pitch,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_ptr,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (ptr)  memcpy(ptr, &msg_ptr[0], msg_ptr.size());
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueWriteBuffer(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_write,
    size_t offset,
    size_t cb,
    const void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_buffer;
  cl_bool msg_blocking_write;
  size_t msg_offset;
  size_t msg_cb;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_buffer = (cl_pointer)buffer;
  msg_blocking_write = blocking_write;
  msg_offset = offset;
  msg_cb = cb;
  if (ptr && cb) { msg_ptr.resize(cb); memcpy(&msg_ptr[0], ptr, cb); }
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueWriteBuffer(
		msg_command_queue,
		msg_buffer,
		msg_blocking_write,
		msg_offset,
		msg_cb,
		msg_ptr,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueWriteBufferRect(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_write,
    const size_t * buffer_origin,
    const size_t * host_origin,
    const size_t * region,
    size_t buffer_row_pitch,
    size_t buffer_slice_pitch,
    size_t host_row_pitch,
    size_t host_slice_pitch,
    const void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_buffer;
  cl_bool msg_blocking_write;
  std::vector<unsigned char> msg_buffer_origin;
  std::vector<unsigned char> msg_host_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_buffer_row_pitch;
  size_t msg_buffer_slice_pitch;
  size_t msg_host_row_pitch;
  size_t msg_host_slice_pitch;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_buffer = (cl_pointer)buffer;
  msg_blocking_write = blocking_write;
  if (buffer_origin) {msg_buffer_origin.resize(3*4); memcpy(&msg_buffer_origin[0], buffer_origin, 3*4);}
  if (host_origin) {msg_host_origin.resize(3*4); memcpy(&msg_host_origin[0], host_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_buffer_row_pitch = buffer_row_pitch;
  msg_buffer_slice_pitch = buffer_slice_pitch;
  msg_host_row_pitch = host_row_pitch;
  msg_host_slice_pitch = host_slice_pitch;
  msg_ptr.resize(region[0]*region[1]*region[2]);
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueWriteBufferRect(
		msg_command_queue,
		msg_buffer,
		msg_blocking_write,
		msg_buffer_origin,
		msg_host_origin,
		msg_region,
		msg_buffer_row_pitch,
		msg_buffer_slice_pitch,
		msg_host_row_pitch,
		msg_host_slice_pitch,
		msg_ptr,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueCopyBuffer(
    cl_command_queue command_queue,
    cl_mem src_buffer,
    cl_mem dst_buffer,
    size_t src_offset,
    size_t dst_offset,
    size_t cb,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_src_buffer;
  cl_pointer msg_dst_buffer;
  size_t msg_src_offset;
  size_t msg_dst_offset;
  size_t msg_cb;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_src_buffer = (cl_pointer)src_buffer;
  msg_dst_buffer = (cl_pointer)dst_buffer;
  msg_src_offset = src_offset;
  msg_dst_offset = dst_offset;
  msg_cb = cb;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueCopyBuffer(
		msg_command_queue,
		msg_src_buffer,
		msg_dst_buffer,
		msg_src_offset,
		msg_dst_offset,
		msg_cb,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueCopyBufferRect(
    cl_command_queue command_queue,
    cl_mem src_buffer,
    cl_mem dst_buffer,
    const size_t * src_origin,
    const size_t * dst_origin,
    const size_t * region,
    size_t src_row_pitch,
    size_t src_slice_pitch,
    size_t dst_row_pitch,
    size_t dst_slice_pitch,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_src_buffer;
  cl_pointer msg_dst_buffer;
  std::vector<unsigned char> msg_src_origin;
  std::vector<unsigned char> msg_dst_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_src_row_pitch;
  size_t msg_src_slice_pitch;
  size_t msg_dst_row_pitch;
  size_t msg_dst_slice_pitch;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_src_buffer = (cl_pointer)src_buffer;
  msg_dst_buffer = (cl_pointer)dst_buffer;
  if (src_origin) {msg_src_origin.resize(3*4); memcpy(&msg_src_origin[0], src_origin, 3*4);}
  if (dst_origin) {msg_dst_origin.resize(3*4); memcpy(&msg_dst_origin[0], dst_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_src_row_pitch = src_row_pitch;
  msg_src_slice_pitch = src_slice_pitch;
  msg_dst_row_pitch = dst_row_pitch;
  msg_dst_slice_pitch = dst_slice_pitch;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueCopyBufferRect(
		msg_command_queue,
		msg_src_buffer,
		msg_dst_buffer,
		msg_src_origin,
		msg_dst_origin,
		msg_region,
		msg_src_row_pitch,
		msg_src_slice_pitch,
		msg_dst_row_pitch,
		msg_dst_slice_pitch,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueReadImage(
    cl_command_queue command_queue,
    cl_mem image,
    cl_bool blocking_read,
    const size_t * origin,
    const size_t * region,
    size_t row_pitch,
    size_t slice_pitch,
    void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_image;
  cl_bool msg_blocking_read;
  std::vector<unsigned char> msg_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_row_pitch;
  size_t msg_slice_pitch;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_image = (cl_pointer)image;
  msg_blocking_read = blocking_read;
  if (origin) {msg_origin.resize(3*4); memcpy(&msg_origin[0], origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_row_pitch = row_pitch;
  msg_slice_pitch = slice_pitch;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueReadImage(
		msg_command_queue,
		msg_image,
		msg_blocking_read,
		msg_origin,
		msg_region,
		msg_row_pitch,
		msg_slice_pitch,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_ptr,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
if (ptr)  memcpy(ptr, &msg_ptr[0], msg_ptr.size());
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueWriteImage(
    cl_command_queue command_queue,
    cl_mem image,
    cl_bool blocking_write,
    const size_t * origin,
    const size_t * region,
    size_t input_row_pitch,
    size_t input_slice_pitch,
    const void * ptr,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_image;
  cl_bool msg_blocking_write;
  std::vector<unsigned char> msg_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_input_row_pitch;
  size_t msg_input_slice_pitch;
  std::vector<unsigned char> msg_ptr;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_image = (cl_pointer)image;
  msg_blocking_write = blocking_write;
  if (origin) {msg_origin.resize(3*4); memcpy(&msg_origin[0], origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_input_row_pitch = input_row_pitch;
  msg_input_slice_pitch = input_slice_pitch;
  msg_ptr.resize(region[0]*region[1]*region[2]);
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueWriteImage(
		msg_command_queue,
		msg_image,
		msg_blocking_write,
		msg_origin,
		msg_region,
		msg_input_row_pitch,
		msg_input_slice_pitch,
		msg_ptr,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueCopyImage(
    cl_command_queue command_queue,
    cl_mem src_image,
    cl_mem dst_image,
    const size_t * src_origin,
    const size_t * dst_origin,
    const size_t * region,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_src_image;
  cl_pointer msg_dst_image;
  std::vector<unsigned char> msg_src_origin;
  std::vector<unsigned char> msg_dst_origin;
  std::vector<unsigned char> msg_region;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_src_image = (cl_pointer)src_image;
  msg_dst_image = (cl_pointer)dst_image;
  if (src_origin) {msg_src_origin.resize(3*4); memcpy(&msg_src_origin[0], src_origin, 3*4);}
  if (dst_origin) {msg_dst_origin.resize(3*4); memcpy(&msg_dst_origin[0], dst_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueCopyImage(
		msg_command_queue,
		msg_src_image,
		msg_dst_image,
		msg_src_origin,
		msg_dst_origin,
		msg_region,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueCopyImageToBuffer(
    cl_command_queue command_queue,
    cl_mem src_image,
    cl_mem dst_buffer,
    const size_t * src_origin,
    const size_t * region,
    size_t dst_offset,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_src_image;
  cl_pointer msg_dst_buffer;
  std::vector<unsigned char> msg_src_origin;
  std::vector<unsigned char> msg_region;
  size_t msg_dst_offset;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_src_image = (cl_pointer)src_image;
  msg_dst_buffer = (cl_pointer)dst_buffer;
  if (src_origin) {msg_src_origin.resize(3*4); memcpy(&msg_src_origin[0], src_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_dst_offset = dst_offset;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueCopyImageToBuffer(
		msg_command_queue,
		msg_src_image,
		msg_dst_buffer,
		msg_src_origin,
		msg_region,
		msg_dst_offset,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueCopyBufferToImage(
    cl_command_queue command_queue,
    cl_mem src_buffer,
    cl_mem dst_image,
    size_t src_offset,
    const size_t * dst_origin,
    const size_t * region,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_src_buffer;
  cl_pointer msg_dst_image;
  size_t msg_src_offset;
  std::vector<unsigned char> msg_dst_origin;
  std::vector<unsigned char> msg_region;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_src_buffer = (cl_pointer)src_buffer;
  msg_dst_image = (cl_pointer)dst_image;
  msg_src_offset = src_offset;
  if (dst_origin) {msg_dst_origin.resize(3*4); memcpy(&msg_dst_origin[0], dst_origin, 3*4);}
  if (region) {msg_region.resize(3*4); memcpy(&msg_region[0], region, 3*4);}
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueCopyBufferToImage(
		msg_command_queue,
		msg_src_buffer,
		msg_dst_image,
		msg_src_offset,
		msg_dst_origin,
		msg_region,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueNDRangeKernel(
    cl_command_queue command_queue,
    cl_kernel kernel,
    cl_uint work_dim,
    const size_t * global_work_offset,
    const size_t * global_work_size,
    const size_t * local_work_size,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_kernel;
  cl_uint msg_work_dim;
  std::vector<unsigned char> msg_global_work_offset;
  std::vector<unsigned char> msg_global_work_size;
  std::vector<unsigned char> msg_local_work_size;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_kernel = (cl_pointer)kernel;
  msg_work_dim = work_dim;
  if (global_work_offset) {msg_global_work_offset.resize(work_dim*4); memcpy(&msg_global_work_offset[0], global_work_offset, work_dim*4);}
  if (global_work_size) {msg_global_work_size.resize(work_dim*4); memcpy(&msg_global_work_size[0], global_work_size, work_dim*4);}
  if (local_work_size) {msg_local_work_size.resize(work_dim*4); memcpy(&msg_local_work_size[0], local_work_size, work_dim*4);}
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueNDRangeKernel(
		msg_command_queue,
		msg_kernel,
		msg_work_dim,
		msg_global_work_offset,
		msg_global_work_size,
		msg_local_work_size,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueTask(
    cl_command_queue command_queue,
    cl_kernel kernel,
    cl_uint num_events_in_wait_list,
    const cl_event * event_wait_list,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_kernel;
  cl_uint msg_num_events_in_wait_list;
  std::vector<unsigned char> msg_event_wait_list;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  msg_kernel = (cl_pointer)kernel;
  msg_num_events_in_wait_list = num_events_in_wait_list;
  if (event_wait_list && num_events_in_wait_list*4) { msg_event_wait_list.resize(num_events_in_wait_list*4); memcpy(&msg_event_wait_list[0], event_wait_list, num_events_in_wait_list*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueTask(
		msg_command_queue,
		msg_kernel,
		msg_num_events_in_wait_list,
		msg_event_wait_list,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueMarker(
    cl_command_queue command_queue,
    cl_event * event) {
  cl_pointer msg_command_queue;
  cl_pointer msg_event;
  msg_command_queue = (cl_pointer)command_queue;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueMarker(
		msg_command_queue,
		&msg_event,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  if (event) *event = *(cl_event *)&msg_event;
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueWaitForEvents(
    cl_command_queue command_queue,
    cl_uint num_events,
    const cl_event * event_list) {
  cl_pointer msg_command_queue;
  cl_uint msg_num_events;
  std::vector<unsigned char> msg_event_list;
  msg_command_queue = (cl_pointer)command_queue;
  msg_num_events = num_events;
  if (event_list && num_events*4) { msg_event_list.resize(num_events*4); memcpy(&msg_event_list[0], event_list, num_events*4); }
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueWaitForEvents(
		msg_command_queue,
		msg_num_events,
		msg_event_list,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


cl_int
client_clEnqueueBarrier(
    cl_command_queue command_queue) {
  cl_pointer msg_command_queue;
  msg_command_queue = (cl_pointer)command_queue;
  cl_pointer msg_func_ret;
  if (!__ocl_gpu_channel_host->Send(new OpenCLIPCMsg_clEnqueueBarrier(
		msg_command_queue,
		 &msg_func_ret
  ))) {
	return CL_SEND_IPC_MESSAGE_FAILURE;
  }
  return (cl_int)msg_func_ret;
}


