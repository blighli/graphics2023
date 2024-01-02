#version 450
//in
layout(location=0)in vec3 Normal;
layout(location=1)in vec2 fragTexCoord;
layout(location=2)in vec3 FragPos;
//out
layout(location=0)out vec4 outColor;
//uniform
layout(set=0,binding=0)uniform UniformBufferObject{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
}ubo;
layout(set=0,binding=1)uniform UboInstance{
    mat4 model;
}uboInstance;
layout(set=1,binding=0)uniform sampler2D texSampler;
struct PointLight{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir);
const PointLight light=PointLight(
    vec3(0.,0.,0.),
    1.,
    .09,
    .032,
    vec3(.1,.1,.1),
    vec3(.8,.8,.8),
    vec3(1.,1.,1.)
);
void main(){
    float distance=length(light.position-FragPos);
    if(distance>.3){//为物体指定index遇到问题，暂时用距离替代
        //物体着色
        vec3 norm=normalize(Normal);
        vec3 viewDir=normalize(ubo.viewPos-FragPos);
        vec3 result=vec3(0.);
        result+=CalcPointLight(light,norm,FragPos,ubo.viewPos);
        outColor=vec4(result,1.);
    }
    else{
        //光源渲染
        outColor=texture(texSampler,fragTexCoord);
    }
}
vec3 CalcPointLight(PointLight light,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    vec3 lightDir=normalize(light.position-fragPos);
    // 漫反射着色
    float diff=max(dot(normal,lightDir),0.);
    // 镜面光着色
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec=pow(max(dot(viewDir,reflectDir),0.),64.f);
    // 衰减
    float distance=length(light.position-fragPos);
    float attenuation=1./(light.constant+light.linear*distance+light.quadratic*(distance*distance));
    // 合并结果
    vec3 ambient=light.ambient*vec3(texture(texSampler,fragTexCoord));
    vec3 diffuse=light.diffuse*diff*vec3(texture(texSampler,fragTexCoord));
    vec3 specular=light.specular*spec*vec3(texture(texSampler,fragTexCoord));
    
    diffuse*=attenuation;
    specular*=attenuation;
    return(ambient+diffuse+specular);
}