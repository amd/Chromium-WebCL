// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_CLIENT_GPU_CHANNEL_HOST_H_
#define CONTENT_COMMON_GPU_CLIENT_GPU_CHANNEL_HOST_H_

#include <string>
#include <vector>

#include "base/atomic_sequence_num.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/process/process.h"
#include "base/synchronization/lock.h"
#include "content/common/content_export.h"
#include "content/common/gpu/gpu_process_launch_causes.h"
#include "content/common/message_router.h"
#include "gpu/config/gpu_info.h"
#include "ipc/ipc_channel_handle.h"
#include "ipc/ipc_channel_proxy.h"
#include "ipc/ipc_sync_channel.h"
#include "media/video/video_decode_accelerator.h"
#include "media/video/video_encode_accelerator.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/size.h"
#include "ui/gl/gpu_preference.h"

#if defined(OS_WIN)
#include <CL/OpenCL.h>
#endif

class GURL;
class TransportTextureService;
struct GPUCreateCommandBufferConfig;

namespace base {
class MessageLoop;
class MessageLoopProxy;
}

namespace gpu {
struct Mailbox;
}

namespace IPC {
class SyncMessageFilter;
}

namespace content {
class CommandBufferProxyImpl;
class GpuChannelHost;
struct GpuRenderingStats;

struct GpuListenerInfo {
  GpuListenerInfo();
  ~GpuListenerInfo();

  base::WeakPtr<IPC::Listener> listener;
  scoped_refptr<base::MessageLoopProxy> loop;
};

class CONTENT_EXPORT GpuChannelHostFactory {
 public:
  typedef base::Callback<void(const gfx::Size)> CreateImageCallback;

  virtual ~GpuChannelHostFactory() {}

  virtual bool IsMainThread() = 0;
  virtual base::MessageLoop* GetMainLoop() = 0;
  virtual scoped_refptr<base::MessageLoopProxy> GetIOLoopProxy() = 0;
  virtual base::WaitableEvent* GetShutDownEvent() = 0;
  virtual scoped_ptr<base::SharedMemory> AllocateSharedMemory(size_t size) = 0;
  virtual int32 CreateViewCommandBuffer(
      int32 surface_id, const GPUCreateCommandBufferConfig& init_params) = 0;
  virtual void CreateImage(
      gfx::PluginWindowHandle window,
      int32 image_id,
      const CreateImageCallback& callback) = 0;
  virtual void DeleteImage(int32 image_id, int32 sync_point) = 0;
  virtual scoped_ptr<gfx::GpuMemoryBuffer> AllocateGpuMemoryBuffer(
      size_t width,
      size_t height,
      unsigned internalformat) = 0;
};

// Encapsulates an IPC channel between the client and one GPU process.
// On the GPU process side there's a corresponding GpuChannel.
// Every method can be called on any thread with a message loop, except for the
// IO thread.
class GpuChannelHost : public IPC::Sender,
                       public base::RefCountedThreadSafe<GpuChannelHost> {
 public:
  // Must be called on the main thread (as defined by the factory).
  static scoped_refptr<GpuChannelHost> Create(
      GpuChannelHostFactory* factory,
      int gpu_host_id,
      int client_id,
      const gpu::GPUInfo& gpu_info,
      const IPC::ChannelHandle& channel_handle);

  // Returns true if |handle| is a valid GpuMemoryBuffer handle that
  // can be shared to the GPU process.
  static bool IsValidGpuMemoryBuffer(gfx::GpuMemoryBufferHandle handle);

  bool IsLost() const {
    DCHECK(channel_filter_.get());
    return channel_filter_->IsLost();
  }

  // The GPU stats reported by the GPU process.
  const gpu::GPUInfo& gpu_info() const { return gpu_info_; }

  // IPC::Sender implementation:
  virtual bool Send(IPC::Message* msg) OVERRIDE;

  // Create and connect to a command buffer in the GPU process.
  CommandBufferProxyImpl* CreateViewCommandBuffer(
      int32 surface_id,
      CommandBufferProxyImpl* share_group,
      const std::vector<int32>& attribs,
      const GURL& active_url,
      gfx::GpuPreference gpu_preference);

  // Create and connect to a command buffer in the GPU process.
  CommandBufferProxyImpl* CreateOffscreenCommandBuffer(
      const gfx::Size& size,
      CommandBufferProxyImpl* share_group,
      const std::vector<int32>& attribs,
      const GURL& active_url,
      gfx::GpuPreference gpu_preference);

  // Creates a video decoder in the GPU process.
  scoped_ptr<media::VideoDecodeAccelerator> CreateVideoDecoder(
      int command_buffer_route_id,
      media::VideoCodecProfile profile,
      media::VideoDecodeAccelerator::Client* client);

  // Creates a video encoder in the GPU process.
  scoped_ptr<media::VideoEncodeAccelerator> CreateVideoEncoder(
      media::VideoEncodeAccelerator::Client* client);

  // Destroy a command buffer created by this channel.
  void DestroyCommandBuffer(CommandBufferProxyImpl* command_buffer);

  // Collect rendering stats from GPU process.
  bool CollectRenderingStatsForSurface(
      int surface_id, GpuRenderingStats* stats);

  // Add a route for the current message loop.
  void AddRoute(int route_id, base::WeakPtr<IPC::Listener> listener);
  void RemoveRoute(int route_id);

  GpuChannelHostFactory* factory() const { return factory_; }
  int gpu_host_id() const { return gpu_host_id_; }

  int client_id() const { return client_id_; }

  // Returns a handle to the shared memory that can be sent via IPC to the
  // GPU process. The caller is responsible for ensuring it is closed. Returns
  // an invalid handle on failure.
  base::SharedMemoryHandle ShareToGpuProcess(
      base::SharedMemoryHandle source_handle);

  // Generates |num| unique mailbox names that can be used with
  // GL_texture_mailbox_CHROMIUM. Unlike genMailboxCHROMIUM, this IPC is
  // handled only on the GPU process' IO thread, and so is not effectively
  // a finish.
  bool GenerateMailboxNames(unsigned num, std::vector<gpu::Mailbox>* names);

  // Reserve one unused transfer buffer ID.
  int32 ReserveTransferBufferId();

  // Returns a GPU memory buffer handle to the buffer that can be sent via
  // IPC to the GPU process. The caller is responsible for ensuring it is
  // closed. Returns an invalid handle on failure.
  gfx::GpuMemoryBufferHandle ShareGpuMemoryBufferToGpuProcess(
      gfx::GpuMemoryBufferHandle source_handle);

  // Reserve one unused gpu memory buffer ID.
  int32 ReserveGpuMemoryBufferId();

 private:
  friend class base::RefCountedThreadSafe<GpuChannelHost>;
  GpuChannelHost(GpuChannelHostFactory* factory,
                 int gpu_host_id,
                 int client_id,
                 const gpu::GPUInfo& gpu_info);
  virtual ~GpuChannelHost();
  void Connect(const IPC::ChannelHandle& channel_handle);

  // A filter used internally to route incoming messages from the IO thread
  // to the correct message loop. It also maintains some shared state between
  // all the contexts.
  class MessageFilter : public IPC::ChannelProxy::MessageFilter {
   public:
    MessageFilter();

    // Called on the IO thread.
    void AddRoute(int route_id,
                  base::WeakPtr<IPC::Listener> listener,
                  scoped_refptr<base::MessageLoopProxy> loop);
    // Called on the IO thread.
    void RemoveRoute(int route_id);

    // IPC::ChannelProxy::MessageFilter implementation
    // (called on the IO thread):
    virtual bool OnMessageReceived(const IPC::Message& msg) OVERRIDE;
    virtual void OnChannelError() OVERRIDE;

    // The following methods can be called on any thread.

    // Whether the channel is lost.
    bool IsLost() const;

    // Gets mailboxes from the pool, and return the number of mailboxes to ask
    // the GPU process to maintain a good pool size. The caller is responsible
    // for sending the GpuChannelMsg_GenerateMailboxNamesAsync message.
    size_t GetMailboxNames(size_t num, std::vector<gpu::Mailbox>* names);

   private:
    virtual ~MessageFilter();
    bool OnControlMessageReceived(const IPC::Message& msg);

    // Message handlers.
    void OnGenerateMailboxNamesReply(const std::vector<gpu::Mailbox>& names);

    // Threading notes: |listeners_| is only accessed on the IO thread. Every
    // other field is protected by |lock_|.
    typedef base::hash_map<int, GpuListenerInfo> ListenerMap;
    ListenerMap listeners_;

    // Protexts all fields below this one.
    mutable base::Lock lock_;

    // Whether the channel has been lost.
    bool lost_;

    // A pool of valid mailbox names.
    std::vector<gpu::Mailbox> mailbox_name_pool_;

    // Number of pending mailbox requested from the GPU process.
    size_t requested_mailboxes_;
  };

  // Threading notes: all fields are constant during the lifetime of |this|
  // except:
  // - |next_transfer_buffer_id_|, atomic type
  // - |next_gpu_memory_buffer_id_|, atomic type
  // - |proxies_|, protected by |context_lock_|
  GpuChannelHostFactory* const factory_;
  const int client_id_;
  const int gpu_host_id_;

  const gpu::GPUInfo gpu_info_;

  scoped_ptr<IPC::SyncChannel> channel_;
  scoped_refptr<MessageFilter> channel_filter_;

  // A filter for sending messages from thread other than the main thread.
  scoped_refptr<IPC::SyncMessageFilter> sync_filter_;

  // Transfer buffer IDs are allocated in sequence.
  base::AtomicSequenceNumber next_transfer_buffer_id_;

  // Gpu memory buffer IDs are allocated in sequence.
  base::AtomicSequenceNumber next_gpu_memory_buffer_id_;

  // Protects proxies_.
  mutable base::Lock context_lock_;
  // Used to look up a proxy from its routing id.
  typedef base::hash_map<int, CommandBufferProxyImpl*> ProxyMap;
  ProxyMap proxies_;




 public:
  // Calling OpenCL API by IPC message, and run it in another process.
  cl_int CallclGetPlatformIDs(
      cl_uint,
      cl_platform_id*,
      cl_uint*);

  cl_int CallclGetPlatformInfo(
      cl_platform_id,
      cl_platform_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclGetDeviceIDs(
      cl_platform_id,
      cl_device_type,
      cl_uint,
      cl_device_id*,
      cl_uint*);

  cl_int CallclGetDeviceInfo(
      cl_device_id,
      cl_device_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclCreateSubDevices(
      cl_device_id,
      const cl_device_partition_property*,
      cl_uint,
      cl_device_id*,
      cl_uint*);

  cl_int CallclRetainDevice (cl_device_id);

  cl_int CallclReleaseDevice (cl_device_id);

  cl_context CallclCreateContext(
      const cl_context_properties*,
      cl_uint,
      const cl_device_id*,
      void (CL_CALLBACK*)(const char*, const void*, size_t, void*),
      void*,
      cl_int*);

  cl_context CallclCreateContextFromType(
      const cl_context_properties*,
      cl_device_type,
      void (CL_CALLBACK*)(const char*, const void*, size_t, void*),
      void*,
      cl_int*);

  cl_int CallclRetainContext(cl_context);

  cl_int CallclReleaseContext(cl_context);

  cl_int CallclGetContextInfo(
      cl_context,
      cl_context_info,
      size_t,
      void*,
      size_t*);

  cl_command_queue CallclCreateCommandQueue(
      cl_context,
      cl_device_id,
      cl_command_queue_properties,
      cl_int*);

  cl_int CallclRetainCommandQueue(cl_command_queue);

  cl_int CallclReleaseCommandQueue(cl_command_queue);

  cl_int CallclGetCommandQueueInfo(
      cl_command_queue,
      cl_command_queue_info,
      size_t,
      void*,
      size_t*);

  cl_mem CallclCreateBuffer(
      cl_context,
      cl_mem_flags,
      size_t,
      void*,
      cl_int*);

  cl_mem CallclCreateSubBuffer(
      cl_mem,
      cl_mem_flags,
      cl_buffer_create_type,
      const void*,
      cl_int*);

  cl_mem CallclCreateImage(
      cl_context,
      cl_mem_flags,
      const cl_image_format*,
      const cl_image_desc*,
      void*,
      cl_int*);

  cl_int CallclRetainMemObject(cl_mem);

  cl_int CallclReleaseMemObject(cl_mem);

  cl_int CallclGetSupportedImageFormats(
      cl_context,
      cl_mem_flags,
      cl_mem_object_type,
      cl_uint,
      cl_image_format*,
      cl_uint*);

  cl_int CallclGetMemObjectInfo(
      cl_mem,
      cl_mem_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclGetImageInfo(
      cl_mem,
      cl_image_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclSetMemObjectDestructorCallback(
      cl_mem,
      void (CL_CALLBACK*)(cl_mem,void*),
      void*);

  cl_sampler CallclCreateSampler(
      cl_context,
      cl_bool,
      cl_addressing_mode,
      cl_filter_mode,
      cl_int*);

  cl_int CallclRetainSampler(cl_sampler);

  cl_int CallclReleaseSampler(cl_sampler);

  cl_int CallclGetSamplerInfo(
      cl_sampler,
      cl_sampler_info,
      size_t,
      void*,
      size_t*);

  cl_program CallclCreateProgramWithSource(
      cl_context,
      cl_uint,
      const char**,
      const size_t*,
      cl_int*);

  cl_program CallclCreateProgramWithBinary(
      cl_context,
      cl_uint,
      const cl_device_id*,
      const size_t*,
      const unsigned char**,
      cl_int*,
      cl_int*);

  cl_program CallclCreateProgramWithBuiltInKernels(
      cl_context,
      cl_uint,
      const cl_device_id*,
      const char*,
      cl_int*);

  cl_int CallclRetainProgram(cl_program);

  cl_int CallclReleaseProgram(cl_program);

  cl_int CallclBuildProgram(
      cl_program,
      cl_uint,
      const cl_device_id*,
      const char*,
      void (CL_CALLBACK*)(cl_program, void*),
      void*);

  cl_int CallclCompileProgram(
      cl_program,
      cl_uint,
      const cl_device_id*,
      const char*,
      cl_uint,
      const cl_program*,
      const char**,
      void (CL_CALLBACK*)(cl_program, void*),
      void*);

  cl_program CallclLinkProgram(
      cl_context,
      cl_uint,
      const cl_device_id*,
      const char*,
      cl_uint,
      const cl_program*,
      void (CL_CALLBACK*)(cl_program, void*),
      void*, cl_int*);

  cl_int CallclUnloadPlatformCompiler(cl_platform_id);

  cl_int CallclGetProgramInfo(
      cl_program,
      cl_program_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclGetProgramBuildInfo(
      cl_program,
      cl_device_id,
      cl_program_build_info,
      size_t,
      void*,
      size_t*);

  cl_kernel CallclCreateKernel(
      cl_program,
      const char*,
      cl_int*);

  cl_int CallclCreateKernelsInProgram(
      cl_program,
      cl_uint,
      cl_kernel*,
      cl_uint*);

  cl_int CallclRetainKernel(cl_kernel);

  cl_int CallclReleaseKernel(cl_kernel);

  cl_int CallclSetKernelArg(
      cl_kernel,
      cl_uint,
      size_t,
      const void*);

  cl_int CallclSetKernelArg_vector(
      cl_kernel,
      cl_uint,
      size_t,
      const void*);

  cl_int CallclGetKernelInfo(
      cl_kernel,
      cl_kernel_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclGetKernelArgInfo(
      cl_kernel,
      cl_uint,
      cl_kernel_arg_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclGetKernelWorkGroupInfo(
      cl_kernel,
      cl_device_id,
      cl_kernel_work_group_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclWaitForEvents(
      cl_uint,
      const cl_event*);

  cl_int CallclGetEventInfo(
      cl_event,
      cl_event_info,
      size_t,
      void*,
      size_t*);

  cl_event CallclCreateUserEvent(
      cl_context,
      cl_int*);

  cl_int CallclRetainEvent(
      cl_event);

  cl_int CallclReleaseEvent(cl_event);

  cl_int CallclSetUserEventStatus(
      cl_event,
      cl_int);

  cl_int CallclSetEventCallback(
      cl_event,
      cl_int,
      void (CL_CALLBACK*)(cl_event, cl_int, void*),
      void*);

  cl_int CallclGetEventProfilingInfo(
      cl_event,
      cl_profiling_info,
      size_t,
      void*,
      size_t*);

  cl_int CallclFlush(cl_command_queue);

  cl_int CallclFinish (cl_command_queue);

  cl_int CallclEnqueueReadBuffer(
      cl_command_queue,
      cl_mem,
      cl_bool,
      size_t,
      size_t,
      void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueReadBufferRect(
      cl_command_queue,
      cl_mem,
      cl_bool,
      const size_t*,
      const size_t*,
      const size_t*,
      size_t,
      size_t,
      size_t,
      size_t,
      void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueWriteBuffer(
      cl_command_queue,
      cl_mem,
      cl_bool,
      size_t,
      size_t,
      const void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueWriteBufferRect(
      cl_command_queue,
      cl_mem,
      cl_bool,
      const size_t*,
      const size_t*,
      const size_t*,
      size_t,
      size_t,
      size_t,
      size_t,
      const void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueFillBuffer(
      cl_command_queue,
      cl_mem,
      const void*,
      size_t,
      size_t,
      size_t,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueCopyBuffer(
      cl_command_queue,
      cl_mem,
      cl_mem,
      size_t,
      size_t,
      size_t,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueCopyBufferRect(
      cl_command_queue,
      cl_mem,
      cl_mem,
      const size_t*,
      const size_t*,
      const size_t*,
      size_t,
      size_t,
      size_t,
      size_t,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueReadImage(
      cl_command_queue,
      cl_mem,
      cl_bool,
      const size_t*,
      const size_t*,
      size_t,
      size_t,
      void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueWriteImage(
      cl_command_queue,
      cl_mem,
      cl_bool,
      const size_t*,
      const size_t*,
      size_t,
      size_t,
      const void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueFillImage(
      cl_command_queue,
      cl_mem,
      const void*,
      const size_t*,
      const size_t*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueCopyImage(
      cl_command_queue,
      cl_mem,
      cl_mem,
      const size_t*,
      const size_t*,
      const size_t*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueCopyImageToBuffer(
      cl_command_queue,
      cl_mem,
      cl_mem,
      const size_t*,
      const size_t*,
      size_t,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueCopyBufferToImage(
      cl_command_queue,
      cl_mem,
      cl_mem,
      size_t,
      const size_t*,
      const size_t*,
      cl_uint,
      const cl_event*,
      cl_event*);

  void* CallclEnqueueMapBuffer(
      cl_command_queue,
      cl_mem,
      cl_bool,
      cl_map_flags,
      size_t, size_t,
      cl_uint,
      const cl_event*,
      cl_event*,
      cl_int*);

  void* CallclEnqueueMapImage(
      cl_command_queue,
      cl_mem,
      cl_bool,
      cl_map_flags,
      const size_t*,
      const size_t*,
      size_t*,
      size_t*,
      cl_uint,
      const cl_event*,
      cl_event*,
      cl_int*);

  cl_int CallclEnqueueUnmapMemObject(
      cl_command_queue,
      cl_mem,
      void*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueMigrateMemObjects(
      cl_command_queue,
      cl_uint,
      const cl_mem*,
      cl_mem_migration_flags,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueNDRangeKernel(
      cl_command_queue,
      cl_kernel,
      cl_uint,
      const size_t*,
      const size_t*,
      const size_t*,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueTask(
      cl_command_queue,
      cl_kernel,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueNativeKernel(
      cl_command_queue,
      void (CL_CALLBACK*)(void*),
      void*,
      size_t,
      cl_uint,
      const cl_mem*,
      const void**,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueMarkerWithWaitList(
      cl_command_queue,
      cl_uint,
      const cl_event*,
      cl_event*);

  cl_int CallclEnqueueBarrierWithWaitList(
      cl_command_queue,
      cl_uint,
      const cl_event*,
      cl_event*);

  
  //ScalableVision


  cl_mem 
	  CallclCreateFromGLBuffer(cl_context     /* context */,
	  cl_mem_flags   /* flags */,
	  cl_GLuint      /* bufobj */,
	  int *          /* errcode_ret */) ;

  cl_mem 
	  CallclCreateFromGLTexture(cl_context      /* context */,
	  cl_mem_flags    /* flags */,
	  cl_GLenum       /* target */,
	  cl_GLint        /* miplevel */,
	  cl_GLuint       /* texture */,
	  cl_int *        /* errcode_ret */) ;

  cl_int
	  CallclEnqueueAcquireGLObjects(cl_command_queue      /* command_queue */,
	  cl_uint               /* num_objects */,
	  const cl_mem *        /* mem_objects */,
	  cl_uint               /* num_events_in_wait_list */,
	  const cl_event *      /* event_wait_list */,
	  cl_event *            /* event */);

  cl_int
	  CallclEnqueueReleaseGLObjects(cl_command_queue      /* command_queue */,
	  cl_uint               /* num_objects */,
	  const cl_mem *        /* mem_objects */,
	  cl_uint               /* num_events_in_wait_list */,
	  const cl_event *      /* event_wait_list */,
	  cl_event *            /* event */);
  DISALLOW_COPY_AND_ASSIGN(GpuChannelHost);
}; // GpuChannelHost end


// Globals

  //ScalableVision
  cl_mem 
	  CallclCreateFromGLBuffer(GpuChannelHost*,cl_context     /* context */,
	  cl_mem_flags   /* flags */,
	  cl_GLuint      /* bufobj */,
	  int *          /* errcode_ret */) ;

  cl_mem 
	  CallclCreateFromGLTexture(GpuChannelHost*,cl_context      /* context */,
	  cl_mem_flags    /* flags */,
	  cl_GLenum       /* target */,
	  cl_GLint        /* miplevel */,
	  cl_GLuint       /* texture */,
	  cl_int *        /* errcode_ret */) ;

  cl_int
	  CallclEnqueueAcquireGLObjects(GpuChannelHost*,cl_command_queue      /* command_queue */,
	  cl_uint               /* num_objects */,
	  const cl_mem *        /* mem_objects */,
	  cl_uint               /* num_events_in_wait_list */,
	  const cl_event *      /* event_wait_list */,
	  cl_event *            /* event */);

  cl_int
	  CallclEnqueueReleaseGLObjects(GpuChannelHost*,cl_command_queue      /* command_queue */,
	  cl_uint               /* num_objects */,
	  const cl_mem *        /* mem_objects */,
	  cl_uint               /* num_events_in_wait_list */,
	  const cl_event *      /* event_wait_list */,
	  cl_event *            /* event */);

  // ScalableVision end

cl_int CallclGetPlatformIDs(
    GpuChannelHost*,
    cl_uint,
    cl_platform_id*,
    cl_uint*);

cl_int CallclGetPlatformInfo(
    GpuChannelHost*,
    cl_platform_id,
    cl_platform_info,
    size_t,
    void*,
    size_t*);

cl_int CallclGetDeviceIDs(
    GpuChannelHost*,
    cl_platform_id,
    cl_device_type,
    cl_uint,
    cl_device_id*,
    cl_uint*);

cl_int CallclGetDeviceInfo(
    GpuChannelHost*,
    cl_device_id,
    cl_device_info,
    size_t,
    void*,
    size_t*);

cl_int CallclCreateSubDevices(
    GpuChannelHost*,
    cl_device_id,
    const cl_device_partition_property*,
    cl_uint,
    cl_device_id*,
    cl_uint*);

cl_int CallclRetainDevice (GpuChannelHost*,cl_device_id);

cl_int CallclReleaseDevice (GpuChannelHost*,cl_device_id);

cl_context CallclCreateContext(
    /*GpuChannelHost*,*/
    const cl_context_properties*,
    cl_uint,
    const cl_device_id*,
    void (CL_CALLBACK*)(const char*, const void*, size_t, void*),
    void*,
    cl_int*);

cl_context CallclCreateContextFromType(
    GpuChannelHost*,
    const cl_context_properties*,
    cl_device_type,
    void (CL_CALLBACK*)(const char*, const void*, size_t, void*),
    void*,
    cl_int*);

cl_int CallclRetainContext(GpuChannelHost*, cl_context);

cl_int CallclReleaseContext(GpuChannelHost*, cl_context);

cl_int CallclGetContextInfo(
    GpuChannelHost*,
    cl_context,
    cl_context_info,
    size_t,
    void*,
    size_t*);

cl_command_queue CallclCreateCommandQueue(
    GpuChannelHost*,
    cl_context,
    cl_device_id,
    cl_command_queue_properties,
    cl_int*);

cl_int CallclRetainCommandQueue(GpuChannelHost*, cl_command_queue);

cl_int CallclReleaseCommandQueue(GpuChannelHost*, cl_command_queue);

cl_int CallclGetCommandQueueInfo(
    GpuChannelHost*,
    cl_command_queue,
    cl_command_queue_info,
    size_t,
    void*,
    size_t*);

cl_mem CallclCreateBuffer(
    GpuChannelHost*,
    cl_context,
    cl_mem_flags,
    size_t,
    void*,
    cl_int*);

cl_mem CallclCreateSubBuffer(
    GpuChannelHost*,
    cl_mem,
    cl_mem_flags,
    cl_buffer_create_type,
    const void*,
    cl_int*);

cl_mem CallclCreateImage(
    GpuChannelHost*,
    cl_context,
    cl_mem_flags,
    const cl_image_format*,
    const cl_image_desc*,
    void*,
    cl_int*);

cl_int CallclRetainMemObject(GpuChannelHost*, cl_mem);

cl_int CallclReleaseMemObject(GpuChannelHost*,cl_mem);

cl_int CallclGetSupportedImageFormats(
    GpuChannelHost*,
    cl_context,
    cl_mem_flags,
    cl_mem_object_type,
    cl_uint,
    cl_image_format*,
    cl_uint*);

cl_int CallclGetMemObjectInfo(
    GpuChannelHost*,
    cl_mem,
    cl_mem_info,
    size_t,
    void*,
    size_t*);

cl_int CallclGetImageInfo(
    GpuChannelHost*,
    cl_mem,
    cl_image_info,
    size_t,
    void*,
    size_t*);

cl_int CallclSetMemObjectDestructorCallback(
    GpuChannelHost*,
    cl_mem,
    void (CL_CALLBACK*)(cl_mem,void*),
    void*);

cl_sampler CallclCreateSampler(
    GpuChannelHost*,
    cl_context,
    cl_bool,
    cl_addressing_mode,
    cl_filter_mode,
    cl_int*);

cl_int CallclRetainSampler(GpuChannelHost*,cl_sampler);

cl_int CallclReleaseSampler(GpuChannelHost*,cl_sampler);

cl_int CallclGetSamplerInfo(
    GpuChannelHost*,
    cl_sampler,
    cl_sampler_info,
    size_t,
    void*,
    size_t*);

cl_program CallclCreateProgramWithSource(
    GpuChannelHost*,
    cl_context,
    cl_uint,
    const char**,
    const size_t*,
    cl_int*);

cl_program CallclCreateProgramWithBinary(
    GpuChannelHost*,
    cl_context,
    cl_uint,
    const cl_device_id*,
    const size_t*,
    const unsigned char**,
    cl_int*,
    cl_int*);

cl_program CallclCreateProgramWithBuiltInKernels(
    GpuChannelHost*,
    cl_context,
    cl_uint,
    const cl_device_id*,
    const char*,
    cl_int*);

cl_int CallclRetainProgram(GpuChannelHost*, cl_program);

cl_int CallclReleaseProgram(GpuChannelHost*, cl_program);

cl_int CallclBuildProgram(
    GpuChannelHost*,
    cl_program,
    cl_uint,
    const cl_device_id*,
    const char*,
    void (CL_CALLBACK*)(cl_program, void*),
    void*);

cl_int CallclCompileProgram(
    GpuChannelHost*,
    cl_program,
    cl_uint,
    const cl_device_id*,
    const char*,
    cl_uint,
    const cl_program*,
    const char**,
    void (CL_CALLBACK*)(cl_program, void*),
    void*);

cl_program CallclLinkProgram(
    GpuChannelHost*,
    cl_context,
    cl_uint,
    const cl_device_id*,
    const char*,
    cl_uint,
    const cl_program*,
    void (CL_CALLBACK*)(cl_program, void*),
    void*, cl_int*);

cl_int CallclUnloadPlatformCompiler(GpuChannelHost*,cl_platform_id);

cl_int CallclGetProgramInfo(
    GpuChannelHost*,
    cl_program,
    cl_program_info,
    size_t,
    void*,
    size_t*);

cl_int CallclGetProgramBuildInfo(
    GpuChannelHost*,
    cl_program,
    cl_device_id,
    cl_program_build_info,
    size_t,
    void*,
    size_t*);

cl_kernel CallclCreateKernel(
    GpuChannelHost*,
    cl_program,
    const char*,
    cl_int*);

cl_int CallclCreateKernelsInProgram(
    GpuChannelHost*,
    cl_program,
    cl_uint,
    cl_kernel*,
    cl_uint*);

cl_int CallclRetainKernel(GpuChannelHost*, cl_kernel);

cl_int CallclReleaseKernel(GpuChannelHost*, cl_kernel);

cl_int CallclSetKernelArg(
    GpuChannelHost*,
    cl_kernel,
    cl_uint,
    size_t,
    const void*);
cl_int CallclSetKernelArg_vector(
    GpuChannelHost*,
    cl_kernel,
    cl_uint,
    size_t,
    const void*);

cl_int CallclGetKernelInfo(
    GpuChannelHost*,
    cl_kernel,
    cl_kernel_info,
    size_t,
    void*,
    size_t*);

cl_int CallclGetKernelArgInfo(
    GpuChannelHost*,
    cl_kernel,
    cl_uint,
    cl_kernel_arg_info,
    size_t,
    void*,
    size_t*);

cl_int CallclGetKernelWorkGroupInfo(
    GpuChannelHost*,
    cl_kernel,
    cl_device_id,
    cl_kernel_work_group_info,
    size_t,
    void*,
    size_t*);

cl_int CallclWaitForEvents(
    GpuChannelHost*,
    cl_uint,
    const cl_event*);

cl_int CallclGetEventInfo(
    GpuChannelHost*,
    cl_event,
    cl_event_info,
    size_t,
    void*,
    size_t*);

cl_event CallclCreateUserEvent(
    GpuChannelHost*,
    cl_context,
    cl_int*);

cl_int CallclRetainEvent(
    GpuChannelHost*,
    cl_event);

cl_int CallclReleaseEvent(GpuChannelHost*, cl_event);

cl_int CallclSetUserEventStatus(
    GpuChannelHost*,
    cl_event,
    cl_int);

cl_int CallclSetEventCallback(
    GpuChannelHost*,
    cl_event,
    cl_int,
    void (CL_CALLBACK*)(cl_event, cl_int, void*),
    void*);

cl_int CallclGetEventProfilingInfo(
    GpuChannelHost*,
    cl_event,
    cl_profiling_info,
    size_t,
    void*,
    size_t*);

cl_int CallclFlush(GpuChannelHost*, cl_command_queue);

cl_int CallclFinish (GpuChannelHost*, cl_command_queue);

cl_int CallclEnqueueReadBuffer(
    GpuChannelHost*, 
    cl_command_queue,
    cl_mem,
    cl_bool,
    size_t,
    size_t,
    void *, 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueReadBufferRect(
    GpuChannelHost*,
    cl_command_queue , 
    cl_mem ,
    cl_bool , 
    const size_t *,
    const size_t *,
    const size_t *,
    size_t ,
    size_t ,
    size_t , 
    size_t ,
    void *, 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueWriteBuffer(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_bool ,
    size_t ,
    size_t ,
    const void *, 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueWriteBufferRect(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_bool ,
    const size_t *, 
    const size_t *,
    const size_t *,
    size_t ,
    size_t , 
    size_t ,
    size_t ,
    const void *,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueFillBuffer(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    const void *,
    size_t ,
    size_t , 
    size_t ,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueCopyBuffer(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    cl_mem ,
    size_t , 
    size_t , 
    size_t ,
    cl_uint ,
    const cl_event *, 
    cl_event * );

cl_int CallclEnqueueCopyBufferRect(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    cl_mem ,
    const size_t *,
    const size_t *,
    const size_t *,
    size_t ,
    size_t ,
    size_t ,
    size_t ,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueReadImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    cl_bool ,
    const size_t *,
    const size_t *,
    size_t , 
    size_t ,
    void *,
    cl_uint , 
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueWriteImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_bool ,
    const size_t *,
    const size_t *,
    size_t ,
    size_t ,
    const void *, 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueFillImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    const void *,
    const size_t *,
    const size_t *,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueCopyImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_mem ,
    const size_t *,
    const size_t *,
    const size_t *,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueCopyImageToBuffer(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    cl_mem ,
    const size_t *, 
    const size_t *,
    size_t , 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueCopyBufferToImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    cl_mem ,
    size_t ,
    const size_t *, 
    const size_t *,
    cl_uint ,
    const cl_event *,
    cl_event * );

void *CallclEnqueueMapBuffer(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_bool ,
    cl_map_flags ,
    size_t , 
    size_t , 
    cl_uint ,
    const cl_event *,
    cl_event * ,
    cl_int *);

void *CallclEnqueueMapImage(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem ,
    cl_bool ,
    cl_map_flags ,
    const size_t *, 
    const size_t *,
    size_t *,
    size_t *,
    cl_uint ,
    const cl_event *,
    cl_event * ,
    cl_int *);

cl_int CallclEnqueueUnmapMemObject(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_mem , 
    void *,
    cl_uint ,
    const cl_event *, 
    cl_event * );

cl_int CallclEnqueueMigrateMemObjects(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_uint ,
    const cl_mem *,
    cl_mem_migration_flags ,
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueNDRangeKernel(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_kernel ,
	  cl_uint work_dim,
    const size_t *,
    const size_t *,
    const size_t *,
    cl_uint ,
    const cl_event *, 
    cl_event * );

cl_int CallclEnqueueTask(
    GpuChannelHost* ,
    cl_command_queue ,
    cl_kernel , 
    cl_uint ,
    const cl_event *,
    cl_event * );

cl_int CallclEnqueueNativeKernel(
    GpuChannelHost* ,
    cl_command_queue , 
    void (CL_CALLBACK* user_func)(void*),
    void*,
    size_t,
    cl_uint, 
    const cl_mem*,
    const void**, 
    cl_uint,
    const cl_event*,
    cl_event* );

cl_int CallclEnqueueMarkerWithWaitList(
    GpuChannelHost*,
    cl_command_queue,
    cl_uint,
    const cl_event *, 
    cl_event* );

cl_int CallclEnqueueBarrierWithWaitList(
    GpuChannelHost*,
    cl_command_queue,
    cl_uint,
    const cl_event*,
    cl_event*);

}  // namespace content

#endif  // CONTENT_COMMON_GPU_CLIENT_GPU_CHANNEL_HOST_H_
