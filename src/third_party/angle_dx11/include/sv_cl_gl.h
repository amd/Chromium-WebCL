#ifndef __SV_CL_GL_H__
#define __SV_CL_GL_H__

#include <CL/cl.h>
#include <CL/cl_gl.h>

extern "C" {

cl_int __stdcall sv_createSharedCLContext(cl_platform_id platform, cl_context *context);
cl_mem __stdcall sv_clCreateFromGLBuffer (	cl_context context,
 	cl_mem_flags flags,
 	GLuint bufobj,
 	cl_int * errcode_ret);

cl_mem __stdcall sv_clCreateFromGLTexture (	cl_context context,
 	cl_mem_flags flags,
 	GLenum texture_target,
 	GLint miplevel,
 	GLuint texture,
 	cl_int *errcode_ret);

cl_mem __stdcall sv_clCreateFromGLRenderbuffer (	cl_context context,
 	cl_mem_flags flags,
 	GLuint renderbuffer,
 	cl_int * errcode_ret);

cl_int __stdcall sv_clEnqueueAcquireGLObjects (	cl_command_queue command_queue,
 	cl_uint num_objects,
 	const cl_mem *mem_objects,
 	cl_uint num_events_in_wait_list,
 	const cl_event *event_wait_list,
 	cl_event *event);

cl_int __stdcall sv_clEnqueueReleaseGLObjects (	cl_command_queue command_queue,
 	cl_uint num_objects,
 	const cl_mem *mem_objects,
 	cl_uint num_events_in_wait_list,
 	const cl_event *event_wait_list,
 	cl_event *event);

cl_int __stdcall sv_clGetGLObjectInfo (	cl_mem memobj,
 	cl_gl_object_type *gl_object_type,
 	GLuint *gl_object_name);

cl_int __stdcall sv_clGetGLTextureInfo (	cl_mem memobj,
 	cl_gl_texture_info param_name,
 	size_t param_value_size,
 	void *param_value,
 	size_t *param_value_size_ret);
}
#endif
