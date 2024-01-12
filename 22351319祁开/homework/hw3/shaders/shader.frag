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
//定向光源
struct DirLight{
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
const DirLight dirLight=DirLight(
    vec3(-.5,-.5,-.5),
    
    vec3(.1,.1,.1),
    vec3(.8,.8,.8),
    vec3(1.,1.,1.)
);
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir);
//点光源
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
#define MAX_POINT_LIGHTS 3

const PointLight lights[MAX_POINT_LIGHTS]=PointLight[](
    PointLight(
        vec3(2.,0.,0.),
        1.,
        .09,
        .032,
        vec3(.0,.1,.1),
        vec3(.0,.8,.8),
        vec3(0.,1.,1.)
    ),
    PointLight(
        vec3(0.,3.,0.),
        1.,
        .09,
        .032,
        vec3(.1,.1,.1),
        vec3(.8,.8,.8),
        vec3(1.,1.,1.)
    ),
    PointLight(
        vec3(0.,0.,2.),
        1.,
        .09,
        .032,
        vec3(.1,.1,.0),
        vec3(.8,.8,.0),
        vec3(1.,1.,0.)
    )
);
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
    
    //多光源渲染
    vec3 norm=normalize(Normal);
    vec3 viewDir=normalize(ubo.viewPos-FragPos);
    vec3 result=vec3(0.);
    //定向光
    result+=CalcDirLight(dirLight,norm,viewDir);
    //点光源
    for(int i=0;i<MAX_POINT_LIGHTS;i++)
    result+=CalcPointLight(lights[i],norm,FragPos,viewDir);
    outColor=vec4(result,1.);
    
}
vec3 CalcDirLight(DirLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir=normalize(-light.direction);
    // 漫反射着色
    float diff=max(dot(normal,lightDir),0.);
    // 镜面光着色
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec=pow(max(dot(viewDir,reflectDir),0.),64.f);
    // 合并结果
    vec3 ambient=light.ambient*vec3(texture(texSampler,fragTexCoord));
    vec3 diffuse=light.diffuse*diff*vec3(texture(texSampler,fragTexCoord));
    vec3 specular=light.specular*spec*vec3(texture(texSampler,fragTexCoord));
    return(ambient+diffuse+specular);
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
    
    ambient*=attenuation;
    diffuse*=attenuation;
    specular*=attenuation;
    
    return(ambient+diffuse+specular);
}