#ifndef PACKING_H_
#define PACKING_H_

vec4 unpackUnorm4x8(in uint p)
{
	return vec4(float(p & 0x000000FF) / 255.0,
				  float((p & 0x0000FF00) >> 8) / 255.0,
				  float((p & 0x00FF0000) >> 16) / 255.0,
				  float((p & 0xFF000000) >> 24) / 255.0);
}

#endif