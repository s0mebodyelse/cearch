# Cearch
inspired by https://github.com/tsoding/seroost

Local Search engine with Web Interface

# Build the project
make 

# Run Cearch
./cearch 8080 docs.gl index

# time without threading
./docs.gl -> 227.4s
./docs.gl/gl4/ -> 7.5s

# Time to build the index
./docs.gl 
    1 thread-> 225.6s
    4 thread-> 241.8s
    8 thread-> 325.4s

./docs.gl/gl4 
    1 thread -> 7.6s
    4 thread -> 8.1s
    8 thread -> 9.9s

# Output on docs.gl/gl4
Body: input-text=bind+texture%2C+to+buffer.
./docs.gl/gl4/glBindTexture.xhtml => 0.5508
./docs.gl/gl4/glGetTexParameter.xhtml => 0.54052
./docs.gl/gl4/glFramebufferTexture.xhtml => 0.505955
./docs.gl/gl4/glTexParameter.xhtml => 0.505093
./docs.gl/gl4/glGetTexLevelParameter.xhtml => 0.495879
./docs.gl/gl4/glTextureView.xhtml => 0.459218
./docs.gl/gl4/glGetTexImage.xhtml => 0.325044
./docs.gl/gl4/glSamplerParameter.xhtml => 0.296377
./docs.gl/gl4/glTexImage2D.xhtml => 0.285143
./docs.gl/gl4/glBindTextureUnit.xhtml => 0.272632

# Output on docs.gl
Body: input-text=bind+texture%2C+to+buffer.
./docs.gl/gl3/glBindTexture.xhtml => 0.648574
./docs.gl/gl4/glBindTexture.xhtml => 0.622337
./docs.gl/gl4/glGetTexParameter.xhtml => 0.601042
./docs.gl/gl2/glGetTexLevelParameter.xhtml => 0.599861
./docs.gl/gl3/glTexParameter.xhtml => 0.592914
./docs.gl/gl4/glFramebufferTexture.xhtml => 0.565869
./docs.gl/gl4/glGetTexLevelParameter.xhtml => 0.56382
./docs.gl/gl4/glTexParameter.xhtml => 0.561648
./docs.gl/gl3/glFramebufferTexture.xhtml => 0.559374
./docs.gl/gl3/glGetTexLevelParameter.xhtml => 0.549945
./docs.gl/gl3/glGetTexParameter.xhtml => 0.522843
./docs.gl/gl4/glTextureView.xhtml => 0.511865
./docs.gl/gl2/glGetTexParameter.xhtml => 0.4952
./docs.gl/specs/gl.xml => 0.477704
