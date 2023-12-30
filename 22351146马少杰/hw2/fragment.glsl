#version 410
out vec4 FragmentColour; // 输出的片段颜色

in vec3 FragNormal; // 片段法线向量
in vec2 FragUV; // 片段的 UV 坐标
in vec4 spacePos; // 片段的世界空间位置

uniform sampler2D texSphere; // 球体贴图采样器
uniform bool diffuse; // 是否使用漫反射光照

void main(void) {
    if(diffuse) { // 如果开启了漫反射
        vec4 sunColor = vec4(1.0); // 太阳光颜色，这里设为白色
        vec3 lightRay = normalize(vec3(0.0) - spacePos.xyz); // 光线方向，指向太阳（假设位置为原点）
        // 使用太阳光颜色、贴图和光照法线计算最终颜色，限制最小亮度为0.2
        FragmentColour = texture(texSphere, FragUV) * sunColor * max(0.2, dot(FragNormal, lightRay));
    }
    else // 如果未开启漫反射
        FragmentColour = texture(texSphere, FragUV); // 直接使用贴图颜色作为最终颜色
}
