//POINT LIGHT
#version 330 core //version

uniform sampler2D tex0;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform float ambientStr;
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor; //(r, g, b, a)

void main () {
    //FragColor = vec4(0.5f, 0.f, 0.f, 1.f); //colors all pixels red

    vec3 normal = normalize(normCoord);
    vec3 lightDir = normalize(lightPos - fragPos);
    
    //diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    //ambient
    vec3 ambientCol = ambientStr * ambientColor;

    //specular
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specPhong);
    vec3 specCol = spec * specStr * lightColor;

    //point light
    //attenuation
    float distance = length(lightPos - fragPos); //calculate the euclidean distance between the light and fragment
    float attenuation = 1.0f / (distance * distance); //calculate the intensity of the light using the inverse law

    //scale the diffuse, ambient, and speculation based on the distance of the object from the light source
    //further from the light source -> less light, closer to the light source -> more light
    //therefore, the calculation of the attenuation follows the inverse law
    diffuse *= attenuation; //scale the diffuse light with the attenuation
    ambientCol *= attenuation; //scale the ambient light with the attenuation
    specCol *= attenuation; //scale the specular light with the attenuation

    FragColor = vec4(specCol + diffuse + ambientCol, 1.0f) * texture(tex0, texCoord);
}