// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/media/vpx_video_decode_accelerator.h"

#if !defined(OS_WIN)
#error This file should only be built on Windows.
#endif   // !defined(OS_WIN)

#include <ks.h>
#include <codecapi.h>
#include <mfapi.h>
#include <mferror.h>
#include <wmcodecdsp.h>

#include "base/bind.h"
#include "base/callback.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "base/memory/scoped_handle.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "base/message_loop/message_loop.h"
#include "base/win/windows_version.h"
#include "media/video/video_decode_accelerator.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_surface_egl.h"
#include "ui/gl/gl_switches.h"

// Include libvpx header files.
// VPX_CODEC_DISABLE_COMPAT excludes parts of the libvpx API that provide
// backwards compatibility for legacy applications using the library.
#define VPX_CODEC_DISABLE_COMPAT 1
extern "C" {
#include "third_party/libvpx/source/libvpx/vpx/vpx_decoder.h"
#include "third_party/libvpx/source/libvpx/vpx/vp8dx.h"
typedef struct Interop_Context {
  void* pSharedHandle; //HANDLE
  void* pDevice;       //LPDIRECT3DDEVICE9EX
  void* pSurface;      //LPDIRECT3DSURFACE9
}Interop_Context;


}

#include <windows.h>
 
 typedef struct {
     LARGE_INTEGER start;
     LARGE_INTEGER stop;
 } stopWatch;
 
 class CStopWatch {
 
 private:
     stopWatch timer;
     LARGE_INTEGER frequency;
     double LIToSecs( LARGE_INTEGER & L) ;
 public:
     CStopWatch() ;
     void startTimer( ) ;
     void stopTimer( ) ;
     double getElapsedTime() ;
 };
 
 double CStopWatch::LIToSecs( LARGE_INTEGER & L) {
     return ((double)L.QuadPart /(double)frequency.QuadPart) ;
 }
 
 CStopWatch::CStopWatch(){
     timer.start.QuadPart=0;
     timer.stop.QuadPart=0; 
     QueryPerformanceFrequency( &frequency ) ;
 }
 
 void CStopWatch::startTimer( ) {
     QueryPerformanceCounter(&timer.start) ;
 }
 
 void CStopWatch::stopTimer( ) {
     QueryPerformanceCounter(&timer.stop) ;
 }
 
 double CStopWatch::getElapsedTime() {
     LARGE_INTEGER time;
     time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
     return LIToSecs( time) ;
 }
 

namespace content {

// We only request 5 picture buffers from the client which are used to hold the
// decoded samples. These buffers are then reused when the client tells us that
// it is done with the buffer.
static const int kNumPictureBuffers = 9;

bool VPXVideoDecodeAccelerator::pre_sandbox_init_done_ = false;
uint32 VPXVideoDecodeAccelerator::dev_manager_reset_token_ = 0;
//IDirect3DDeviceManager9* VPXVideoDecodeAccelerator::device_manager_ = NULL;
IDirect3DDevice9Ex* VPXVideoDecodeAccelerator::device_ = NULL;
IDirect3DQuery9* VPXVideoDecodeAccelerator::query_ = NULL;
IDirect3D9Ex* VPXVideoDecodeAccelerator::d3d9_ = NULL;

#define RETURN_ON_FAILURE(result, log, ret)  \
  do {                                       \
    if (!(result)) {                         \
      DLOG(ERROR) << log;                    \
      return ret;                            \
    }                                        \
  } while (0)

#define RETURN_ON_HR_FAILURE(result, log, ret)                    \
  RETURN_ON_FAILURE(SUCCEEDED(result),                            \
                    log << ", HRESULT: 0x" << std::hex << result, \
                    ret);

#define RETURN_AND_NOTIFY_ON_FAILURE(result, log, error_code, ret)  \
  do {                                                              \
    if (!(result)) {                                                \
      DVLOG(1) << log;                                              \
      StopOnError(error_code);                                      \
      return ret;                                                   \
    }                                                               \
  } while (0)

#define RETURN_AND_NOTIFY_ON_HR_FAILURE(result, log, error_code, ret)  \
  RETURN_AND_NOTIFY_ON_FAILURE(SUCCEEDED(result),                      \
                               log << ", HRESULT: 0x" << std::hex << result, \
                               error_code, ret);

// Maximum number of iterations we allow before aborting the attempt to flush
// the batched queries to the driver and allow torn/corrupt frames to be
// rendered.
enum { kMaxIterationsForD3DFlush = 10 };


static void InitSampleFromInputBuffer(VpxSample &sample,
    const media::BitstreamBuffer& bitstream_buffer,
    DWORD stream_size,
    DWORD alignment) {
  base::SharedMemory shm(bitstream_buffer.handle(), true);
  RETURN_ON_FAILURE(shm.Map(bitstream_buffer.size()),
                    "Failed in base::SharedMemory::Map", );

  sample.init(reinterpret_cast<const uint8*>(shm.memory()),
                           bitstream_buffer.size(), bitstream_buffer.id());
}

// Maintains information about a VPX picture buffer, i.e. whether it is
// available for rendering, the texture information, etc.
struct VPXVideoDecodeAccelerator::VPXPictureBuffer {
 public:
  static linked_ptr<VPXPictureBuffer> Create(
      const media::PictureBuffer& buffer, EGLConfig egl_config);
  ~VPXPictureBuffer();

  void ReusePictureBuffer();
  // Copies the output sample data to the picture buffer provided by the
  // client.
  // The dest_surface parameter contains the decoded bits.
  bool CopyOutputSampleDataToPictureBuffer(IDirect3DSurface9* dest_surface);

  bool available() const {
    return available_;
  }

  void set_available(bool available) {
    available_ = available;
  }

  int id() const {
    return picture_buffer_.id();
  }

  gfx::Size size() const {
    return picture_buffer_.size();
  }

 public:
  explicit VPXPictureBuffer(const media::PictureBuffer& buffer);

  bool available_;
  media::PictureBuffer picture_buffer_;
  EGLSurface decoding_surface_;
  base::win::ScopedComPtr<IDirect3DTexture9> decoding_texture_;
  // Set to true if RGB is supported by the texture.
  // Defaults to true.
  bool use_rgb_;

  DISALLOW_COPY_AND_ASSIGN(VPXPictureBuffer);
};

// static
linked_ptr<VPXVideoDecodeAccelerator::VPXPictureBuffer>
    VPXVideoDecodeAccelerator::VPXPictureBuffer::Create(
        const media::PictureBuffer& buffer, EGLConfig egl_config) {
  linked_ptr<VPXPictureBuffer> picture_buffer(new VPXPictureBuffer(buffer));

  EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
  EGLint use_rgb = 1;
  eglGetConfigAttrib(egl_display, egl_config, EGL_BIND_TO_TEXTURE_RGB,
                     &use_rgb);

  EGLint attrib_list[] = {
    EGL_WIDTH, buffer.size().width(),
    EGL_HEIGHT, buffer.size().height(),
    EGL_TEXTURE_FORMAT, use_rgb ? EGL_TEXTURE_RGB : EGL_TEXTURE_RGBA,
    EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
    EGL_NONE
  };

  picture_buffer->decoding_surface_ = eglCreatePbufferSurface(
      egl_display,
      egl_config,
      attrib_list);
  RETURN_ON_FAILURE(picture_buffer->decoding_surface_,
                    "Failed to create surface",
                    linked_ptr<VPXPictureBuffer>(NULL));

  HANDLE share_handle = NULL;
  EGLBoolean ret = eglQuerySurfacePointerANGLE(
      egl_display,
      picture_buffer->decoding_surface_,
      EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
      &share_handle);

  RETURN_ON_FAILURE(share_handle && ret == EGL_TRUE,
                    "Failed to query ANGLE surface pointer",
                    linked_ptr<VPXPictureBuffer>(NULL));

  HRESULT hr = VPXVideoDecodeAccelerator::device_->CreateTexture(
      buffer.size().width(),
      buffer.size().height(),
      1,
      D3DUSAGE_RENDERTARGET,
      use_rgb ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8,
      D3DPOOL_DEFAULT,
      picture_buffer->decoding_texture_.Receive(),
      &share_handle);

  RETURN_ON_HR_FAILURE(hr, "Failed to create texture",
                       linked_ptr<VPXPictureBuffer>(NULL));
  picture_buffer->use_rgb_ = !!use_rgb;

  return picture_buffer;
}

VPXVideoDecodeAccelerator::VPXPictureBuffer::VPXPictureBuffer(
    const media::PictureBuffer& buffer)
    : available_(true),
      picture_buffer_(buffer),
      decoding_surface_(NULL) {
}

VPXVideoDecodeAccelerator::VPXPictureBuffer::~VPXPictureBuffer() {
  if (decoding_surface_) {
    EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();

    eglReleaseTexImage(
        egl_display,
        decoding_surface_,
        EGL_BACK_BUFFER);

    eglDestroySurface(
        egl_display,
        decoding_surface_);
    decoding_surface_ = NULL;
  }
}

void VPXVideoDecodeAccelerator::VPXPictureBuffer::ReusePictureBuffer() {
  DCHECK(decoding_surface_);
  EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
  eglReleaseTexImage(
    egl_display,
    decoding_surface_,
    EGL_BACK_BUFFER);
  set_available(true);
}

bool VPXVideoDecodeAccelerator::VPXPictureBuffer::
    CopyOutputSampleDataToPictureBuffer(IDirect3DSurface9* dest_surface) {
  DCHECK(dest_surface);

  D3DSURFACE_DESC surface_desc;
  HRESULT hr = dest_surface->GetDesc(&surface_desc);
  RETURN_ON_HR_FAILURE(hr, "Failed to get surface description", false);

  D3DSURFACE_DESC texture_desc;
  decoding_texture_->GetLevelDesc(0, &texture_desc);

  if (texture_desc.Width != surface_desc.Width ||
      texture_desc.Height != surface_desc.Height) {
    NOTREACHED() << "Decode surface of different dimension than texture";
    return false;
  }

  //hr = d3d9_->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT,
  //                                        D3DDEVTYPE_HAL,
  //                                        surface_desc.Format,
  //                                        D3DFMT_X8R8G8B8);
  hr = d3d9_->CheckDeviceFormatConversion(
    D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, surface_desc.Format,
      use_rgb_ ? D3DFMT_X8R8G8B8 : D3DFMT_A8R8G8B8);

  bool device_supports_format_conversion = (hr == S_OK);

  RETURN_ON_FAILURE(device_supports_format_conversion,
                    "Device does not support format converision",
                    false);

  // This function currently executes in the context of IPC handlers in the
  // GPU process which ensures that there is always an OpenGL context.
  GLint current_texture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

  glBindTexture(GL_TEXTURE_2D, picture_buffer_.texture_id());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  base::win::ScopedComPtr<IDirect3DSurface9> d3d_surface;
  hr = decoding_texture_->GetSurfaceLevel(0, d3d_surface.Receive());
  RETURN_ON_HR_FAILURE(hr, "Failed to get surface from texture", false);

  CStopWatch timer;
  timer.startTimer();

  hr = device_->StretchRect(dest_surface,
    NULL,
    d3d_surface,
    NULL,
    D3DTEXF_NONE);


  RETURN_ON_HR_FAILURE(hr, "Colorspace conversion via StretchRect failed",
                        false);
#if 1
  // Ideally, this should be done immediately before the draw call that uses
  // the texture. Flush it once here though.
  hr = query_->Issue(D3DISSUE_END);
  RETURN_ON_HR_FAILURE(hr, "Failed to issue END", false);

  // The VPX decoder has its own device which it uses for decoding. ANGLE
  // has its own device which we don't have access to.
  // The above code attempts to copy the decoded picture into a surface
  // which is owned by ANGLE. As there are multiple devices involved in
  // this, the StretchRect call above is not synchronous.
  // We attempt to flush the batched operations to ensure that the picture is
  // copied to the surface owned by ANGLE.
  // We need to do this in a loop and call flush multiple times.
  // We have seen the GetData call for flushing the command buffer fail to
  // return success occassionally on multi core machines, leading to an
  // infinite loop.
  // Workaround is to have an upper limit of 10 on the number of iterations to
  // wait for the Flush to finish.
  int iterations = 0;
  while ((query_->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE) &&
          ++iterations < kMaxIterationsForD3DFlush) {
    LOG(ERROR) << "Sleep(1)";
    Sleep(1);  // Poor-man's Yield().
  }
  
  timer.stopTimer();
  double t = timer.getElapsedTime();
  LOG(ERROR) << "ANGLE flush time (secs): " << t;
#endif


  EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
  eglBindTexImage(
      egl_display,
      decoding_surface_,
      EGL_BACK_BUFFER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glBindTexture(GL_TEXTURE_2D, current_texture);
  return true;
}

VPXVideoDecodeAccelerator::PendingSampleInfo::PendingSampleInfo(
    int32 buffer_id, vpx_image_t* sample)
    : input_buffer_id(buffer_id), output_sample(sample) {
  //output_sample.Attach(sample);
}

VPXVideoDecodeAccelerator::PendingSampleInfo::~PendingSampleInfo() {}

// static
void VPXVideoDecodeAccelerator::PreSandboxInitialization() {
  // Should be called only once during program startup.
  DCHECK(!pre_sandbox_init_done_);

  RETURN_ON_FAILURE(CreateD3DDevManager(),
                    "Failed to initialize D3D device and manager",);
  /*
  if (base::win::GetVersion() == base::win::VERSION_WIN8) {
    // On Windows 8+ mf.dll forwards to mfcore.dll. It does not exist in
    // Windows 7. Loading mfcore.dll fails on Windows 8.1 in the
    // sandbox.
    if (!LoadLibrary(L"mfcore.dll")) {
      DLOG(ERROR) << "Failed to load mfcore.dll, Error: " << ::GetLastError();
      return;
    }
    // MFStartup needs to be called once outside the sandbox. It fails on
    // Windows 8.1 with E_NOTIMPL if it is called the first time in the
    // sandbox.
    RETURN_ON_HR_FAILURE(MFStartup(MF_VERSION, MFSTARTUP_FULL),
                         "MFStartup failed.",);
  }
  */

  pre_sandbox_init_done_ = true;
}

// static
bool VPXVideoDecodeAccelerator::CreateD3DDevManager() {
  HRESULT hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9_);
  RETURN_ON_HR_FAILURE(hr, "Direct3DCreate9Ex failed", false);

  D3DPRESENT_PARAMETERS present_params = {0};
  present_params.BackBufferWidth = 1;
  present_params.BackBufferHeight = 1;
  present_params.BackBufferFormat = D3DFMT_UNKNOWN;
  present_params.BackBufferCount = 1;
  present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
  present_params.hDeviceWindow = ::GetShellWindow();
  present_params.Windowed = TRUE;
  present_params.Flags = D3DPRESENTFLAG_VIDEO;
  present_params.FullScreen_RefreshRateInHz = 0;
  present_params.PresentationInterval = 0;

  hr = d3d9_->CreateDeviceEx(D3DADAPTER_DEFAULT,
                             D3DDEVTYPE_HAL,
                             ::GetShellWindow(),
                             D3DCREATE_FPU_PRESERVE |
                             //D3DCREATE_SOFTWARE_VERTEXPROCESSING |
                             D3DCREATE_HARDWARE_VERTEXPROCESSING |
                             D3DCREATE_DISABLE_PSGP_THREADING |
                             D3DCREATE_MULTITHREADED,
                             &present_params,
                             NULL,
                             &device_);
  RETURN_ON_HR_FAILURE(hr, "Failed to create D3D device", false);

  //hr = VPX2CreateDirect3DDeviceManager9(&dev_manager_reset_token_,
  //                                       &device_manager_);
  //RETURN_ON_HR_FAILURE(hr, "VPX2CreateDirect3DDeviceManager9 failed", false);

  //hr = device_manager_->ResetDevice(device_, dev_manager_reset_token_);
  //RETURN_ON_HR_FAILURE(hr, "Failed to reset device", false);

  hr = device_->CreateQuery(D3DQUERYTYPE_EVENT, &query_);
  RETURN_ON_HR_FAILURE(hr, "Failed to create D3D device query", false);
  
  // Ensure query_ API works (to avoid an infinite loop later in
  // CopyOutputSampleDataToPictureBuffer).
  hr = query_->Issue(D3DISSUE_END);
  RETURN_ON_HR_FAILURE(hr, "Failed to issue END test query", false);
  return true;
}

VPXVideoDecodeAccelerator::VPXVideoDecodeAccelerator(
    media::VideoDecodeAccelerator::Client* client,
    const base::Callback<bool(void)>& make_context_current)
    : client_(client),
      egl_config_(NULL),
      state_(kUninitialized),
      pictures_requested_(false),
      inputs_before_decode_(0),
      make_context_current_(make_context_current),
      temp_surface_yv12_(NULL),
      temp_surface_rgba_(NULL) {
  memset(&input_stream_info_, 0, sizeof(input_stream_info_));
  memset(&output_stream_info_, 0, sizeof(output_stream_info_));
}

VPXVideoDecodeAccelerator::~VPXVideoDecodeAccelerator() {
  client_ = NULL;
  if (temp_surface_rgba_)
    temp_surface_rgba_->Release();
  if (temp_surface_yv12_)
    temp_surface_yv12_->Release();
}

bool VPXVideoDecodeAccelerator::Initialize(media::VideoCodecProfile profile) {
  DCHECK(CalledOnValidThread());

  if (profile != media::VP9PROFILE_MIN &&
      profile != media::VP9PROFILE_MAIN) {
    RETURN_AND_NOTIFY_ON_FAILURE(false,
        "Unsupported vp9 profile", PLATFORM_FAILURE, false);
  }

  restart_ = true;


  RETURN_AND_NOTIFY_ON_FAILURE(pre_sandbox_init_done_,
      "PreSandbox initialization not completed", PLATFORM_FAILURE, false);

  RETURN_AND_NOTIFY_ON_FAILURE(
      gfx::g_driver_egl.ext.b_EGL_ANGLE_surface_d3d_texture_2d_share_handle,
      "EGL_ANGLE_surface_d3d_texture_2d_share_handle unavailable",
      PLATFORM_FAILURE,
      false);

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kUninitialized),
      "Initialize: invalid state: " << state_, ILLEGAL_STATE, false);


  RETURN_AND_NOTIFY_ON_FAILURE(InitDecoder(profile),
      "Failed to initialize decoder", PLATFORM_FAILURE, false);

  /*
  RETURN_AND_NOTIFY_ON_FAILURE(GetStreamsInfoAndBufferReqs(),
      "Failed to get input/output stream info.", PLATFORM_FAILURE, false);

  RETURN_AND_NOTIFY_ON_FAILURE(
      SendMFTMessage(MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0),
      "Send MFT_MESSAGE_NOTIFY_BEGIN_STREAMING notification failed",
      PLATFORM_FAILURE, false);

  RETURN_AND_NOTIFY_ON_FAILURE(
      SendMFTMessage(MFT_MESSAGE_NOTIFY_START_OF_STREAM, 0),
      "Send MFT_MESSAGE_NOTIFY_START_OF_STREAM notification failed",
      PLATFORM_FAILURE, false);
  */

  state_ = kNormal;
  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::NotifyInitializeDone,
      base::AsWeakPtr(this)));
  return true;
}

void VPXVideoDecodeAccelerator::Decode(
    const media::BitstreamBuffer& bitstream_buffer) {
  DCHECK(CalledOnValidThread());

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kNormal || state_ == kStopped ||
                                state_ == kFlushing),
      "Invalid state: " << state_, ILLEGAL_STATE,);

  
  CStopWatch timer;
  double t;
  timer.startTimer();

  VpxSample sample;
  InitSampleFromInputBuffer(sample, bitstream_buffer,
    input_stream_info_.cbSize,
    input_stream_info_.cbAlignment);

  //timer.stopTimer();
  //t = timer.getElapsedTime();
  //LOG(ERROR) << "InitSampleFromInputBuffer time (secs): " << t;

  //RETURN_AND_NOTIFY_ON_HR_FAILURE(sample->SetSampleTime(bitstream_buffer.id()),
  //    "Failed to associate input buffer id with sample", PLATFORM_FAILURE,);

  timer.startTimer();
  DecodeInternal(sample, false);
  timer.stopTimer();
  t = timer.getElapsedTime();
  LOG(ERROR) << "***Total decode+display time (secs): " << t;
}

void VPXVideoDecodeAccelerator::AssignPictureBuffers(
    const std::vector<media::PictureBuffer>& buffers) {
  DCHECK(CalledOnValidThread());

  RETURN_AND_NOTIFY_ON_FAILURE((state_ != kUninitialized),
      "Invalid state: " << state_, ILLEGAL_STATE,);
  RETURN_AND_NOTIFY_ON_FAILURE((kNumPictureBuffers == buffers.size()),
      "Failed to provide requested picture buffers. (Got " << buffers.size() <<
      ", requested " << kNumPictureBuffers << ")", INVALID_ARGUMENT,);

  // Copy the picture buffers provided by the client to the available list,
  // and mark these buffers as available for use.
  for (size_t buffer_index = 0; buffer_index < buffers.size();
       ++buffer_index) {
    linked_ptr<VPXPictureBuffer> picture_buffer =
        VPXPictureBuffer::Create(buffers[buffer_index], egl_config_);
    RETURN_AND_NOTIFY_ON_FAILURE(picture_buffer.get(),
        "Failed to allocate picture buffer", PLATFORM_FAILURE,);

    bool inserted = output_picture_buffers_.insert(std::make_pair(
        buffers[buffer_index].id(), picture_buffer)).second;
    DCHECK(inserted);
  }
  ProcessPendingSamples();
  if (state_ == kFlushing && pending_output_samples_.empty())
    FlushInternal();
}

void VPXVideoDecodeAccelerator::ReusePictureBuffer(
    int32 picture_buffer_id) {
  DCHECK(CalledOnValidThread());

  RETURN_AND_NOTIFY_ON_FAILURE((state_ != kUninitialized),
      "Invalid state: " << state_, ILLEGAL_STATE,);

  if (output_picture_buffers_.empty())
    return;

  OutputBuffers::iterator it = output_picture_buffers_.find(picture_buffer_id);
  RETURN_AND_NOTIFY_ON_FAILURE(it != output_picture_buffers_.end(),
      "Invalid picture id: " << picture_buffer_id, INVALID_ARGUMENT,);

  it->second->ReusePictureBuffer();
  ProcessPendingSamples();

  if (state_ == kFlushing && pending_output_samples_.empty())
    FlushInternal();
}

void VPXVideoDecodeAccelerator::Flush() {
  DCHECK(CalledOnValidThread());

  DVLOG(1) << "VPXVideoDecodeAccelerator::Flush";

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kNormal || state_ == kStopped),
      "Unexpected decoder state: " << state_, ILLEGAL_STATE,);

  state_ = kFlushing;

  //RETURN_AND_NOTIFY_ON_FAILURE(SendMFTMessage(MFT_MESSAGE_COMMAND_DRAIN, 0),
  //    "Failed to send drain message", PLATFORM_FAILURE,);

  if (!pending_output_samples_.empty())
    return;

  FlushInternal();
}

void VPXVideoDecodeAccelerator::Reset() {
  DCHECK(CalledOnValidThread());

  DVLOG(1) << "VPXVideoDecodeAccelerator::Reset";

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kNormal || state_ == kStopped),
      "Reset: invalid state: " << state_, ILLEGAL_STATE,);

  state_ = kResetting;

  pending_output_samples_.clear();

  NotifyInputBuffersDropped();

  //RETURN_AND_NOTIFY_ON_FAILURE(SendMFTMessage(MFT_MESSAGE_COMMAND_FLUSH, 0),
  //    "Reset: Failed to send message.", PLATFORM_FAILURE,);

  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::NotifyResetDone, base::AsWeakPtr(this)));

  state_ = VPXVideoDecodeAccelerator::kNormal;
}

void VPXVideoDecodeAccelerator::Destroy() {
  DCHECK(CalledOnValidThread());
  Invalidate();
  delete this;
}

static vpx_codec_ctx* InitializeVpxContext(IDirect3DDevice9Ex* device, vpx_codec_ctx* context/*,
                                           const VideoDecoderConfig& config*/) {
  
  context = new vpx_codec_ctx();
  vpx_codec_dec_cfg_t vpx_config = {0};
  /*vpx_config.w = config.coded_size().width();
  vpx_config.h = config.coded_size().height();*/
  vpx_config.threads = 4; //GetThreadCount(config); 

#ifndef AMD_ACCELERATED
  vpx_codec_err_t status = vpx_codec_dec_init(context,
                                              //config.codec() == kCodecVP9 ?
                                                  vpx_codec_vp9_dx() /*:
                                                  vpx_codec_vp8_dx()*/,
                                              NULL, //&vpx_config,
                                              0);
#else
  Interop_Context  interop_context;
  interop_context.pDevice = device;

  vpx_codec_err_t status = vpx_codec_dec_init_ver_ex(context,
    //config.codec() == kCodecVP9 ?
    vpx_codec_vp9_dx() /*:
                       vpx_codec_vp8_dx()*/,
                       NULL, //&vpx_config,
                       0,
                       VPX_DECODER_ABI_VERSION,
                       &interop_context); //device);
#endif

  if (status != VPX_CODEC_OK) {
    LOG(ERROR) << "vpx_codec_dec_init failed, status=" << status;
    delete context;
    return NULL;
  }
  return context;
}

bool VPXVideoDecodeAccelerator::InitDecoder(media::VideoCodecProfile profile) {
  vpx_codec_ = InitializeVpxContext(device_, vpx_codec_/*, config*/);
  if (!vpx_codec_)
    return false;
  /*
  if (config.format() == VideoFrame::YV12A) {
    vpx_codec_alpha_ = InitializeVpxContext(vpx_codec_alpha_, config);
    if (!vpx_codec_alpha_)
      return false;
  }
  */

  EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();

  EGLint config_attribs[] = {
    EGL_BUFFER_SIZE, 32,
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_ALPHA_SIZE, 0,
    EGL_NONE
  };

  EGLint num_configs;

  if (!eglChooseConfig(
      egl_display,
      config_attribs,
      &egl_config_,
      1,
      &num_configs))
    return false;

  return true; //SetDecoderMediaTypes();
}

/*
bool VPXVideoDecodeAccelerator::CheckDecoderVpxSupport() {
  base::win::ScopedComPtr<IMFAttributes> attributes;
  HRESULT hr = decoder_->GetAttributes(attributes.Receive());
  RETURN_ON_HR_FAILURE(hr, "Failed to get decoder attributes", false);

  UINT32 VPX = 0;
  hr = attributes->GetUINT32(MF_SA_D3D_AWARE, &VPX);
  RETURN_ON_HR_FAILURE(hr, "Failed to check if decoder supports VPX", false);

  hr = attributes->SetUINT32(CODECAPI_AVDecVideoAcceleration_H264, TRUE);
  RETURN_ON_HR_FAILURE(hr, "Failed to enable VPX H/W decoding", false);
  return true;
}

bool VPXVideoDecodeAccelerator::SetDecoderMediaTypes() {
  RETURN_ON_FAILURE(SetDecoderInputMediaType(),
                    "Failed to set decoder input media type", false);
  return SetDecoderOutputMediaType(MFVideoFormat_NV12);
}

bool VPXVideoDecodeAccelerator::SetDecoderInputMediaType() {
  base::win::ScopedComPtr<IMFMediaType> media_type;
  HRESULT hr = MFCreateMediaType(media_type.Receive());
  RETURN_ON_HR_FAILURE(hr, "MFCreateMediaType failed", false);

  hr = media_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
  RETURN_ON_HR_FAILURE(hr, "Failed to set major input type", false);

  hr = media_type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_H264);
  RETURN_ON_HR_FAILURE(hr, "Failed to set subtype", false);

  // Not sure about this. msdn recommends setting this value on the input
  // media type.
  hr = media_type->SetUINT32(MF_MT_INTERLACE_MODE,
                             MFVideoInterlace_MixedInterlaceOrProgressive);
  RETURN_ON_HR_FAILURE(hr, "Failed to set interlace mode", false);

  hr = decoder_->SetInputType(0, media_type, 0);  // No flags
  RETURN_ON_HR_FAILURE(hr, "Failed to set decoder input type", false);
  return true;
}

bool VPXVideoDecodeAccelerator::SetDecoderOutputMediaType(
    const GUID& subtype) {
  base::win::ScopedComPtr<IMFMediaType> out_media_type;

  for (uint32 i = 0;
       SUCCEEDED(decoder_->GetOutputAvailableType(0, i,
                                                  out_media_type.Receive()));
       ++i) {
    GUID out_subtype = {0};
    HRESULT hr = out_media_type->GetGUID(MF_MT_SUBTYPE, &out_subtype);
    RETURN_ON_HR_FAILURE(hr, "Failed to get output major type", false);

    if (out_subtype == subtype) {
      hr = decoder_->SetOutputType(0, out_media_type, 0);  // No flags
      RETURN_ON_HR_FAILURE(hr, "Failed to set decoder output type", false);
      return true;
    }
    out_media_type.Release();
  }
  return false;
}

bool VPXVideoDecodeAccelerator::SendMFTMessage(MFT_MESSAGE_TYPE msg,
                                                int32 param) {
  HRESULT hr = decoder_->ProcessMessage(msg, param);
  return SUCCEEDED(hr);
}

// Gets the minimum buffer sizes for input and output samples. The MFT will not
// allocate buffer for input nor output, so we have to do it ourselves and make
// sure they're the correct size. We only provide decoding if VPX is enabled.
bool VPXVideoDecodeAccelerator::GetStreamsInfoAndBufferReqs() {
  HRESULT hr = decoder_->GetInputStreamInfo(0, &input_stream_info_);
  RETURN_ON_HR_FAILURE(hr, "Failed to get input stream info", false);

  hr = decoder_->GetOutputStreamInfo(0, &output_stream_info_);
  RETURN_ON_HR_FAILURE(hr, "Failed to get decoder output stream info", false);

  DVLOG(1) << "Input stream info: ";
  DVLOG(1) << "Max latency: " << input_stream_info_.hnsMaxLatency;
  // There should be three flags, one for requiring a whole frame be in a
  // single sample, one for requiring there be one buffer only in a single
  // sample, and one that specifies a fixed sample size. (as in cbSize)
  CHECK_EQ(input_stream_info_.dwFlags, 0x7u);

  DVLOG(1) << "Min buffer size: " << input_stream_info_.cbSize;
  DVLOG(1) << "Max lookahead: " << input_stream_info_.cbMaxLookahead;
  DVLOG(1) << "Alignment: " << input_stream_info_.cbAlignment;

  DVLOG(1) << "Output stream info: ";
  // The flags here should be the same and mean the same thing, except when
  // VPX is enabled, there is an extra 0x100 flag meaning decoder will
  // allocate its own sample.
  DVLOG(1) << "Flags: "
          << std::hex << std::showbase << output_stream_info_.dwFlags;
  CHECK_EQ(output_stream_info_.dwFlags, 0x107u);
  DVLOG(1) << "Min buffer size: " << output_stream_info_.cbSize;
  DVLOG(1) << "Alignment: " << output_stream_info_.cbAlignment;
  return true;
}
*/

/*
void VPXVideoDecodeAccelerator::DoDecode() {
  // This function is also called from FlushInternal in a loop which could
  // result in the state transitioning to kStopped due to no decoded output.
  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kNormal || state_ == kFlushing ||
                                state_ == kStopped),
      "DoDecode: not in normal/flushing/stopped state", ILLEGAL_STATE,);


  MFT_OUTPUT_DATA_BUFFER output_data_buffer = {0};
  DWORD status = 0;

  HRESULT hr = decoder_->ProcessOutput(0,  // No flags
                                       1,  // # of out streams to pull from
                                       &output_data_buffer,
                                       &status);
  IMFCollection* events = output_data_buffer.pEvents;
  if (events != NULL) {
    VLOG(1) << "Got events from ProcessOuput, but discarding";
    events->Release();
  }
  if (FAILED(hr)) {
    // A stream change needs further ProcessInput calls to get back decoder
    // output which is why we need to set the state to stopped.
    if (hr == MF_E_TRANSFORM_STREAM_CHANGE) {
      if (!SetDecoderOutputMediaType(MFVideoFormat_NV12)) {
        // Decoder didn't let us set NV12 output format. Not sure as to why
        // this can happen. Give up in disgust.
        NOTREACHED() << "Failed to set decoder output media type to NV12";
        state_ = kStopped;
      } else {
        DVLOG(1) << "Received output format change from the decoder."
                    " Recursively invoking DoDecode";
        DoDecode();
      }
      return;
    } else if (hr == MF_E_TRANSFORM_NEED_MORE_INPUT) {
      // No more output from the decoder. Stop playback.
      state_ = kStopped;
      return;
    } else {
      NOTREACHED() << "Unhandled error in DoDecode()";
      return;
    }
  }


  TRACE_EVENT_END_ETW("VPXVideoDecodeAccelerator.Decoding", this, "");

  TRACE_COUNTER1("VPX Decoding", "TotalPacketsBeforeDecode",
                 inputs_before_decode_);

  inputs_before_decode_ = 0;

  RETURN_AND_NOTIFY_ON_FAILURE(ProcessOutputSample(output_data_buffer.pSample),
      "Failed to process output sample.", PLATFORM_FAILURE,);
}
*/

/*
bool VPXVideoDecodeAccelerator::ProcessOutputSample(IMFSample* sample) {
  RETURN_ON_FAILURE(sample, "Decode succeeded with NULL output sample", false);

  base::win::ScopedComPtr<IMFMediaBuffer> output_buffer;
  HRESULT hr = sample->GetBufferByIndex(0, output_buffer.Receive());
  RETURN_ON_HR_FAILURE(hr, "Failed to get buffer from output sample", false);

  base::win::ScopedComPtr<IDirect3DSurface9> surface;
  hr = MFGetService(output_buffer, MR_BUFFER_SERVICE,
                    IID_PPV_ARGS(surface.Receive()));
  RETURN_ON_HR_FAILURE(hr, "Failed to get D3D surface from output sample",
                       false);

  LONGLONG input_buffer_id = 0;
  RETURN_ON_HR_FAILURE(sample->GetSampleTime(&input_buffer_id),
                       "Failed to get input buffer id associated with sample",
                       false);

  pending_output_samples_.push_back(
      PendingSampleInfo(input_buffer_id, sample));

  // If we have available picture buffers to copy the output data then use the
  // first one and then flag it as not being available for use.
  if (output_picture_buffers_.size()) {
    ProcessPendingSamples();
    return true;
  }
  if (pictures_requested_) {
    DVLOG(1) << "Waiting for picture slots from the client.";
    return true;
  }

  // We only read the surface description, which contains its width/height when
  // we need the picture buffers from the client. Once we have those, then they
  // are reused.
  D3DSURFACE_DESC surface_desc;
  hr = surface->GetDesc(&surface_desc);
  RETURN_ON_HR_FAILURE(hr, "Failed to get surface description", false);

  // Go ahead and request picture buffers.
  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::RequestPictureBuffers,
      base::AsWeakPtr(this), surface_desc.Width, surface_desc.Height));

  pictures_requested_ = true;
  return true;
}
*/

void VPXVideoDecodeAccelerator::ProcessPendingSamples(
  IDirect3DSurface9 *direct_output_surface) {
  RETURN_AND_NOTIFY_ON_FAILURE(make_context_current_.Run(),
      "Failed to make context current", PLATFORM_FAILURE,);

  OutputBuffers::iterator index;

  for (index = output_picture_buffers_.begin();
       index != output_picture_buffers_.end() &&
       !pending_output_samples_.empty();
       ++index) {
    if (index->second->available()) {
      PendingSampleInfo sample_info = pending_output_samples_.front();
#ifndef AMD_ACCELERATED
      HRESULT hr;

      /* base::win::ScopedComPtr<IMFMediaBuffer> output_buffer;
      HRESULT hr = sample_info.output_sample->GetBufferByIndex(
          0, output_buffer.Receive());
      RETURN_AND_NOTIFY_ON_HR_FAILURE(
          hr, "Failed to get buffer from output sample", PLATFORM_FAILURE,);

      base::win::ScopedComPtr<IDirect3DSurface9> surface;
      hr = MFGetService(output_buffer, MR_BUFFER_SERVICE,
                        IID_PPV_ARGS(surface.Receive()));
      RETURN_AND_NOTIFY_ON_HR_FAILURE(
          hr, "Failed to get D3D surface from output sample",
          PLATFORM_FAILURE,); */
      
      vpx_image_t *vpx_image = sample_info.output_sample;
      // copy decoding result vpx_image into temp_surface_yv12_
      if (!temp_surface_yv12_) {
        //device_->CheckDeviceFormatConversion(D3DADAPTER_DEFAULT,d3dCaps.DeviceType,
        //  (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DFMT_A8R8G8B8);
        // Creating an offscreen plain surface to load the YV12 image
        device_->CreateOffscreenPlainSurface(vpx_image->d_w, vpx_image->d_h,
          (D3DFORMAT)MAKEFOURCC('Y','V','1','2'),D3DPOOL_DEFAULT ,&temp_surface_yv12_,NULL);
      }

      IDirect3DSurface9 *surface = temp_surface_yv12_;

      { // Copy decoded frame into d3d surface
      int w = vpx_image->d_w, h = vpx_image->d_h;
      D3DLOCKED_RECT LockedRect; // Describes a locked rectangular region.
      RECT rect;
      rect.left = 0;   rect.top = 0;
      rect.right = w;   rect.bottom = h;

      if(FAILED(surface->LockRect(&LockedRect, &rect, 0))){
      }

      BYTE *locked = (BYTE *) LockedRect.pBits; // Pointer to the locked bits.

      int i, pitch=LockedRect.Pitch;
      for (i =0; i<h; i++)
        memcpy(locked+pitch*i, vpx_image->planes[0]+vpx_image->stride[0]*i, w);
      locked += pitch*h;
      for (i=0; i<h/2; i++)
        memcpy(locked+pitch/2*i, vpx_image->planes[2]+vpx_image->stride[2]*i, w/2);
      locked += pitch*h/4;
      for (i=0; i<h/2; i++)
        memcpy(locked+pitch/2*i, vpx_image->planes[1]+vpx_image->stride[1]*i, w/2);

      surface->UnlockRect();
      }



      D3DSURFACE_DESC surface_desc;
      hr = surface->GetDesc(&surface_desc);
      RETURN_AND_NOTIFY_ON_HR_FAILURE(
          hr, "Failed to get surface description", PLATFORM_FAILURE,);

      if (surface_desc.Width !=
              static_cast<uint32>(index->second->size().width()) ||
          surface_desc.Height !=
              static_cast<uint32>(index->second->size().height())) {
        temp_surface_yv12_->Release();
        temp_surface_yv12_ = NULL;

        HandleResolutionChanged(surface_desc.Width, surface_desc.Height);
        return;
      }
       
      RETURN_AND_NOTIFY_ON_FAILURE(
          index->second->CopyOutputSampleDataToPictureBuffer(
              surface),
          "Failed to copy output sample", PLATFORM_FAILURE,);
#else
      // In the accelerated case, the decoded picture is in temp_surface_rgba_

      if (width_ !=
        static_cast<uint32>(index->second->size().width()) ||
        height_ !=
        static_cast<uint32>(index->second->size().height())) {
          HandleResolutionChanged(width_, height_);
          return;
      }
      //base::win::ScopedComPtr<IDirect3DSurface9> d3d_surface;
      //HRESULT hr = index->second->decoding_texture_->GetSurfaceLevel(0,d3d_surface.Receive());
      //RETURN_ON_HR_FAILURE(hr, "Failed to get surface from texture", );
      if (direct_output_surface) {
        base::win::ScopedComPtr<IDirect3DSurface9> d3d_surface;
        HRESULT hr = index->second->decoding_texture_->GetSurfaceLevel(0, d3d_surface.Receive());
        RETURN_ON_HR_FAILURE(hr, "Failed to get surface from texture 3", );
        if (direct_output_surface != d3d_surface.get())
          LOG(ERROR) << "Logic error!!!!!";
      } else {

        RETURN_AND_NOTIFY_ON_FAILURE(
          index->second->CopyOutputSampleDataToPictureBuffer(
          temp_surface_rgba_),
          "Failed to copy output sample", PLATFORM_FAILURE,);
      }
#endif

      media::Picture output_picture(index->second->id(),
                                    sample_info.input_buffer_id);
      base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
          &VPXVideoDecodeAccelerator::NotifyPictureReady,
          base::AsWeakPtr(this), output_picture));

      index->second->set_available(false);
      pending_output_samples_.pop_front();
    }
  }

  if (!pending_input_buffers_.empty() && pending_output_samples_.empty()) {
    base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
        &VPXVideoDecodeAccelerator::DecodePendingInputBuffers,
        base::AsWeakPtr(this)));
  }
}

void VPXVideoDecodeAccelerator::StopOnError(
  media::VideoDecodeAccelerator::Error error) {
  DCHECK(CalledOnValidThread());

  if (client_)
    client_->NotifyError(error);
  client_ = NULL;

  if (state_ != kUninitialized) {
    Invalidate();
  }
}

void VPXVideoDecodeAccelerator::Invalidate() {
  if (state_ == kUninitialized)
    return;
  output_picture_buffers_.clear();
  pending_output_samples_.clear();
  pending_input_buffers_.clear();
  decoder_.Release();
  MFShutdown();
  state_ = kUninitialized;
}

void VPXVideoDecodeAccelerator::NotifyInitializeDone() {
  if (client_)
    client_->NotifyInitializeDone();
}

void VPXVideoDecodeAccelerator::NotifyInputBufferRead(int input_buffer_id) {
  if (client_)
    client_->NotifyEndOfBitstreamBuffer(input_buffer_id);
}

void VPXVideoDecodeAccelerator::NotifyFlushDone() {
  if (client_)
    client_->NotifyFlushDone();
}

void VPXVideoDecodeAccelerator::NotifyResetDone() {
  if (client_)
    client_->NotifyResetDone();
}

void VPXVideoDecodeAccelerator::RequestPictureBuffers(int width, int height) {
  // This task could execute after the decoder has been torn down.
  if (state_ != kUninitialized && client_) {
    client_->ProvidePictureBuffers(
        kNumPictureBuffers,
        gfx::Size(width, height),
        GL_TEXTURE_2D);
  }
}

void VPXVideoDecodeAccelerator::NotifyPictureReady(
    const media::Picture& picture) {
  // This task could execute after the decoder has been torn down.
  if (state_ != kUninitialized && client_)
    client_->PictureReady(picture);
}

void VPXVideoDecodeAccelerator::NotifyInputBuffersDropped() {
  if (!client_ || !pending_output_samples_.empty())
    return;

  for (PendingInputs::iterator it = pending_input_buffers_.begin();
       it != pending_input_buffers_.end(); ++it) {
    LONGLONG input_buffer_id = it->id_; //0;
    //RETURN_ON_HR_FAILURE((*it)->GetSampleTime(&input_buffer_id),
    //                     "Failed to get buffer id associated with sample",);
    client_->NotifyEndOfBitstreamBuffer(input_buffer_id);
  }
  pending_input_buffers_.clear();
}

void VPXVideoDecodeAccelerator::DecodePendingInputBuffers() {
  RETURN_AND_NOTIFY_ON_FAILURE((state_ != kUninitialized),
      "Invalid state: " << state_, ILLEGAL_STATE,);

  if (pending_input_buffers_.empty() || !pending_output_samples_.empty())
    return;

  PendingInputs pending_input_buffers_copy;
  std::swap(pending_input_buffers_, pending_input_buffers_copy);

  for (PendingInputs::iterator it = pending_input_buffers_copy.begin();
       it != pending_input_buffers_copy.end(); ++it) {
    DecodeInternal(*it, true);
  }
}

void VPXVideoDecodeAccelerator::FlushInternal() {
  // The DoDecode function sets the state to kStopped when the decoder returns
  // MF_E_TRANSFORM_NEED_MORE_INPUT.
  // The MFT decoder can buffer upto 30 frames worth of input before returning
  // an output frame. This loop here attemptres to retrieve as many output frames
  // as possible from the buffered set.
  /*
  while (state_ != kStopped) {
    //DoDecode();
    if (!pending_output_samples_.empty())
      return;
  }
  */
  while (! pending_output_samples_.empty())
    this->ProcessPendingSamples();


  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::NotifyFlushDone, base::AsWeakPtr(this)));

  state_ = kNormal;
}

void VPXVideoDecodeAccelerator::DecodeInternal(VpxSample& sample, bool is_a_pending_sample) {
  DCHECK(CalledOnValidThread());
    // if restart_ is true, the sample must be the first packet, get info
  if (restart_) {
    vpx_codec_stream_info_t info;
    vpx_codec_peek_stream_info_ex(vpx_codec_vp9_dx(), &sample.data_[0], sample.data_.size(), &info);
    width_ = info.w;
    height_ = info.h;
    LOG(ERROR) << "info.width=" << info.w << " info.height=" << info.h;
    restart_ = false;
  }




  if (state_ == kUninitialized)
    return;

  if (!pending_output_samples_.empty() || !pending_input_buffers_.empty()) {
    VpxSample tmp;
    pending_input_buffers_.push_back(tmp);
    pending_input_buffers_.back().data_.swap(sample.data_);
    return;
  }

  if (!inputs_before_decode_) {
    TRACE_EVENT_BEGIN_ETW("VPXVideoDecodeAccelerator.Decoding", this, "");
  }

  inputs_before_decode_++;

  IDirect3DSurface9 *direct_output_surface = NULL;

/*
  HRESULT hr = decoder_->ProcessInput(0, sample, 0);
  // As per msdn if the decoder returns MF_E_NOTACCEPTING then it means that it
  // has enough data to produce one or more output samples. In this case the
  // recommended options are to
  // 1. Generate new output by calling IMFTransform::ProcessOutput until it
  //    returns MF_E_TRANSFORM_NEED_MORE_INPUT.
  // 2. Flush the input data
  // We implement the first option, i.e to retrieve the output sample and then
  // process the input again. Failure in either of these steps is treated as a
  // decoder failure.
  if (hr == MF_E_NOTACCEPTING) {
    DoDecode();
    RETURN_AND_NOTIFY_ON_FAILURE((state_ == kStopped || state_ == kNormal),
        "Failed to process output. Unexpected decoder state: " << state_,
        PLATFORM_FAILURE,);
    hr = decoder_->ProcessInput(0, sample, 0);
    // If we continue to get the MF_E_NOTACCEPTING error we do the following:-
    // 1. Add the input sample to the pending queue.
    // 2. If we don't have any output samples we post the
    //    DecodePendingInputBuffers task to process the pending input samples.
    //    If we have an output sample then the above task is posted when the
    //    output samples are sent to the client.
    // This is because we only support 1 pending output sample at any
    // given time due to the limitation with the Microsoft media foundation
    // decoder where it recycles the output Decoder surfaces.
    if (hr == MF_E_NOTACCEPTING) {
      pending_input_buffers_.push_back(sample);
      if (pending_output_samples_.empty()) {
        base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
            &VPXVideoDecodeAccelerator::DecodePendingInputBuffers,
            base::AsWeakPtr(this)));
      }
      return;
    }
  }
  RETURN_AND_NOTIFY_ON_HR_FAILURE(hr, "Failed to process input sample",
      PLATFORM_FAILURE,);
*/

//  bool VpxVideoDecoder::VpxDecode(const scoped_refptr<DecoderBuffer>& buffer,
//                                scoped_refptr<VideoFrame>* video_frame)
//{
  //DCHECK(video_frame);
  //DCHECK(!buffer->end_of_stream());
#ifndef AMD_ACCELERATED
  // Pass |buffer| to libvpx.
  //int64 timestamp = buffer->timestamp().InMicroseconds();
  void* user_priv = reinterpret_cast<void*>(sample.id_);
  vpx_codec_err_t status = vpx_codec_decode(vpx_codec_,
                                            &sample.data_[0], //buffer->data(),
                                            sample.data_.size(), //buffer->data_size(),
                                            user_priv,
                                            0);
  if (status != VPX_CODEC_OK) {
    LOG(ERROR) << "vpx_codec_decode() failed, status=" << status;
    return; // false;
  }

  // Gets pointer to decoded data.
  vpx_codec_iter_t iter = NULL;
  const vpx_image_t* vpx_image = vpx_codec_get_frame(vpx_codec_, &iter);
  if (!vpx_image) {
    //*video_frame = NULL;
    return; // true;
  }

  if (vpx_image->user_priv != reinterpret_cast<void*>(sample.id_)) {
    LOG(ERROR) << "Invalid output timestamp.";
    return; // false;
  }

  /*
  const vpx_image_t* vpx_image_alpha = NULL;
  if (vpx_codec_alpha_ && buffer->side_data_size() >= 8) {
    // Pass alpha data to libvpx.
    int64 timestamp_alpha = buffer->timestamp().InMicroseconds();
    void* user_priv_alpha = reinterpret_cast<void*>(&timestamp_alpha);

    // First 8 bytes of side data is side_data_id in big endian.
    const uint64 side_data_id = base::NetToHost64(
        *(reinterpret_cast<const uint64*>(buffer->side_data())));
    if (side_data_id == 1) {
      status = vpx_codec_decode(vpx_codec_alpha_,
                                buffer->side_data() + 8,
                                buffer->side_data_size() - 8,
                                user_priv_alpha,
                                0);

      if (status != VPX_CODEC_OK) {
        LOG(ERROR) << "vpx_codec_decode() failed on alpha, status=" << status;
        return false;
      }

      // Gets pointer to decoded data.
      vpx_codec_iter_t iter_alpha = NULL;
      vpx_image_alpha = vpx_codec_get_frame(vpx_codec_alpha_, &iter_alpha);
      if (!vpx_image_alpha) {
        *video_frame = NULL;
        return true;
      }

      if (vpx_image_alpha->user_priv !=
          reinterpret_cast<void*>(&timestamp_alpha)) {
        LOG(ERROR) << "Invalid output timestamp on alpha.";
        return false;
      }
    }
  }
  */
  /*
  CopyVpxImageTo(vpx_image, vpx_image_alpha, video_frame);
  (*video_frame)->SetTimestamp(base::TimeDelta::FromMicroseconds(timestamp));
  */
#else
  {
    if (!temp_surface_rgba_) {
      HRESULT hr = device_->CreateOffscreenPlainSurfaceEx(width_, height_,
        D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT ,&temp_surface_rgba_, &temp_surface_rgba_shared_handle_, NULL);
      LOG(ERROR) << "width_=" << width_ << " height_=" << height_;
      LOG(ERROR) << "temp_surface_rgba_ = " << temp_surface_rgba_;
      LOG(ERROR) << "hr = " << hr;
      RETURN_ON_HR_FAILURE(hr, "Failed to create temp_surface_rgba", );

      { // Copy decoded frame into d3d surface
        int w = width_, h = height_;
        D3DLOCKED_RECT LockedRect; // Describes a locked rectangular region.
        RECT rect;
        rect.left = 0;   rect.top = 0;
        rect.right = w;   rect.bottom = h;

        if(FAILED(temp_surface_rgba_->LockRect(&LockedRect, &rect, 0))){
        }

        BYTE *locked = (BYTE *) LockedRect.pBits; // Pointer to the locked bits.

        int i, pitch=LockedRect.Pitch;
        for (i =0; i<h; i++)
          if ((i/16)%2)
            memset(locked+pitch*i, 255, width_*4);
          else
            memset(locked+pitch*i, 0, width_*4);

        temp_surface_rgba_->UnlockRect();
      }

    }

    D3DSURFACE_DESC surface_desc;
    HRESULT hr = temp_surface_rgba_->GetDesc(&surface_desc);
    RETURN_AND_NOTIFY_ON_HR_FAILURE(
          hr, "Failed to get temp_surface_rgba_ description", PLATFORM_FAILURE,);
    if (width_ !=
      static_cast<uint32>(surface_desc.Width) ||
        height_ !=
        static_cast<uint32>(surface_desc.Height)) {
      temp_surface_rgba_->Release();
      hr = device_->CreateOffscreenPlainSurface(width_, height_,
        D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT ,&temp_surface_rgba_,NULL);
      RETURN_ON_HR_FAILURE(hr, "Failed to create resized temp_surface_rgba_", );

    }

    OutputBuffers::iterator index;
//#define DIRECT_OUTPUT_TO_PICTURE_BUFFER
#ifdef DIRECT_OUTPUT_TO_PICTURE_BUFFER
    for (index = output_picture_buffers_.begin();
      index != output_picture_buffers_.end();
      ++index)
        if (index->second->available())
          break;

    if (index != output_picture_buffers_.end()) {
      base::win::ScopedComPtr<IDirect3DSurface9> d3d_surface;
      hr = index->second->decoding_texture_->GetSurfaceLevel(0, d3d_surface.Receive());
      RETURN_ON_HR_FAILURE(hr, "Failed to get surface from texture", );
      direct_output_surface = d3d_surface;
    }


    GLint current_texture = 0;
    if (direct_output_surface) {
      // This function currently executes in the context of IPC handlers in the
      // GPU process which ensures that there is always an OpenGL context.

      glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

      glBindTexture(GL_TEXTURE_2D, index->second->picture_buffer_.texture_id());

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      base::win::ScopedComPtr<IDirect3DSurface9> d3d_surface;
      hr = index->second->decoding_texture_->GetSurfaceLevel(0, d3d_surface.Receive());
      RETURN_ON_HR_FAILURE(hr, "Failed to get surface from texture 4", );

      /*hr = device_->StretchRect(dest_surface,
      NULL,
      d3d_surface,
      NULL,
      D3DTEXF_NONE);
      RETURN_ON_HR_FAILURE(hr, "Colorspace conversion via StretchRect failed",
      false);*/
    }
#else //DIRECT_OUTPUT_TO_PICTURE_BUFFER
    direct_output_surface = NULL;
#endif //DIRECT_OUTPUT_TO_PICTURE_BUFFER

    CStopWatch timer;
    timer.startTimer();
    Interop_Context  interop_context;
    interop_context.pDevice = device_;
    interop_context.pSharedHandle = temp_surface_rgba_shared_handle_;
    interop_context.pSurface = temp_surface_rgba_;
    vpx_codec_err_t status = vpx_codec_decode_ex(vpx_codec_,
      &sample.data_[0], //buffer->data(),
      sample.data_.size(), //buffer->data_size(),
      NULL,
      0,
      &interop_context); //direct_output_surface ? direct_output_surface : temp_surface_rgba_);
    timer.stopTimer();
    double t = timer.getElapsedTime();
    LOG(ERROR) << "vpx_codec_decode_ex time (secs): " << t;

    if (status != VPX_CODEC_OK) {
      LOG(ERROR) << "vpx_codec_decode_ex() failed, status=" << status;
      return; // false;
    }

#ifdef DIRECT_OUTPUT_TO_PICTURE_BUFFER
    if (direct_output_surface) {
#if 1
      // Ideally, this should be done immediately before the draw call that uses
      // the texture. Flush it once here though.
      hr = query_->Issue(D3DISSUE_END);
      RETURN_ON_HR_FAILURE(hr, "Failed to issue END", );

      // The VPX decoder has its own device which it uses for decoding. ANGLE
      // has its own device which we don't have access to.
      // The above code attempts to copy the decoded picture into a surface
      // which is owned by ANGLE. As there are multiple devices involved in
      // this, the StretchRect call above is not synchronous.
      // We attempt to flush the batched operations to ensure that the picture is
      // copied to the surface owned by ANGLE.
      // We need to do this in a loop and call flush multiple times.
      // We have seen the GetData call for flushing the command buffer fail to
      // return success occassionally on multi core machines, leading to an
      // infinite loop.
      // Workaround is to have an upper limit of 10 on the number of iterations to
      // wait for the Flush to finish.
      int iterations = 0;
      while ((query_->GetData(NULL, 0, D3DGETDATA_FLUSH) == S_FALSE) &&
        ++iterations < kMaxIterationsForD3DFlush) {
          Sleep(1);  // Poor-man's Yield().
      }
#endif

      EGLDisplay egl_display = gfx::GLSurfaceEGL::GetHardwareDisplay();
      eglBindTexImage(
        egl_display,
        index->second->decoding_surface_,
        EGL_BACK_BUFFER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glBindTexture(GL_TEXTURE_2D, current_texture);
    }
#endif //DIRECT_OUTPUT_TO_PICTURE_BUFFER
  }
#endif

  if (output_picture_buffers_.size()) {
    
    int input_buffer_id =  sample.id_;

    pending_output_samples_.push_back(
      PendingSampleInfo(input_buffer_id,
#ifndef AMD_ACCELERATED
      (vpx_image_t*)vpx_image
#else
      // in the accelerated case pass the decode result is i n temp_surface_rgba_
      NULL
#endif
      ));

    // Have output buffer, present vpx_image immediately
    ProcessPendingSamples(direct_output_surface);
    return; // true;
  } else { // don't have output buffer , request

    if (pictures_requested_) {
      DVLOG(1) << "Waiting for picture slots from the client.";
    } else {

      /* https://groups.google.com/a/webmproject.org/forum/#!topic/apps-devel/umsu4_bAv3g
      > What really happens when h/w is different from d_h/d_w? 

      The buffer is aligned. If the display dimensions are not multiples of 
      16 or there is a border (used internally), then they will not match. 
      When you copy the image after decoding, you want to copy the display 
      size. 

      */

      // Go ahead and request picture buffers.
      base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
        &VPXVideoDecodeAccelerator::RequestPictureBuffers,
        base::AsWeakPtr(this),
#ifndef AMD_ACCELERATED
        vpx_image->d_w, vpx_image->d_h
#else
        width_, height_
#endif
        ));
       pictures_requested_ = true;

    }

  }

  //return true;
//}

  // DoDecode();

  RETURN_AND_NOTIFY_ON_FAILURE((state_ == kStopped || state_ == kNormal),
      "Failed to process output. Unexpected decoder state: " << state_,
      ILLEGAL_STATE,);

  LONGLONG input_buffer_id = (LONGLONG)sample.id_; //vpx_image->user_priv;


  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::NotifyInputBufferRead,
      base::AsWeakPtr(this), input_buffer_id));
}

void VPXVideoDecodeAccelerator::HandleResolutionChanged(int width,
                                                         int height) {
  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::DismissStaleBuffers,
      base::AsWeakPtr(this), output_picture_buffers_));

  base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
      &VPXVideoDecodeAccelerator::RequestPictureBuffers,
      base::AsWeakPtr(this), width, height));

  output_picture_buffers_.clear();
}

void VPXVideoDecodeAccelerator::DismissStaleBuffers(
    const OutputBuffers& picture_buffers) {
  OutputBuffers::const_iterator index;

  for (index = picture_buffers.begin();
       index != picture_buffers.end();
       ++index) {
    DVLOG(1) << "Dismissing picture id: " << index->second->id();
    client_->DismissPictureBuffer(index->second->id());
  }
}

}  // namespace content
