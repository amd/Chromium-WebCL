#ifndef OCL_MSG_H
#define OCL_MSG_H

#include "ipc/ipc_message_utils.h"
#include "ipc/param_traits_macros.h"
#define cl_pointer uint32

#ifdef GPU_MESSAGE_START
#undef GPU_MESSAGE_START
#endif
#define GPU_MESSAGE_START 1000000

IPC_SYNC_MESSAGE_CONTROL1_3(OpenCLIPCMsg_clGetPlatformIDs, 
	cl_uint, //! cl_uint num_entries
	std::vector<unsigned char>, //! cl_platform_id * platforms
	cl_uint, //! cl_uint * num_platforms
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetPlatformInfo, 
	cl_pointer, //! cl_platform_id platform
	cl_platform_info, //! cl_platform_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetDeviceIDs, 
	cl_pointer, //! cl_platform_id platform
	cl_device_type, //! cl_device_type device_type
	cl_uint, //! cl_uint num_entries
	std::vector<unsigned char>, //! cl_device_id * devices
	cl_uint, //! cl_uint * num_devices
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetDeviceInfo, 
	cl_pointer, //! cl_device_id device
	cl_device_info, //! cl_device_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL5_2(OpenCLIPCMsg_clCreateContext, 
	std::vector<unsigned char>, //! const cl_context_properties * properties
	cl_uint, //! cl_uint num_devices
	std::vector<unsigned char>, //! const cl_device_id * devices
	cl_pointer, //! callback callback
	cl_pointer, //! void * user_data
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_context

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseContext, 
	cl_pointer, //! cl_context context
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_2(OpenCLIPCMsg_clCreateCommandQueue, 
	cl_pointer, //! cl_context context
	cl_pointer, //! cl_device_id device
	cl_command_queue_properties, //! cl_command_queue_properties properties
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_command_queue

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseCommandQueue, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetCommandQueueInfo, 
	cl_pointer, //! cl_command_queue command_queue
	cl_command_queue_info, //! cl_command_queue_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLIPCMsg_clCreateBuffer, 
	cl_pointer, //! cl_context context
	cl_mem_flags, //! cl_mem_flags flags
	size_t, //! size_t size
	cl_pointer, //! void * host_ptr
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_mem

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLIPCMsg_clCreateSubBuffer, 
	cl_pointer, //! cl_mem buffer
	cl_mem_flags, //! cl_mem_flags flags
	cl_buffer_create_type, //! cl_buffer_create_type buffer_create_type
	std::vector<unsigned char>, //! const void * buffer_create_info
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_mem

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseMemObject, 
	cl_pointer, //! cl_mem memobj
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLIPCMsg_clGetSupportedImageFormats, 
	cl_pointer, //! cl_context context
	cl_mem_flags, //! cl_mem_flags flags
	cl_mem_object_type, //! cl_mem_object_type image_type
	cl_uint, //! cl_uint num_entries
	std::vector<unsigned char>, //! cl_image_format * image_formats
	cl_uint, //! cl_uint * num_image_formats
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetMemObjectInfo, 
	cl_pointer, //! cl_mem memobj
	cl_mem_info, //! cl_mem_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetImageInfo, 
	cl_pointer, //! cl_mem image
	cl_image_info, //! cl_image_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLIPCMsg_clCreateSampler, 
	cl_pointer, //! cl_context context
	cl_bool, //! cl_bool normalized_coords
	cl_addressing_mode, //! cl_addressing_mode addressing_mode
	cl_filter_mode, //! cl_filter_mode filter_mode
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_sampler

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseSampler, 
	cl_pointer, //! cl_sampler sampler
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetSamplerInfo, 
	cl_pointer, //! cl_sampler sampler
	cl_sampler_info, //! cl_sampler_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLIPCMsg_clCreateProgramWithSource, 
	cl_pointer, //! cl_context context
	cl_uint, //! cl_uint count
	std::vector<unsigned char>, //! const char ** strings
	std::vector<unsigned char>, //! const size_t * lengths
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_program

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseProgram, 
	cl_pointer, //! cl_program program
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_6(t1, t2, t3, t4, t5, t6) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6
#define IPC_COMMA_AND_6(x) x
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6>
struct ParamTraits< Tuple6<T1, T2, T3, T4, T5, T6> > {
  typedef Tuple6<T1, T2, T3, T4, T5, T6> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");

  }
};
} // namespace IPC
#define IPC_TUPLE_IN_6(t1, t2, t3, t4, t5, t6) Tuple6<t1, t2, t3, t4, t5, t6>
#define IPC_NAME_IN_6(t1, t2, t3, t4, t5, t6) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6)
#define IPC_SYNC_MESSAGE_CONTROL6_1(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, otype1) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 6, 1, (itype1, itype2, itype3, itype4, itype5, itype6), (otype1))
IPC_SYNC_MESSAGE_CONTROL6_1(OpenCLIPCMsg_clBuildProgram, 
	cl_pointer, //! cl_program program
	cl_uint, //! cl_uint num_devices
	std::vector<unsigned char>, //! const cl_device_id * device_list
	std::vector<unsigned char>, //! const char * options
	cl_pointer, //! callback callback
	cl_pointer, //! void * user_data
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetProgramInfo, 
	cl_pointer, //! cl_program program
	cl_program_info, //! cl_program_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLIPCMsg_clGetProgramBuildInfo, 
	cl_pointer, //! cl_program program
	cl_pointer, //! cl_device_id device
	cl_program_build_info, //! cl_program_build_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL2_2(OpenCLIPCMsg_clCreateKernel, 
	cl_pointer, //! cl_program program
	std::vector<unsigned char>, //! const char * kernel_name
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_kernel

IPC_SYNC_MESSAGE_CONTROL2_3(OpenCLIPCMsg_clCreateKernelsInProgram, 
	cl_pointer, //! cl_program program
	cl_uint, //! cl_uint num_kernels
	std::vector<unsigned char>, //! cl_kernel * kernels
	cl_uint, //! cl_uint * num_kernels_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseKernel, 
	cl_pointer, //! cl_kernel kernel
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_1(OpenCLIPCMsg_clSetKernelArg, 
	cl_pointer, //! cl_kernel kernel
	cl_uint, //! cl_uint arg_index
	size_t, //! size_t arg_size
	std::vector<unsigned char>, //! const void * arg_value
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetKernelInfo, 
	cl_pointer, //! cl_kernel kernel
	cl_kernel_info, //! cl_kernel_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_3(OpenCLIPCMsg_clGetKernelWorkGroupInfo, 
	cl_pointer, //! cl_kernel kernel
	cl_pointer, //! cl_device_id device
	cl_kernel_work_group_info, //! cl_kernel_work_group_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL2_1(OpenCLIPCMsg_clWaitForEvents, 
	cl_uint, //! cl_uint num_events
	std::vector<unsigned char>, //! const cl_event * event_list
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetEventInfo, 
	cl_pointer, //! cl_event event
	cl_event_info, //! cl_event_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_2(OpenCLIPCMsg_clCreateUserEvent, 
	cl_pointer, //! cl_context context
	cl_int, //! cl_int * errcode_ret
	cl_pointer) //! return cl_event

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clReleaseEvent, 
	cl_pointer, //! cl_event event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL2_1(OpenCLIPCMsg_clSetUserEventStatus, 
	cl_pointer, //! cl_event event
	cl_int, //! cl_int execution_status
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_1(OpenCLIPCMsg_clSetEventCallback, 
	cl_pointer, //! cl_event event
	cl_int, //! cl_int command_exec_callback_type
	cl_pointer, //! callback callback
	cl_pointer, //! void * user_data
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_3(OpenCLIPCMsg_clGetEventProfilingInfo, 
	cl_pointer, //! cl_event event
	cl_profiling_info, //! cl_profiling_info param_name
	size_t, //! size_t param_value_size
	std::vector<unsigned char>, //! void * param_value
	size_t, //! size_t * param_value_size_ret
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clFlush, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clFinish, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_7(t1, t2, t3, t4, t5, t6, t7) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7
#define IPC_COMMA_AND_7(x) x
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
struct ParamTraits< Tuple7<T1, T2, T3, T4, T5, T6, T7> > {
  typedef Tuple7<T1, T2, T3, T4, T5, T6, T7> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class OutT1, class OutT2, class OutT3>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple7<InT1, InT2, InT3, InT4, InT5, InT6, InT7>&in,
        Tuple3<OutT1, OutT2, OutT3>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g), &out->a,&out->b,&out->c);
}
#define IPC_TUPLE_IN_7(t1, t2, t3, t4, t5, t6, t7) Tuple7<t1, t2, t3, t4, t5, t6, t7>
#define IPC_NAME_IN_7(t1, t2, t3, t4, t5, t6, t7) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7)
#define IPC_SYNC_MESSAGE_CONTROL7_3(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, otype1, otype2, otype3) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 7, 3, (itype1, itype2, itype3, itype4, itype5, itype6, itype7), (otype1, otype2, otype3))
IPC_SYNC_MESSAGE_CONTROL7_3(OpenCLIPCMsg_clEnqueueReadBuffer, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem buffer
	cl_bool, //! cl_bool blocking_read
	size_t, //! size_t offset
	size_t, //! size_t cb
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	std::vector<unsigned char>, //! void * ptr
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7, const t8& iarg8, const t9& iarg9, const t10& iarg10, const t11& iarg11, const t12& iarg12
#define IPC_COMMA_AND_12(x) x
template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L>
struct Tuple12 {
public:
typedef A TypeA;
typedef B TypeB;
typedef C TypeC;
typedef D TypeD;
typedef E TypeE;
typedef F TypeF;
typedef G TypeG;
typedef H TypeH;
typedef I TypeI;
typedef J TypeJ;
typedef K TypeK;
typedef L TypeL;
Tuple12() {}
Tuple12(
typename TupleTraits<A>::ParamType a,
typename TupleTraits<B>::ParamType b,
typename TupleTraits<C>::ParamType c,
typename TupleTraits<D>::ParamType d,
typename TupleTraits<E>::ParamType e,
typename TupleTraits<F>::ParamType f,
typename TupleTraits<G>::ParamType g,
typename TupleTraits<H>::ParamType h,
typename TupleTraits<I>::ParamType i,
typename TupleTraits<J>::ParamType j,
typename TupleTraits<K>::ParamType k,
typename TupleTraits<L>::ParamType l)
: a(a),b(b),c(c),d(d),e(e),f(f),g(g),h(h),i(i),j(j),k(k),l(l){}
A a;
B b;
C c;
D d;
E e;
F f;
G g;
H h;
I i;
J j;
K k;
L l;
};
template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11, class t12>
struct TupleTypes< Tuple12<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12> > {
typedef Tuple12<typename TupleTraits<t1>::ValueType, typename TupleTraits<t2>::ValueType, typename TupleTraits<t3>::ValueType, typename TupleTraits<t4>::ValueType, typename TupleTraits<t5>::ValueType, typename TupleTraits<t6>::ValueType, typename TupleTraits<t7>::ValueType, typename TupleTraits<t8>::ValueType, typename TupleTraits<t9>::ValueType, typename TupleTraits<t10>::ValueType, typename TupleTraits<t11>::ValueType, typename TupleTraits<t12>::ValueType> ValueTuple;
typedef Tuple12<typename TupleTraits<t1>::RefType, typename TupleTraits<t2>::RefType, typename TupleTraits<t3>::RefType, typename TupleTraits<t4>::RefType, typename TupleTraits<t5>::RefType, typename TupleTraits<t6>::RefType, typename TupleTraits<t7>::RefType, typename TupleTraits<t8>::RefType, typename TupleTraits<t9>::RefType, typename TupleTraits<t10>::RefType, typename TupleTraits<t11>::RefType, typename TupleTraits<t12>::RefType> RefTuple;
typedef Tuple12<typename TupleTraits<t1>::ParamType, typename TupleTraits<t2>::ParamType, typename TupleTraits<t3>::ParamType, typename TupleTraits<t4>::ParamType, typename TupleTraits<t5>::ParamType, typename TupleTraits<t6>::ParamType, typename TupleTraits<t7>::ParamType, typename TupleTraits<t8>::ParamType, typename TupleTraits<t9>::ParamType, typename TupleTraits<t10>::ParamType, typename TupleTraits<t11>::ParamType, typename TupleTraits<t12>::ParamType> ParamTuple;
};
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline Tuple12<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&, T11&, T12&> MakeRefTuple(T1& a,T2& b,T3& c,T4& d,T5& e,T6& f,T7& g,T8& h,T9& i,T10& j,T11& k,T12& l) {
  return Tuple12<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&, T11&, T12&>(a,b,c,d,e,f,g,h,i,j,k,l);
}
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
struct ParamTraits< Tuple12<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> > {
  typedef Tuple12<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);
WriteParam(m, p.h);
WriteParam(m, p.i);
WriteParam(m, p.j);
WriteParam(m, p.k);
WriteParam(m, p.l);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) &&ReadParam(m, iter, &r->h) &&ReadParam(m, iter, &r->i) &&ReadParam(m, iter, &r->j) &&ReadParam(m, iter, &r->k) &&ReadParam(m, iter, &r->l) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");
LogParam(p.h, l); l->append(", ");
LogParam(p.i, l); l->append(", ");
LogParam(p.j, l); l->append(", ");
LogParam(p.k, l); l->append(", ");
LogParam(p.l, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class InT9, class InT10, class InT11, class InT12, class OutT1, class OutT2, class OutT3>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple12<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8, InT9, InT10, InT11, InT12>&in,
        Tuple3<OutT1, OutT2, OutT3>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h),base::internal::UnwrapTraits<InT9>::Unwrap(in.i),base::internal::UnwrapTraits<InT10>::Unwrap(in.j),base::internal::UnwrapTraits<InT11>::Unwrap(in.k),base::internal::UnwrapTraits<InT12>::Unwrap(in.l), &out->a,&out->b,&out->c);
}
#define IPC_TUPLE_IN_12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) Tuple12<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12>
#define IPC_NAME_IN_12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8, iarg9, iarg10, iarg11, iarg12)
#define IPC_SYNC_MESSAGE_CONTROL12_3(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12, otype1, otype2, otype3) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 12, 3, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12), (otype1, otype2, otype3))
IPC_SYNC_MESSAGE_CONTROL12_3(OpenCLIPCMsg_clEnqueueReadBufferRect, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem buffer
	cl_bool, //! cl_bool blocking_read
	std::vector<unsigned char>, //! const size_t * buffer_origin
	std::vector<unsigned char>, //! const size_t * host_origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t buffer_row_pitch
	size_t, //! size_t buffer_slice_pitch
	size_t, //! size_t host_row_pitch
	size_t, //! size_t host_slice_pitch
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	std::vector<unsigned char>, //! void * ptr
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_8(t1, t2, t3, t4, t5, t6, t7, t8) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7, const t8& iarg8
#define IPC_COMMA_AND_8(x) x
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct ParamTraits< Tuple8<T1, T2, T3, T4, T5, T6, T7, T8> > {
  typedef Tuple8<T1, T2, T3, T4, T5, T6, T7, T8> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);
WriteParam(m, p.h);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) &&ReadParam(m, iter, &r->h) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");
LogParam(p.h, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class OutT1, class OutT2>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple8<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8>&in,
        Tuple2<OutT1, OutT2>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h), &out->a,&out->b);
}
#define IPC_TUPLE_IN_8(t1, t2, t3, t4, t5, t6, t7, t8) Tuple8<t1, t2, t3, t4, t5, t6, t7, t8>
#define IPC_NAME_IN_8(t1, t2, t3, t4, t5, t6, t7, t8) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8)
#define IPC_SYNC_MESSAGE_CONTROL8_2(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, otype1, otype2) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 8, 2, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8), (otype1, otype2))
IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueWriteBuffer, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem buffer
	cl_bool, //! cl_bool blocking_write
	size_t, //! size_t offset
	size_t, //! size_t cb
	std::vector<unsigned char>, //! const void * ptr
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_13(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7, const t8& iarg8, const t9& iarg9, const t10& iarg10, const t11& iarg11, const t12& iarg12, const t13& iarg13
#define IPC_COMMA_AND_13(x) x
template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J, class K, class L, class M>
struct Tuple13 {
public:
typedef A TypeA;
typedef B TypeB;
typedef C TypeC;
typedef D TypeD;
typedef E TypeE;
typedef F TypeF;
typedef G TypeG;
typedef H TypeH;
typedef I TypeI;
typedef J TypeJ;
typedef K TypeK;
typedef L TypeL;
typedef M TypeM;
Tuple13() {}
Tuple13(
typename TupleTraits<A>::ParamType a,
typename TupleTraits<B>::ParamType b,
typename TupleTraits<C>::ParamType c,
typename TupleTraits<D>::ParamType d,
typename TupleTraits<E>::ParamType e,
typename TupleTraits<F>::ParamType f,
typename TupleTraits<G>::ParamType g,
typename TupleTraits<H>::ParamType h,
typename TupleTraits<I>::ParamType i,
typename TupleTraits<J>::ParamType j,
typename TupleTraits<K>::ParamType k,
typename TupleTraits<L>::ParamType l,
typename TupleTraits<M>::ParamType m)
: a(a),b(b),c(c),d(d),e(e),f(f),g(g),h(h),i(i),j(j),k(k),l(l),m(m){}
A a;
B b;
C c;
D d;
E e;
F f;
G g;
H h;
I i;
J j;
K k;
L l;
M m;
};
template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10, class t11, class t12, class t13>
struct TupleTypes< Tuple13<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13> > {
typedef Tuple13<typename TupleTraits<t1>::ValueType, typename TupleTraits<t2>::ValueType, typename TupleTraits<t3>::ValueType, typename TupleTraits<t4>::ValueType, typename TupleTraits<t5>::ValueType, typename TupleTraits<t6>::ValueType, typename TupleTraits<t7>::ValueType, typename TupleTraits<t8>::ValueType, typename TupleTraits<t9>::ValueType, typename TupleTraits<t10>::ValueType, typename TupleTraits<t11>::ValueType, typename TupleTraits<t12>::ValueType, typename TupleTraits<t13>::ValueType> ValueTuple;
typedef Tuple13<typename TupleTraits<t1>::RefType, typename TupleTraits<t2>::RefType, typename TupleTraits<t3>::RefType, typename TupleTraits<t4>::RefType, typename TupleTraits<t5>::RefType, typename TupleTraits<t6>::RefType, typename TupleTraits<t7>::RefType, typename TupleTraits<t8>::RefType, typename TupleTraits<t9>::RefType, typename TupleTraits<t10>::RefType, typename TupleTraits<t11>::RefType, typename TupleTraits<t12>::RefType, typename TupleTraits<t13>::RefType> RefTuple;
typedef Tuple13<typename TupleTraits<t1>::ParamType, typename TupleTraits<t2>::ParamType, typename TupleTraits<t3>::ParamType, typename TupleTraits<t4>::ParamType, typename TupleTraits<t5>::ParamType, typename TupleTraits<t6>::ParamType, typename TupleTraits<t7>::ParamType, typename TupleTraits<t8>::ParamType, typename TupleTraits<t9>::ParamType, typename TupleTraits<t10>::ParamType, typename TupleTraits<t11>::ParamType, typename TupleTraits<t12>::ParamType, typename TupleTraits<t13>::ParamType> ParamTuple;
};
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline Tuple13<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&, T11&, T12&, T13&> MakeRefTuple(T1& a,T2& b,T3& c,T4& d,T5& e,T6& f,T7& g,T8& h,T9& i,T10& j,T11& k,T12& l,T13& m) {
  return Tuple13<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&, T11&, T12&, T13&>(a,b,c,d,e,f,g,h,i,j,k,l,m);
}
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
struct ParamTraits< Tuple13<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> > {
  typedef Tuple13<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);
WriteParam(m, p.h);
WriteParam(m, p.i);
WriteParam(m, p.j);
WriteParam(m, p.k);
WriteParam(m, p.l);
WriteParam(m, p.m);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) &&ReadParam(m, iter, &r->h) &&ReadParam(m, iter, &r->i) &&ReadParam(m, iter, &r->j) &&ReadParam(m, iter, &r->k) &&ReadParam(m, iter, &r->l) &&ReadParam(m, iter, &r->m) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");
LogParam(p.h, l); l->append(", ");
LogParam(p.i, l); l->append(", ");
LogParam(p.j, l); l->append(", ");
LogParam(p.k, l); l->append(", ");
LogParam(p.l, l); l->append(", ");
LogParam(p.m, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class InT9, class InT10, class InT11, class InT12, class InT13, class OutT1, class OutT2>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple13<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8, InT9, InT10, InT11, InT12, InT13>&in,
        Tuple2<OutT1, OutT2>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h),base::internal::UnwrapTraits<InT9>::Unwrap(in.i),base::internal::UnwrapTraits<InT10>::Unwrap(in.j),base::internal::UnwrapTraits<InT11>::Unwrap(in.k),base::internal::UnwrapTraits<InT12>::Unwrap(in.l),base::internal::UnwrapTraits<InT13>::Unwrap(in.m), &out->a,&out->b);
}
#define IPC_TUPLE_IN_13(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13) Tuple13<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13>
#define IPC_NAME_IN_13(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8, iarg9, iarg10, iarg11, iarg12, iarg13)
#define IPC_SYNC_MESSAGE_CONTROL13_2(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12, itype13, otype1, otype2) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 13, 2, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12, itype13), (otype1, otype2))
IPC_SYNC_MESSAGE_CONTROL13_2(OpenCLIPCMsg_clEnqueueWriteBufferRect, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem buffer
	cl_bool, //! cl_bool blocking_write
	std::vector<unsigned char>, //! const size_t * buffer_origin
	std::vector<unsigned char>, //! const size_t * host_origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t buffer_row_pitch
	size_t, //! size_t buffer_slice_pitch
	size_t, //! size_t host_row_pitch
	size_t, //! size_t host_slice_pitch
	std::vector<unsigned char>, //! const void * ptr
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueCopyBuffer, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem src_buffer
	cl_pointer, //! cl_mem dst_buffer
	size_t, //! size_t src_offset
	size_t, //! size_t dst_offset
	size_t, //! size_t cb
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class InT9, class InT10, class InT11, class InT12, class OutT1, class OutT2>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple12<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8, InT9, InT10, InT11, InT12>&in,
        Tuple2<OutT1, OutT2>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h),base::internal::UnwrapTraits<InT9>::Unwrap(in.i),base::internal::UnwrapTraits<InT10>::Unwrap(in.j),base::internal::UnwrapTraits<InT11>::Unwrap(in.k),base::internal::UnwrapTraits<InT12>::Unwrap(in.l), &out->a,&out->b);
}
#define IPC_TUPLE_IN_12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) Tuple12<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12>
#define IPC_NAME_IN_12(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8, iarg9, iarg10, iarg11, iarg12)
#define IPC_SYNC_MESSAGE_CONTROL12_2(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12, otype1, otype2) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 12, 2, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, itype11, itype12), (otype1, otype2))
IPC_SYNC_MESSAGE_CONTROL12_2(OpenCLIPCMsg_clEnqueueCopyBufferRect, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem src_buffer
	cl_pointer, //! cl_mem dst_buffer
	std::vector<unsigned char>, //! const size_t * src_origin
	std::vector<unsigned char>, //! const size_t * dst_origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t src_row_pitch
	size_t, //! size_t src_slice_pitch
	size_t, //! size_t dst_row_pitch
	size_t, //! size_t dst_slice_pitch
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_9(t1, t2, t3, t4, t5, t6, t7, t8, t9) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7, const t8& iarg8, const t9& iarg9
#define IPC_COMMA_AND_9(x) x
template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
struct Tuple9 {
public:
typedef A TypeA;
typedef B TypeB;
typedef C TypeC;
typedef D TypeD;
typedef E TypeE;
typedef F TypeF;
typedef G TypeG;
typedef H TypeH;
typedef I TypeI;
Tuple9() {}
Tuple9(
typename TupleTraits<A>::ParamType a,
typename TupleTraits<B>::ParamType b,
typename TupleTraits<C>::ParamType c,
typename TupleTraits<D>::ParamType d,
typename TupleTraits<E>::ParamType e,
typename TupleTraits<F>::ParamType f,
typename TupleTraits<G>::ParamType g,
typename TupleTraits<H>::ParamType h,
typename TupleTraits<I>::ParamType i)
: a(a),b(b),c(c),d(d),e(e),f(f),g(g),h(h),i(i){}
A a;
B b;
C c;
D d;
E e;
F f;
G g;
H h;
I i;
};
template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9>
struct TupleTypes< Tuple9<t1, t2, t3, t4, t5, t6, t7, t8, t9> > {
typedef Tuple9<typename TupleTraits<t1>::ValueType, typename TupleTraits<t2>::ValueType, typename TupleTraits<t3>::ValueType, typename TupleTraits<t4>::ValueType, typename TupleTraits<t5>::ValueType, typename TupleTraits<t6>::ValueType, typename TupleTraits<t7>::ValueType, typename TupleTraits<t8>::ValueType, typename TupleTraits<t9>::ValueType> ValueTuple;
typedef Tuple9<typename TupleTraits<t1>::RefType, typename TupleTraits<t2>::RefType, typename TupleTraits<t3>::RefType, typename TupleTraits<t4>::RefType, typename TupleTraits<t5>::RefType, typename TupleTraits<t6>::RefType, typename TupleTraits<t7>::RefType, typename TupleTraits<t8>::RefType, typename TupleTraits<t9>::RefType> RefTuple;
typedef Tuple9<typename TupleTraits<t1>::ParamType, typename TupleTraits<t2>::ParamType, typename TupleTraits<t3>::ParamType, typename TupleTraits<t4>::ParamType, typename TupleTraits<t5>::ParamType, typename TupleTraits<t6>::ParamType, typename TupleTraits<t7>::ParamType, typename TupleTraits<t8>::ParamType, typename TupleTraits<t9>::ParamType> ParamTuple;
};
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Tuple9<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&> MakeRefTuple(T1& a,T2& b,T3& c,T4& d,T5& e,T6& f,T7& g,T8& h,T9& i) {
  return Tuple9<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&>(a,b,c,d,e,f,g,h,i);
}
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct ParamTraits< Tuple9<T1, T2, T3, T4, T5, T6, T7, T8, T9> > {
  typedef Tuple9<T1, T2, T3, T4, T5, T6, T7, T8, T9> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);
WriteParam(m, p.h);
WriteParam(m, p.i);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) &&ReadParam(m, iter, &r->h) &&ReadParam(m, iter, &r->i) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");
LogParam(p.h, l); l->append(", ");
LogParam(p.i, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class InT9, class OutT1, class OutT2, class OutT3>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple9<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8, InT9>&in,
        Tuple3<OutT1, OutT2, OutT3>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h),base::internal::UnwrapTraits<InT9>::Unwrap(in.i), &out->a,&out->b,&out->c);
}
#define IPC_TUPLE_IN_9(t1, t2, t3, t4, t5, t6, t7, t8, t9) Tuple9<t1, t2, t3, t4, t5, t6, t7, t8, t9>
#define IPC_NAME_IN_9(t1, t2, t3, t4, t5, t6, t7, t8, t9) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8, iarg9)
#define IPC_SYNC_MESSAGE_CONTROL9_3(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, otype1, otype2, otype3) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 9, 3, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9), (otype1, otype2, otype3))
IPC_SYNC_MESSAGE_CONTROL9_3(OpenCLIPCMsg_clEnqueueReadImage, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem image
	cl_bool, //! cl_bool blocking_read
	std::vector<unsigned char>, //! const size_t * origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t row_pitch
	size_t, //! size_t slice_pitch
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	std::vector<unsigned char>, //! void * ptr
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

#define IPC_TYPE_IN_10(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) const t1& iarg1, const t2& iarg2, const t3& iarg3, const t4& iarg4, const t5& iarg5, const t6& iarg6, const t7& iarg7, const t8& iarg8, const t9& iarg9, const t10& iarg10
#define IPC_COMMA_AND_10(x) x
template <class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
struct Tuple10 {
public:
typedef A TypeA;
typedef B TypeB;
typedef C TypeC;
typedef D TypeD;
typedef E TypeE;
typedef F TypeF;
typedef G TypeG;
typedef H TypeH;
typedef I TypeI;
typedef J TypeJ;
Tuple10() {}
Tuple10(
typename TupleTraits<A>::ParamType a,
typename TupleTraits<B>::ParamType b,
typename TupleTraits<C>::ParamType c,
typename TupleTraits<D>::ParamType d,
typename TupleTraits<E>::ParamType e,
typename TupleTraits<F>::ParamType f,
typename TupleTraits<G>::ParamType g,
typename TupleTraits<H>::ParamType h,
typename TupleTraits<I>::ParamType i,
typename TupleTraits<J>::ParamType j)
: a(a),b(b),c(c),d(d),e(e),f(f),g(g),h(h),i(i),j(j){}
A a;
B b;
C c;
D d;
E e;
F f;
G g;
H h;
I i;
J j;
};
template <class t1, class t2, class t3, class t4, class t5, class t6, class t7, class t8, class t9, class t10>
struct TupleTypes< Tuple10<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10> > {
typedef Tuple10<typename TupleTraits<t1>::ValueType, typename TupleTraits<t2>::ValueType, typename TupleTraits<t3>::ValueType, typename TupleTraits<t4>::ValueType, typename TupleTraits<t5>::ValueType, typename TupleTraits<t6>::ValueType, typename TupleTraits<t7>::ValueType, typename TupleTraits<t8>::ValueType, typename TupleTraits<t9>::ValueType, typename TupleTraits<t10>::ValueType> ValueTuple;
typedef Tuple10<typename TupleTraits<t1>::RefType, typename TupleTraits<t2>::RefType, typename TupleTraits<t3>::RefType, typename TupleTraits<t4>::RefType, typename TupleTraits<t5>::RefType, typename TupleTraits<t6>::RefType, typename TupleTraits<t7>::RefType, typename TupleTraits<t8>::RefType, typename TupleTraits<t9>::RefType, typename TupleTraits<t10>::RefType> RefTuple;
typedef Tuple10<typename TupleTraits<t1>::ParamType, typename TupleTraits<t2>::ParamType, typename TupleTraits<t3>::ParamType, typename TupleTraits<t4>::ParamType, typename TupleTraits<t5>::ParamType, typename TupleTraits<t6>::ParamType, typename TupleTraits<t7>::ParamType, typename TupleTraits<t8>::ParamType, typename TupleTraits<t9>::ParamType, typename TupleTraits<t10>::ParamType> ParamTuple;
};
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline Tuple10<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&> MakeRefTuple(T1& a,T2& b,T3& c,T4& d,T5& e,T6& f,T7& g,T8& h,T9& i,T10& j) {
  return Tuple10<T1&, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, T10&>(a,b,c,d,e,f,g,h,i,j);
}
namespace IPC {
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
struct ParamTraits< Tuple10<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> > {
  typedef Tuple10<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10> param_type;
  static void Write(Message* m, const param_type& p) {
    WriteParam(m, p.a);
WriteParam(m, p.b);
WriteParam(m, p.c);
WriteParam(m, p.d);
WriteParam(m, p.e);
WriteParam(m, p.f);
WriteParam(m, p.g);
WriteParam(m, p.h);
WriteParam(m, p.i);
WriteParam(m, p.j);

  }
  static bool Read(const Message* m, PickleIterator* iter, param_type* r) {
    return (ReadParam(m, iter, &r->a) &&ReadParam(m, iter, &r->b) &&ReadParam(m, iter, &r->c) &&ReadParam(m, iter, &r->d) &&ReadParam(m, iter, &r->e) &&ReadParam(m, iter, &r->f) &&ReadParam(m, iter, &r->g) &&ReadParam(m, iter, &r->h) &&ReadParam(m, iter, &r->i) &&ReadParam(m, iter, &r->j) && true);
  }
  static void Log(const param_type& p, std::string* l) {
  LogParam(p.a, l); l->append(", ");
LogParam(p.b, l); l->append(", ");
LogParam(p.c, l); l->append(", ");
LogParam(p.d, l); l->append(", ");
LogParam(p.e, l); l->append(", ");
LogParam(p.f, l); l->append(", ");
LogParam(p.g, l); l->append(", ");
LogParam(p.h, l); l->append(", ");
LogParam(p.i, l); l->append(", ");
LogParam(p.j, l); l->append(", ");

  }
};
} // namespace IPC
template<class ObjT, class Method, class InT1, class InT2, class InT3, class InT4, class InT5, class InT6, class InT7, class InT8, class InT9, class InT10, class OutT1, class OutT2>
inline void DispatchToMethod(ObjT* obj, Method method,
        const Tuple10<InT1, InT2, InT3, InT4, InT5, InT6, InT7, InT8, InT9, InT10>&in,
        Tuple2<OutT1, OutT2>* out) {
  (obj->*method)(base::internal::UnwrapTraits<InT1>::Unwrap(in.a),base::internal::UnwrapTraits<InT2>::Unwrap(in.b),base::internal::UnwrapTraits<InT3>::Unwrap(in.c),base::internal::UnwrapTraits<InT4>::Unwrap(in.d),base::internal::UnwrapTraits<InT5>::Unwrap(in.e),base::internal::UnwrapTraits<InT6>::Unwrap(in.f),base::internal::UnwrapTraits<InT7>::Unwrap(in.g),base::internal::UnwrapTraits<InT8>::Unwrap(in.h),base::internal::UnwrapTraits<InT9>::Unwrap(in.i),base::internal::UnwrapTraits<InT10>::Unwrap(in.j), &out->a,&out->b);
}
#define IPC_TUPLE_IN_10(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) Tuple10<t1, t2, t3, t4, t5, t6, t7, t8, t9, t10>
#define IPC_NAME_IN_10(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10) MakeRefTuple(iarg1, iarg2, iarg3, iarg4, iarg5, iarg6, iarg7, iarg8, iarg9, iarg10)
#define IPC_SYNC_MESSAGE_CONTROL10_2(msg_class, itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10, otype1, otype2) \
	IPC_MESSAGE_DECL(SYNC, CONTROL, msg_class, 10, 2, (itype1, itype2, itype3, itype4, itype5, itype6, itype7, itype8, itype9, itype10), (otype1, otype2))
IPC_SYNC_MESSAGE_CONTROL10_2(OpenCLIPCMsg_clEnqueueWriteImage, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem image
	cl_bool, //! cl_bool blocking_write
	std::vector<unsigned char>, //! const size_t * origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t input_row_pitch
	size_t, //! size_t input_slice_pitch
	std::vector<unsigned char>, //! const void * ptr
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueCopyImage, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem src_image
	cl_pointer, //! cl_mem dst_image
	std::vector<unsigned char>, //! const size_t * src_origin
	std::vector<unsigned char>, //! const size_t * dst_origin
	std::vector<unsigned char>, //! const size_t * region
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueCopyImageToBuffer, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem src_image
	cl_pointer, //! cl_mem dst_buffer
	std::vector<unsigned char>, //! const size_t * src_origin
	std::vector<unsigned char>, //! const size_t * region
	size_t, //! size_t dst_offset
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueCopyBufferToImage, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_mem src_buffer
	cl_pointer, //! cl_mem dst_image
	size_t, //! size_t src_offset
	std::vector<unsigned char>, //! const size_t * dst_origin
	std::vector<unsigned char>, //! const size_t * region
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL8_2(OpenCLIPCMsg_clEnqueueNDRangeKernel, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_kernel kernel
	cl_uint, //! cl_uint work_dim
	std::vector<unsigned char>, //! const size_t * global_work_offset
	std::vector<unsigned char>, //! const size_t * global_work_size
	std::vector<unsigned char>, //! const size_t * local_work_size
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL4_2(OpenCLIPCMsg_clEnqueueTask, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_kernel kernel
	cl_uint, //! cl_uint num_events_in_wait_list
	std::vector<unsigned char>, //! const cl_event * event_wait_list
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_2(OpenCLIPCMsg_clEnqueueMarker, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer, //! cl_event * event
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL3_1(OpenCLIPCMsg_clEnqueueWaitForEvents, 
	cl_pointer, //! cl_command_queue command_queue
	cl_uint, //! cl_uint num_events
	std::vector<unsigned char>, //! const cl_event * event_list
	cl_pointer) //! return cl_int

IPC_SYNC_MESSAGE_CONTROL1_1(OpenCLIPCMsg_clEnqueueBarrier, 
	cl_pointer, //! cl_command_queue command_queue
	cl_pointer) //! return cl_int

#endif // OCL_MSG_H
