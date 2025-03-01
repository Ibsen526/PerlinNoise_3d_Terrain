#version 330 core

layout(location = 0) out vec4 f_color; //Gibt die Farb und Positionswerte zurück zum Programm

uniform vec2 resolution;
uniform vec2 offset;

float random (in vec2 st) {
    //return fract(sin(dot(st.xy, vec2(12.9898f,78.233f))) * 43758.5453123f);
    return fract(sin(dot(st.xy, vec2(15.0f, 80.0f))) * 43758.5453123f);
}

// 2D Noise based on Morgan McGuire @morgan3d
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0f, 0.0f));
    float c = random(i + vec2(0.0f, 1.0f));
    float d = random(i + vec2(1.0f, 1.0f));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0f-2.0f*f);
    //vec2 u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0f - u.x) +
            (d - b) * u.x * u.y;
}

void main() {
    vec2 st = gl_FragCoord.xy / resolution.xy;

    vec2 dis = st + (offset / 10.0f);

    // Scale the coordinate system to see
    // some noise in action
    vec2 pos = vec2(dis * 5.0f); //Zooms in and out

    // Use the noise function
    float n = noise(pos);

    //gl_FragColor = vec4(0.0f, 0.0f, 1.0f - n, 1.0f); //blue
    gl_FragColor = vec4(vec3(n), 1.0f); //black
}