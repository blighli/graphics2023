#version 450

layout(binding=0)uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
}ubo;
layout(binding=1)uniform UboInstance{
    mat4 model;
}uboInstance;
layout(location=0)in vec3 inPosition;
layout(location=1)in vec3 inNomal;
layout(location=2)in vec2 inTexCoord;

layout(location=0)out vec3 Normal;
layout(location=1)out vec2 fragTexCoord;
layout(location=2)out vec3 FragPos;
void main(){
    gl_Position=ubo.proj*ubo.view*uboInstance.model*vec4(inPosition,1.);
    FragPos=vec3(uboInstance.model*vec4(inPosition,1.));
    Normal=mat3(transpose(inverse(uboInstance.model)))*inNomal;
    fragTexCoord=inTexCoord;
}