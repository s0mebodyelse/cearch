# Cearch
inspired by https://github.com/tsoding/seroost

Local Search engine with Web Interface

# Build the project
make 

# Run Cearch
./cearch 8080 docs.gl index

# time without threading
./docs.gl -> 169.1s
./docs.gl/gl4/ -> 7.0s

# Time to build the index with threads
./docs.gl 
    1 thread-> 112.7s
    4 thread-> 43.6s
    8 thread-> 35.2s

./docs.gl/gl4 
    1 thread -> 7.0s
    4 thread -> 3.8s
    8 thread -> 3.4s

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
./docs.gl/gl3/glBindTexture.xhtml => 0.62961
./docs.gl/gl4/glBindTexture.xhtml => 0.60404
./docs.gl/gl4/glGetTexParameter.xhtml => 0.583459
./docs.gl/gl2/glGetTexLevelParameter.xhtml => 0.582312
./docs.gl/gl3/glTexParameter.xhtml => 0.575568
./docs.gl/gl4/glFramebufferTexture.xhtml => 0.549065
./docs.gl/gl4/glGetTexLevelParameter.xhtml => 0.546375
./docs.gl/gl4/glTexParameter.xhtml => 0.545217
./docs.gl/gl3/glFramebufferTexture.xhtml => 0.542859
./docs.gl/gl3/glGetTexLevelParameter.xhtml => 0.53365
./docs.gl/gl3/glGetTexParameter.xhtml => 0.507547
./docs.gl/gl4/glTextureView.xhtml => 0.496796
./docs.gl/gl2/glGetTexParameter.xhtml => 0.480713
./docs.gl/gl2/glTexParameter.xhtml => 0.46361
./docs.gl/specs/gl.xml => 0.460636
