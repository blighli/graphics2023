#ifndef PACKING_H_
#define PACKING_H_

#include "layout.glsl"

VertexAttribute unpackVertexData(in VertexInput i)
{
	return VertexAttribute(i.slot0.xyz, i.slot1.xyz, i.slot2.xy);
}

#endif