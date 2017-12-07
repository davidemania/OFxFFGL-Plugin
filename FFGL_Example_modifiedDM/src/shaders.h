//
//  shaders.h
//  FFGL_Example_modifiedDM
//
//  Created by Davide Manià on 15/01/14.
//
//

#ifndef FFGL_Example_modifiedDM_shaders_h
#define FFGL_Example_modifiedDM_shaders_h

#define STRINGIFY(x) #x

void replaceString(string& source, const string& old_text, const string& new_text)
{
    size_t pos = 0;
    while ((pos = source.find(old_text, pos)) != std::string::npos)
    {
        source.replace(pos, old_text.length(), new_text);
        pos += new_text.length();
    }
}

void prepareShaderCode(string& source)
{
    replaceString(source, "NEWLINE", "\n");
}

// simple vertex shader with GLSL 1.20

static string defaultVertexShader = " \
#version 120 NEWLINE \
#extension GL_ARB_texture_rectangle : enable NEWLINE \
#extension GL_EXT_gpu_shader4 : enable NEWLINE \
" STRINGIFY(
            
void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = gl_Color;
}
                                                               
);

 //simple fragment shader

static string defaultFragmentShader = " \
#version 120 NEWLINE \
#extension GL_ARB_texture_rectangle : enable NEWLINE \
#extension GL_EXT_gpu_shader4 : enable NEWLINE \
" STRINGIFY(

uniform sampler2DRect texture0;
                                                                 
void main()
{
    //Getting coordinates of the current pixel in texture
    vec2 pos = gl_TexCoord[0].xy;
    //Getting the pixel color from the texture texture0 in pos
    vec4 color = texture2DRect(texture0, pos);
    //Changing the color - invert red, green, blue components
    color.r = 1.0 - color.r;
    color.g = 1.0 - color.g;
    color.b = 1.0 - color.b;
    //Output the color of shader
    gl_FragColor = color;
}
                                                                 
);

#endif // FFGL_Example_modifiedDM_shaders_h
