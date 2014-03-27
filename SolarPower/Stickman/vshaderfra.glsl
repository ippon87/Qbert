
attribute   vec4 vPosition;
attribute   vec3 vNormal;

// output values that will be interpretated per-fragment
varying  vec3 fN;
varying  vec3 fE;
varying  vec3 fL;

uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 Projection;

void main()
{
    vec3 pos = (ModelView * vPosition).xyz;

    fN = normalize( ModelView*vec4(vNormal, 0.0) ).xyz;
    fE = normalize( -pos );
    fL = normalize( (ModelView*LightPosition).xyz - pos );
    
    gl_Position = Projection*ModelView*vPosition;
}
