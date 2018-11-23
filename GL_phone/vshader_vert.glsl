#version 330 core

in vec3 vPosition;  //顶点坐标
in vec3 vNormal;    //

uniform vec3 lightPos; //光源坐标

uniform mat4 modelViewProjMatrix;  //三个矩阵合一
uniform mat4 modelViewMatrix;      //模视矩阵

out vec4 color;  //输出插值后的颜色

// Phong 光照模型的实现 (per-vertex shading)顶点颜色插值

void main()
{
	gl_Position = modelViewProjMatrix * vec4(vPosition, 1.0);  //将对象变换投影到相机坐标系下

	// 将顶点坐标变换到相机坐标系
	vec4 vertPos_cameraspace = modelViewProjMatrix * vec4(vPosition, 1.0);
	vec4 lightPos_cameraspace = modelViewMatrix * vec4(lightPos, 1.0);
	
	vec3 V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
	vec3 N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;
	vec3 L = lightPos_cameraspace.xyz / lightPos_cameraspace.w;
	
	// 设置三维物体的材质属性
	vec3 ambiColor =  vec3(0.2, 0.2, 0.2);
	vec3 diffColor =  vec3(0.8, 0.4, 0.7);
	vec3 specColor = vec3(0.3, 0.3, 0.3);
	
	// 计算N，L，V，R四个向量并归一化
	vec3 N_norm = normalize(N);
	vec3 L_norm = normalize(L - V);
	vec3 V_norm = normalize(-V);
	vec3 R_norm = reflect(-L_norm, N_norm);
	
	// 计算漫反射系数和镜面反射系数
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0);
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);
	
	// 高光系数
	float shininess = 10.0;
	
	// 计算最终每个顶点的输出颜色
	color = vec4(ambiColor + 
				 diffColor * lambertian +
				 specColor * pow(specular, shininess), 1.0);
}