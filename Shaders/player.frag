//POINT LIGHT
#version 330 core //version

uniform sampler2D tex0; //texture 0
uniform sampler2D tex1; //texture 1
uniform sampler2D norm_tex; //normal map texture

uniform vec3 lightPos; //light position
uniform vec3 lightColor; //light color

uniform float ambientStr; //ambient strength
uniform vec3 ambientColor; //ambient color

uniform float specStr; //specular strength
uniform float specPhong; //specular phong

uniform vec3 cameraPos; //camera position

in vec2 texCoord; //texture coordinates
in vec3 normCoord; //normal coordinates
in vec3 fragPos; //fragment position

in mat3 TBN; //TBN matrix

out vec4 FragColor; //output fragment color

void main () {
    
    //get the brick color
    vec4 brickColor = texture(tex0, texCoord);
    brickColor.a = 0.6; //set the alpha value of the brick to 0.6 (semi transparent)

    //get yae's color
    vec4 yaeColor = texture(tex1, texCoord);

    vec4 pixelColor; //stores the pixel color
    if (yaeColor.a < 0.1) {
        //use the brick color if the alpha value of yae is transparent
        pixelColor = brickColor;
    } else {
        //otherwise use yae's color
        pixelColor = yaeColor;
    }

    //get the normal map rgb color
    vec3 normal = texture(norm_tex,  texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0); //convert rgb(0 to 1) to xyz (-1 to 1)
    normal = normalize(TBN * normal); //multiply the normal with the tbn matrix

    //determine the light direction
    vec3 lightDir = normalize(lightPos - fragPos);
    
    //ambient
    vec3 ambient = ambientStr * ambientColor;

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    //specular
    vec3 viewDir = normalize(cameraPos - fragPos); //calculate for the view direction
    vec3 reflectDir = reflect(-lightDir, normal); //calculate for the reflection direction

    //calculate specular light
    float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specPhong);
    vec3 specular = spec * specStr * lightColor;

    //point light values from https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    float constant = 1.0f;
    float linear = 0.014f;
    float quadratic = 0.0007f;
    float distance = length(lightPos - fragPos); //calculate the euclidean distance between the light and fragment
    float attenuation = 1.0f / (constant + linear * distance + quadratic * (distance * distance)); //formula from  https://ogldev.org/www/tutorial20/tutorial20.html) for more flexible point light

    //scale the diffuse, ambient, and speculation based on the distance of the object from the light source
    ambient *= attenuation; //scale the ambient light with the attenuation
    diffuse *= attenuation; //scale the diffuse light with the attenuation
    specular *= attenuation; //scale the specular light with the attenuation 

    FragColor = vec4(ambient + diffuse + specular, 1.0f) * pixelColor;
    //FragColor = texture(tex0, texCoord);
}