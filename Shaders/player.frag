#version 330 core //version

struct DirectionalLight {
    vec3 direction; //direction of the directional light
    vec3 color; //color of the light
    float intensity; //intesity of the light
    float ambientStr; //ambient strength
    float specStr; //specular strength
    float specPhong; //specular phong
};

struct PointLight {
    vec3 position; //position of the point light
    vec3 color; //color of the light
    float intensity; //intesity of the light
    float ambientStr; //ambient strength
    float specStr; //specular strength
    float specPhong; //specular phong
    float constant; //constant factor for attentuation
    float linear; //linear factor for attentuation
    float quadratic; //quadratic factor for attentuation
};

struct SpotLight {
    vec3 position; //position of the spot light
    vec3 direction; //direction of the spot light
    vec3 color; //color of the light
    float intensity; //intesity of the light
    float ambientStr; //ambient strength
    float specStr; //specular strength
    float specPhong; //specular phong
    float constant; //constant factor for attentuation
    float linear; //linear factor for attentuation
    float quadratic; //quadratic factor for attentuation
    float cutoff;
};

uniform sampler2D tex0; //index of the texture
uniform sampler2D norm_tex;

uniform DirectionalLight directionalLight; //directional light
uniform SpotLight spotLight; //point light

uniform vec3 cameraPos; //camera position

in vec2 texCoord; //texture coordinates
in vec3 normCoord; //normal coordinates
in vec3 fragPos; //fragment position

in mat3 TBN;

out vec4 FragColor; //output fragment color

vec3 calculateDirectionalLight (DirectionalLight light) {
    vec3 normal = texture(norm_tex,  texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); //convert rgb(0 to 1) to xyz (-1 to 1)
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(-light.direction);

    //ambient
    vec3 ambient = light.ambientStr * light.color;

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color;
    
    // specular
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.1f), light.specPhong);
    vec3 specular = light.specStr * spec * light.color;  

    //scale the ambient, diffuse, and specular based on the light intensity
    ambient *= light.intensity;
    diffuse *= light.intensity;
    specular *= light.intensity;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light) {
    vec3 normal = texture(norm_tex,  texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); //convert rgb(0 to 1) to xyz (-1 to 1)
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(light.position - fragPos); //light direction
    
    //ambient
    vec3 ambient = light.ambientStr * light.color;

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * light.color;

    // specular
    vec3 viewDir = normalize(cameraPos - fragPos); //view direction
    vec3 reflectDir = reflect(-lightDir, normal); //reflection direction

    //calculate specular light
    float spec = pow(max(dot(reflectDir, viewDir), 0.1f), light.specPhong);
    vec3 specular = light.specStr * spec * light.color;  

    //calculate attenuation
    float distance = length(light.position - fragPos); //calculate the euclidean distance between the light and fragment
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //formula from  https://ogldev.org/www/tutorial20/tutorial20.html) for more flexible point light

    //scale the diffuse, ambient, and speculation based on the distance of the object from the light source
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //scale the ambient, diffuse, and specular based on the light intensity
    ambient *= light.intensity;
    diffuse *= light.intensity;
    specular *= light.intensity;

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(SpotLight light) {
    vec3 normal = texture(norm_tex,  texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); //convert rgb(0 to 1) to xyz (-1 to 1)
    normal = normalize(TBN * normal);

    vec3 lightDir = normalize(light.position - fragPos); //light direction
    
    //ambient
    vec3 ambient = light.ambientStr * light.color;

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * light.color;

    // specular
    vec3 viewDir = normalize(cameraPos - fragPos); //view direction
    vec3 reflectDir = reflect(-lightDir, normal); //reflection direction

    //calculate specular light
    float spec = pow(max(dot(reflectDir, viewDir), 0.1f), light.specPhong);
    vec3 specular = light.specStr * spec * light.color;  

    //calculate attenuation
    float distance = length(light.position - fragPos); //calculate the euclidean distance between the light and fragment
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); //formula from  https://ogldev.org/www/tutorial20/tutorial20.html) for more flexible point light

    //scale the diffuse, ambient, and speculation based on the distance of the object from the light source
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    //scale the ambient, diffuse, and specular based on the light intensity
    ambient *= light.intensity;
    diffuse *= light.intensity;
    specular *= light.intensity;

    float theta = dot(lightDir, normalize(-light.direction));
    if (theta > light.cutoff) {
        return ambient + diffuse + specular; 
    } else {
        return ambient;
    }
}

void main () {
    vec3 total; //stores the sum of the lights
    
    //calculate directional light
    //total += calculateDirectionalLight(directionalLight);

    //calculate spot light
    total += calculateSpotLight(spotLight);

    FragColor = vec4(total, 1.0f) * texture(tex0, texCoord);
}