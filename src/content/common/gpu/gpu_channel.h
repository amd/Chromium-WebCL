// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_GPU_CHANNEL_H_
#define CONTENT_COMMON_GPU_GPU_CHANNEL_H_

#include <deque>
#include <string>

#include "base/id_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "base/process/process.h"
#include "build/build_config.h"
#include "content/common/gpu/gpu_command_buffer_stub.h"
#include "content/common/gpu/gpu_memory_manager.h"
#include "content/common/message_router.h"
#include "ipc/ipc_sync_channel.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/size.h"
#include "ui/gl/gl_share_group.h"
#include "ui/gl/gpu_preference.h"

#if defined(OS_ANDROID)
#include "content/common/android/surface_texture_peer.h"
#endif

#if defined(OS_WIN)
#include <CL/OpenCL.h>
#endif

#define cl_point uint32

struct GPUCreateCommandBufferConfig;

namespace base {
class MessageLoopProxy;
class WaitableEvent;
}

namespace gpu {
class PreemptionFlag;
namespace gles2 {
class ImageManager;
}
}

#if defined(OS_ANDROID)
namespace content {
class StreamTextureManagerAndroid;
}
#endif

namespace content {
class GpuChannelManager;
class GpuChannelMessageFilter;
struct GpuRenderingStats;
class GpuVideoEncodeAccelerator;
class GpuWatchdog;

// Encapsulates an IPC channel between the GPU process and one renderer
// process. On the renderer side there's a corresponding GpuChannelHost.
class GpuChannel : public IPC::Listener,
                   public IPC::Sender,
                   public base::RefCountedThreadSafe<GpuChannel> {
 public:
  // Takes ownership of the renderer process handle.
  GpuChannel(GpuChannelManager* gpu_channel_manager,
             GpuWatchdog* watchdog,
             gfx::GLShareGroup* share_group,
             gpu::gles2::MailboxManager* mailbox_manager,
             int client_id,
             bool software);

  bool Init(base::MessageLoopProxy* io_message_loop,
            base::WaitableEvent* shutdown_event);

  // Get the GpuChannelManager that owns this channel.
  GpuChannelManager* gpu_channel_manager() const {
    return gpu_channel_manager_;
  }

  // Returns the name of the associated IPC channel.
  std::string GetChannelName();

#if defined(OS_POSIX)
  int TakeRendererFileDescriptor();
#endif  // defined(OS_POSIX)

  base::ProcessId renderer_pid() const { return channel_->peer_pid(); }

  scoped_refptr<base::MessageLoopProxy> io_message_loop() const {
    return io_message_loop_;
  }

  // IPC::Listener implementation:
  virtual bool OnMessageReceived(const IPC::Message& msg) OVERRIDE;
  virtual void OnChannelError() OVERRIDE;

  // IPC::Sender implementation:
  virtual bool Send(IPC::Message* msg) OVERRIDE;

  // Requeue the message that is currently being processed to the beginning of
  // the queue. Used when the processing of a message gets aborted because of
  // unscheduling conditions.
  void RequeueMessage();

  // This is called when a command buffer transitions from the unscheduled
  // state to the scheduled state, which potentially means the channel
  // transitions from the unscheduled to the scheduled state. When this occurs
  // deferred IPC messaged are handled.
  void OnScheduled();

  // This is called when a command buffer transitions between scheduled and
  // descheduled states. When any stub is descheduled, we stop preempting
  // other channels.
  void StubSchedulingChanged(bool scheduled);

  void CreateViewCommandBuffer(
      const gfx::GLSurfaceHandle& window,
      int32 surface_id,
      const GPUCreateCommandBufferConfig& init_params,
      int32* route_id);

  void CreateImage(
      gfx::PluginWindowHandle window,
      int32 image_id,
      gfx::Size* size);
  void DeleteImage(int32 image_id);

  gfx::GLShareGroup* share_group() const { return share_group_.get(); }

  GpuCommandBufferStub* LookupCommandBuffer(int32 route_id);

  void LoseAllContexts();
  void MarkAllContextsLost();

  // Destroy channel and all contained contexts.
  void DestroySoon();

  // Generate a route ID guaranteed to be unique for this channel.
  int GenerateRouteID();

  // Called to add/remove a listener for a particular message routing ID.
  void AddRoute(int32 route_id, IPC::Listener* listener);
  void RemoveRoute(int32 route_id);

  gpu::PreemptionFlag* GetPreemptionFlag();

  bool handle_messages_scheduled() const { return handle_messages_scheduled_; }
  uint64 messages_processed() const { return messages_processed_; }

  // If |preemption_flag->IsSet()|, any stub on this channel
  // should stop issuing GL commands. Setting this to NULL stops deferral.
  void SetPreemptByFlag(
      scoped_refptr<gpu::PreemptionFlag> preemption_flag);

#if defined(OS_ANDROID)
  StreamTextureManagerAndroid* stream_texture_manager() {
    return stream_texture_manager_.get();
  }
#endif

  void CacheShader(const std::string& key, const std::string& shader);

  void AddFilter(IPC::ChannelProxy::MessageFilter* filter);
  void RemoveFilter(IPC::ChannelProxy::MessageFilter* filter);

 protected:
  virtual ~GpuChannel();

 private:
  friend class base::RefCountedThreadSafe<GpuChannel>;
  friend class GpuChannelMessageFilter;

  void OnDestroy();

  bool OnControlMessageReceived(const IPC::Message& msg);

  void HandleMessage();

  // Message handlers.
  void OnCreateOffscreenCommandBuffer(
      const gfx::Size& size,
      const GPUCreateCommandBufferConfig& init_params,
      int32* route_id);
  void OnDestroyCommandBuffer(int32 route_id);
  void OnCreateVideoEncoder(int32* route_id);
  void OnDestroyVideoEncoder(int32 route_id);

#if defined(OS_ANDROID)
  // Register the StreamTextureProxy class with the gpu process so that all
  // the callbacks will be correctly forwarded to the renderer.
  void OnRegisterStreamTextureProxy(int32 stream_id, int32* route_id);

  // Create a java surface texture object and send it to the renderer process
  // through binder thread.
  void OnEstablishStreamTexture(
      int32 stream_id, int32 primary_id, int32 secondary_id);

  // Set the size of StreamTexture.
  void OnSetStreamTextureSize(int32 stream_id, const gfx::Size& size);
#endif

  // Collect rendering stats.
  void OnCollectRenderingStatsForSurface(
      int32 surface_id, GpuRenderingStats* stats);

  // Decrement the count of unhandled IPC messages and defer preemption.
  void MessageProcessed();

  // The lifetime of objects of this class is managed by a GpuChannelManager.
  // The GpuChannelManager destroy all the GpuChannels that they own when they
  // are destroyed. So a raw pointer is safe.
  GpuChannelManager* gpu_channel_manager_;

  scoped_ptr<IPC::SyncChannel> channel_;

  uint64 messages_processed_;

  // Whether the processing of IPCs on this channel is stalled and we should
  // preempt other GpuChannels.
  scoped_refptr<gpu::PreemptionFlag> preempting_flag_;

  // If non-NULL, all stubs on this channel should stop processing GL
  // commands (via their GpuScheduler) when preempted_flag_->IsSet()
  scoped_refptr<gpu::PreemptionFlag> preempted_flag_;

  std::deque<IPC::Message*> deferred_messages_;

  // The id of the client who is on the other side of the channel.
  int client_id_;

  // Uniquely identifies the channel within this GPU process.
  std::string channel_id_;

  // Used to implement message routing functionality to CommandBuffer objects
  MessageRouter router_;

  // The share group that all contexts associated with a particular renderer
  // process use.
  scoped_refptr<gfx::GLShareGroup> share_group_;

  scoped_refptr<gpu::gles2::MailboxManager> mailbox_manager_;
  scoped_refptr<gpu::gles2::ImageManager> image_manager_;
#if defined(OS_ANDROID)
  scoped_ptr<StreamTextureManagerAndroid> stream_texture_manager_;
#endif

  typedef IDMap<GpuCommandBufferStub, IDMapOwnPointer> StubMap;
  StubMap stubs_;

  typedef IDMap<GpuVideoEncodeAccelerator, IDMapOwnPointer> EncoderMap;
  EncoderMap video_encoders_;

  bool log_messages_;  // True if we should log sent and received messages.
  gpu::gles2::DisallowedFeatures disallowed_features_;
  GpuWatchdog* watchdog_;
  bool software_;
  bool handle_messages_scheduled_;
  bool processed_get_state_fast_;
  IPC::Message* currently_processing_message_;

  base::WeakPtrFactory<GpuChannel> weak_factory_;

  scoped_refptr<GpuChannelMessageFilter> filter_;
  scoped_refptr<base::MessageLoopProxy> io_message_loop_;

  size_t num_stubs_descheduled_;

  DISALLOW_COPY_AND_ASSIGN(GpuChannel);

  GpuCommandBufferStub* current_stub_;

 public:
  // Handle the OpenCL API calling.
  void OnCallclGetPlatformIDs(
      const cl_uint&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      cl_uint*,
      cl_int*);

  void OnCallclGetDeviceIDs(
      const cl_point&,
      const cl_device_type&,
      const cl_uint&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      cl_uint*,
      cl_int*);

  void OnCallclCreateSubDevices(
      const cl_point&,
      const std::vector<cl_device_partition_property>&,
      const cl_uint&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      cl_uint*,
      cl_int*);

  void OnCallclRetainDevice(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseDevice(
      const cl_point&,
      cl_int*);

  void OnCallclCreateContext(
      const std::vector<cl_context_properties>&,
      const cl_uint&,
      const std::vector<cl_point>&,
      const std::vector<cl_point>&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateContextFromType(
      const std::vector<cl_context_properties>&,
      const cl_device_type&,
      const cl_point&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclRetainContext(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseContext(
      const cl_point&,
      cl_int*);

  void OnCallclCreateCommandQueue(
      const cl_point&,
      const cl_point&,
      const cl_command_queue_properties&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclRetainCommandQueue(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseCommandQueue(
      const cl_point&,
      cl_int*);

  void OnCallclCreateBuffer(
      const cl_point&,
      const cl_mem_flags&,
      const size_t&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateSubBuffer(
      const cl_point&,
      const cl_mem_flags&,
      const cl_buffer_create_type&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateImage(
      const cl_point&,
      const cl_mem_flags&,
      const std::vector<cl_uint>&,
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclRetainMemObject(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseMemObject(
      const cl_point&,
      cl_int*);

  void OnCallclGetSupportedImageFormats(
      const cl_point&,
      const cl_mem_flags&,
      const cl_mem_object_type&,
      const cl_uint&,
      const std::vector<bool>&,
      std::vector<cl_uint>*,
      cl_uint*,
      cl_int*);

  void OnCallclSetMemObjectDestructorCallback(
      const cl_point&,
      const cl_point&,
      const cl_point&,
      cl_int*);

  void OnCallclCreateSampler(
      const cl_point&,
      const cl_bool&,
      const cl_addressing_mode&,
      const cl_filter_mode&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclRetainSampler(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseSampler(
      const cl_point&,
      cl_int*);

  void OnCallclCreateProgramWithSource(
      const cl_point&,
      const cl_uint&,
      const std::vector<std::string>&,
      const std::vector<size_t>&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateProgramWithBinary(
      const cl_point&,
      const cl_uint&,
      const std::vector<cl_point>&,
      const std::vector<size_t>&,
      const std::vector<std::vector<unsigned char>>&,
      std::vector<cl_int>*,
      cl_int*,
      cl_point*);

  void OnCallclCreateProgramWithBuiltInKernels(
      const cl_point&,
      const cl_uint&,
      const std::vector<cl_point>&,
      const std::string&,
      cl_int*,
      cl_point*);

  void OnCallclRetainProgram(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseProgram(
      const cl_point&,
      cl_int*);

  void OnCallclBuildProgram(
      const cl_point&,
      const cl_uint&,
      const std::vector<cl_point>&,
      const std::string&,
      const std::vector<cl_point>&,
      cl_int*);

  void OnCallclCompileProgram(
      const std::vector<cl_point>&,
      const std::vector<cl_uint>&,
      const std::vector<cl_point>&,
      const std::vector<std::string>&,
      const std::vector<cl_point>&,
      cl_int*);

  void OnCallclLinkProgram(
      const std::vector<cl_point>&,
      const std::vector<cl_uint>&,
      const std::vector<cl_point>&,
      const std::vector<cl_point>&,
      const std::string&,
      cl_int*,
      cl_point*);

  void OnCallclUnloadPlatformCompiler(
      const cl_point&,
      cl_int*);

  void OnCallclCreateKernel(
      const cl_point&,
      const std::string&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclCreateKernelsInProgram(
      const cl_point&,
      const cl_uint&,
      const std::vector<cl_point>&,
      const std::vector<bool>&,
      cl_uint*,
      cl_int*);

  void OnCallclRetainKernel(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseKernel(
      const cl_point&,
      cl_int*);

  void OnCallclSetKernelArg(
      const cl_point&,
      const cl_uint&,
      const size_t&,
      const cl_point&,
      cl_int*);
  void OnCallclSetKernelArg_vector(
      const cl_point&,
      const cl_uint&,
      const std::vector<unsigned char>&,
      cl_int*);

  void OnCallclWaitForEvents(
      const cl_uint&,
      const std::vector<cl_point>&,
      cl_int*);

  void OnCallclCreateUserEvent(
      const cl_point&,
      const std::vector<bool>&,
      cl_int*,
      cl_point*);

  void OnCallclRetainEvent(
      const cl_point&,
      cl_int*);

  void OnCallclReleaseEvent(
      const cl_point&,
      cl_int*);

  void OnCallclSetUserEventStatus(
      const cl_point&,
      const cl_int&,
      cl_int*);

  void OnCallclSetEventCallback(
      const cl_point&,
      const cl_int&,
      const cl_point&,
      const cl_point&,
      cl_int*);

  void OnCallclFlush(
      const cl_point&,
      cl_int*);

  void OnCallclFinish(
      const cl_point&,
      cl_int*);

  void OnCallclGetPlatformInfo_string(
      const cl_point&,
      const cl_platform_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_cl_uint(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_size_t_list(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<size_t>*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_size_t(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_cl_ulong(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*); 

  void OnCallclGetDeviceInfo_string(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_cl_point(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetDeviceInfo_intptr_t_list(
      const cl_point&,
      const cl_device_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<intptr_t>*,
      size_t*,
      cl_int*); 

  void OnCallclGetContextInfo_cl_uint(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetContextInfo_cl_point_list(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      size_t*,
      cl_int*);

  void OnCallclGetContextInfo_intptr_t_list(
      const cl_point&,
      const cl_context_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<intptr_t>*,
      size_t*,
      cl_int*);

  void OnCallclGetCommandQueueInfo_cl_point(
      const cl_point&,
      const cl_command_queue_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetCommandQueueInfo_cl_uint(
      const cl_point&,
      const cl_command_queue_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetCommandQueueInfo_cl_ulong(
      const cl_point&,
      const cl_command_queue_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclGetMemObjectInfo_cl_uint(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetMemObjectInfo_cl_ulong(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclGetMemObjectInfo_size_t(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetMemObjectInfo_cl_point(
      const cl_point&,
      const cl_mem_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetImageInfo_cl_image_format(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<cl_uint>*,
      size_t*,
      cl_int*);

  void OnCallclGetImageInfo_size_t(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetImageInfo_cl_point(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetImageInfo_cl_uint(
      const cl_point&,
      const cl_image_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetSamplerInfo_cl_uint(
      const cl_point&,
      const cl_sampler_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetSamplerInfo_cl_point(
      const cl_point&,
      const cl_sampler_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_cl_uint(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_cl_point(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_cl_point_list(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<cl_point>*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_string(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_size_t_list(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<size_t>*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_string_list(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<std::string>*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramInfo_size_t(
      const cl_point&,
      const cl_program_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramBuildInfo_cl_int(
      const cl_point&,
      const cl_point&,
      const cl_program_build_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_int*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramBuildInfo_string(
      const cl_point&,
      const cl_point&,
      const cl_program_build_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetProgramBuildInfo_cl_uint(
      const cl_point&,
      const cl_point&,
      const cl_program_build_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelInfo_string(
      const cl_point&,
      const cl_kernel_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelInfo_cl_uint(
      const cl_point&,
      const cl_kernel_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelInfo_cl_point(
      const cl_point&,
      const cl_kernel_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelArgInfo_cl_uint(
      const cl_point&,
      const cl_uint&,
      const cl_kernel_arg_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelArgInfo_string(
      const cl_point&,
      const cl_uint&,
      const cl_kernel_arg_info&,
      const size_t&,
      const std::vector<bool>&,
      std::string*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelArgInfo_cl_ulong(
      const cl_point&,
      const cl_uint&,
      const cl_kernel_arg_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelWorkGroupInfo_size_t_list(
      const cl_point&,
      const cl_point&,
      const cl_kernel_work_group_info&,
      const size_t&,
      const std::vector<bool>&,
      std::vector<size_t>*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelWorkGroupInfo_size_t(
      const cl_point&,
      const cl_point&,
      const cl_kernel_work_group_info&,
      const size_t&,
      const std::vector<bool>&,
      size_t*,
      size_t*,
      cl_int*);

  void OnCallclGetKernelWorkGroupInfo_cl_ulong(
      const cl_point&,
      const cl_point&,
      const cl_kernel_work_group_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclGetEventInfo_cl_point(
      const cl_point&,
      const cl_event_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_point*,
      size_t*,
      cl_int*);

  void OnCallclGetEventInfo_cl_uint(
      const cl_point&,
      const cl_event_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_uint*,
      size_t*,
      cl_int*);

  void OnCallclGetEventInfo_cl_int(
      const cl_point&,
      const cl_event_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_int*,
      size_t*,
      cl_int*);

  void OnCallclGetEventProfilingInfo_cl_ulong(
      const cl_point&,
      const cl_profiling_info&,
      const size_t&,
      const std::vector<bool>&,
      cl_ulong*,
      size_t*,
      cl_int*);

  void OnCallclEnqueueReadBuffer(
      const std::vector<cl_point>&,
      const cl_bool&,
      const std::vector<size_t>&,
      const cl_uint&,
      std::vector<unsigned char>*,
      cl_point*,
      cl_int*);

  void OnCallclEnqueueWriteBuffer(
    const std::vector<cl_point>&,
    const cl_bool&,
    const std::vector<size_t>&,
    const std::vector<unsigned char>&,
    const cl_uint&,
    cl_point*,
    cl_int*);

  void OnCallclEnqueueNDRangeKernel(
    const std::vector<cl_point>&,
    cl_int,
    const std::vector<size_t>&,
    const std::vector<cl_point>&,
    cl_point*,
    cl_int*);

  void OnCallclEnqueueReadBufferRect             (std::vector<cl_point>, cl_bool, std::vector<size_t>, cl_point, cl_uint, cl_point*, cl_int*);
  void OnCallclEnqueueWriteBufferRect            (std::vector<cl_point>, cl_bool, std::vector<size_t>, cl_point, cl_uint, cl_point*, cl_int*);
  void OnCallclEnqueueFillBuffer                 (std::vector<cl_point>, cl_point, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueCopyBuffer                 (std::vector<cl_point>, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueCopyBufferRect             (std::vector<cl_point>, std::vector<size_t>, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueReadImage                  (std::vector<cl_point>, cl_bool, std::vector<size_t>, cl_point, cl_uint, cl_point*, cl_int*);
  void OnCallclEnqueueWriteImage                 (std::vector<cl_point>, cl_bool, std::vector<size_t>, cl_point, cl_uint, cl_point*, cl_int*);
  void OnCallclEnqueueFillImage                  (std::vector<cl_point>, cl_point, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueCopyImage                  (std::vector<cl_point>, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueCopyImageToBuffer          (std::vector<cl_point>, std::vector<size_t>, size_t, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueCopyBufferToImage          (std::vector<cl_point>, size_t, std::vector<size_t>, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueMapBuffer                  (std::vector<cl_point>, cl_bool, cl_map_flags, std::vector<size_t>, cl_point*, cl_int*, cl_point*);
  void OnCallclEnqueueMapImage                   (std::vector<cl_point>, cl_bool, cl_map_flags, std::vector<size_t>, cl_uint, cl_point*, cl_int*, cl_point*);
  void OnCallclEnqueueUnmapMemObject             (cl_point, cl_point, cl_point, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueMigrateMemObjects          (cl_point, std::vector<cl_uint>, std::vector<cl_point>, cl_mem_migration_flags, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueTask                       (cl_point, cl_point, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueNativeKernel               (std::vector<cl_point>, size_t, std::vector<cl_uint>, cl_point, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueMarkerWithWaitList         (cl_point, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);
  void OnCallclEnqueueBarrierWithWaitList        (cl_point, cl_uint, std::vector<cl_point>, cl_point*, cl_int*);

  void OnCallclCreateFromGLBuffer(
                            cl_point, // context
                            cl_uint, // flags
                            cl_uint, // bufobj
                            cl_int*,  // errcode_ret
							cl_point*);

  void OnCallclCreateFromGLTexture(
							cl_point      /* context */,
							cl_uint    /* flags */,
							cl_uint       /* target */,
							cl_int        /* miplevel */,
							cl_uint       /* texture */,
							cl_int*        /* errcode_ret */,
							cl_point* /* func_ret */
							);

  void OnCallclEnqueueAcquireGLObjects(
							cl_point, // cmdqueue
							std::vector<cl_point>, // mem_objects
							std::vector<cl_point>, // event_wait_list
							cl_point*, // event ret
							cl_int* // func_ret
							);

  void OnCallclEnqueueReleaseGLObjects(
							cl_point, // cmdqueue
							std::vector<cl_point>, // mem_objects
							std::vector<cl_point>, // event_wait_list
							cl_point*, // event ret
							cl_int* // func_ret
							);

#include "content/common/gpu/ocl_service_header.h"
  void OnCallhostPtrSize(cl_uint size);

};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_GPU_CHANNEL_H_
