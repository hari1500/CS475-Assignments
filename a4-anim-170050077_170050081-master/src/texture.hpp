#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_
GLuint LoadTexture( const char * filename, int width, int height );
GLuint LoadTextureAll( const char * filename );
GLuint LoadTexturePNG( const char * filename );
void FreeTexture( GLuint texture );
#endif 
