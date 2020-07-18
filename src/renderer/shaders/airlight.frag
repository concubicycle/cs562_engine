#version 430 core
out vec4 FragColor;



in VS_OUT {    
    float optical_thickness;
} fs_in;



void main()
{
    int s0 = gl_FrontFacing ? 1 : -1;
    float contribution = s0;
    
    FragColor = vec4(s0 * contribution) * vec4(0.3, 0.3, 0.3, 0.3);
    //FragColor = vec4(0, 1, 0, 1);
}
