#if 0
__kernel void yuv_rgba(__global uchar *buffer_pool, 
  int y_plane_offset,
  int u_plane_offset,
  int v_plane_offset,
  int y_stride,
  int uv_stride,
 __write_only image2d_t output
 ){
  int gIdx, gIdy;
  gIdx = get_global_id(0);
  gIdy = get_global_id(1);

  __global uchar2 *src_y_buffer = (__global uchar2 *)(buffer_pool + y_plane_offset);
  __global uchar *src_u_buffer = buffer_pool + u_plane_offset;
  __global uchar *src_v_buffer = buffer_pool + v_plane_offset;

  
  
  float2 y0, y1;
  float u, v;

  y0 = convert_float2(src_y_buffer[(gIdy << 1) * (y_stride >> 1) + gIdx]);
  y1 = convert_float2(src_y_buffer[((gIdy << 1) + 1) * (y_stride >> 1) + gIdx]);
  u = (float)(src_u_buffer[gIdy * uv_stride + gIdx]);
  v = (float)(src_v_buffer[gIdy * uv_stride + gIdx]);
  //write to read only kernel

  
  float4 leftTop = (float4)(1.164f * (y0.x - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y0.x - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y0.x - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 rightTop = (float4)(1.164f * (y0.y - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y0.y - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y0.y - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 leftBottom = (float4)(1.164f * (y1.x - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y1.x - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y1.x - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 RightBottom = (float4)(1.164f * (y1.y - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y1.y - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y1.y - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  
  write_imagef(output, (int2)(gIdx * 2, gIdy * 2), leftTop / (float4)255.0f); 
  write_imagef(output, (int2)(gIdx * 2 + 1, gIdy * 2), rightTop / (float4)255.0f);
  write_imagef(output, (int2)(gIdx * 2, gIdy * 2 + 1), leftBottom / (float4)255.0f);
  write_imagef(output, (int2)(gIdx * 2 + 1, gIdy * 2 + 1), RightBottom / (float4)255.0f);
  
}
#endif

__kernel void yuv_rgba(__global uchar *buffer_pool, 
  int y_plane_offset,
  int u_plane_offset,
  int v_plane_offset,
  int y_stride,
  int uv_stride,
 __write_only image2d_t output_y,
 __write_only image2d_t output_v,
 __write_only image2d_t output_u
 ){
  int gIdx, gIdy;
  gIdx = get_global_id(0);
  gIdy = get_global_id(1);


  __global uchar2 *src_y_buffer = (__global uchar2 *)(buffer_pool + y_plane_offset);
  __global uchar *src_u_buffer = buffer_pool + u_plane_offset;
  __global uchar *src_v_buffer = buffer_pool + v_plane_offset;

  
  
  float2 y0, y1;
  float u, v;

  y0 = convert_float2(src_y_buffer[(gIdy << 1) * (y_stride >> 1) + gIdx]);
  y1 = convert_float2(src_y_buffer[((gIdy << 1) + 1) * (y_stride >> 1) + gIdx]);
  u = (float)(src_u_buffer[gIdy * uv_stride + gIdx]);
  v = (float)(src_v_buffer[gIdy * uv_stride + gIdx]);
  
  
  float4 leftTop = (float4)(y0.x, 0, 0, 255.0f);
  float4 rightTop = (float4)(y0.y, 0, 0, 255.0f);
  float4 leftBottom = (float4)(y1.x,0,0,255.0f);
  float4 RightBottom = (float4)(y1.y, 0, 0, 255.0f);
  float4 u_4 = (float4)(u, 0, 0, 255.0f);
  float4 v_4 = (float4)(v, 0, 0, 255.0f);
  


  write_imagef(output_y, (int2)(gIdx * 2, gIdy * 2), leftTop / (float4)255.0f); 
  write_imagef(output_y, (int2)(gIdx * 2 + 1, gIdy * 2), rightTop / (float4)255.0f);
  write_imagef(output_y, (int2)(gIdx * 2, gIdy * 2 + 1), leftBottom / (float4)255.0f);
  write_imagef(output_y, (int2)(gIdx * 2 + 1, gIdy * 2 + 1), RightBottom / (float4)255.0f);
  
  write_imagef(output_v, (int2)(gIdx, gIdy), v_4 / (float4)255.0f);
  write_imagef(output_u, (int2)(gIdx, gIdy), u_4 / (float4)255.0f);
  
 
}


