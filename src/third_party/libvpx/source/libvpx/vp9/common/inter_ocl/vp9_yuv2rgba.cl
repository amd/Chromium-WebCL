__kernel void yuv_rgba(__global uchar *buffer_pool,
  __global uchar* read_only_buffer_pool,
  int y_plane_offset,
  int u_plane_offset,
  int v_plane_offset,
  int y_stride,
  int uv_stride,
 __write_only image2d_t output){
  int gIdx, gIdy;
  gIdx = get_global_id(0);
  gIdy = get_global_id(1);

  __global uchar2 *src_y_buffer = (__global uchar2 *)(buffer_pool + y_plane_offset);
  __global uchar *src_u_buffer = buffer_pool + u_plane_offset;
  __global uchar *src_v_buffer = buffer_pool + v_plane_offset;

  __global uchar2 *y_buffer = (__global uchar2 *)(read_only_buffer_pool + y_plane_offset);
  __global uchar *u_buffer = read_only_buffer_pool + u_plane_offset;
  __global uchar *v_buffer = read_only_buffer_pool + v_plane_offset;
  float2 y0, y1;
  float u, v;
  uchar2 c_y0, c_y1;
  uchar c_u, c_v;
  c_y0 = src_y_buffer[(gIdy << 1) * (y_stride >> 1) + gIdx];
  c_y1 = src_y_buffer[((gIdy << 1) + 1) * (y_stride >> 1) + gIdx];
  c_u = src_u_buffer[gIdy * uv_stride + gIdx];
  c_v = src_v_buffer[gIdy * uv_stride + gIdx];
  //write to read only kernel
 y0 = convert_float2(c_y0);
 y1 = convert_float2(c_y1);
 u = (float)(c_u);
 v = (float)(c_v);
 
  
  float4 leftTop = (float4)(1.164f * (y0.x - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y0.x - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y0.x - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 rightTop = (float4)(1.164f * (y0.y - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y0.y - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y0.y - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 leftBottom = (float4)(1.164f * (y1.x - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y1.x - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y1.x - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  float4 RightBottom = (float4)(1.164f * (y1.y - 16.0f) + 1.596f * (v - 128.0f), 1.164f * (y1.y - 16.0f) - 0.813f * (v - 128.0f) - 0.392f * (u - 128.0f), 1.164f * (y1.y - 16.0f) + 2.017f * (u - 128.0f), 255.0f);
  write_imagef(output, (int2)(gIdx * 2, gIdy * 2), leftTop / (float4)255.0f); 
  write_imagef(output, (int2)(gIdx * 2 + 1, gIdy * 2), rightTop / (float4)255.0f);
  write_imagef(output, (int2)(gIdx * 2, gIdy * 2 + 1), leftBottom / (float4)255.0f);
  write_imagef(output, (int2)(gIdx * 2 + 1, gIdy * 2 + 1), RightBottom / (float4)255.0f);
  
  y_buffer[(gIdy << 1) * (y_stride >> 1) + gIdx] = c_y0;
  y_buffer[((gIdy << 1) + 1) * (y_stride >> 1) + gIdx] = c_y1;
  u_buffer[gIdy * uv_stride + gIdx] = c_u;
  v_buffer[gIdy * uv_stride + gIdx] = c_v;

}


