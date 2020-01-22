
inline bool glCheckError_(char *file, uint32 line) {
    GLenum _glError = glGetError();
    if (_glError != GL_NO_ERROR) {
        Print("OpenGL error (%s:%d): 0x%x (%d)\n", file, line, _glError, _glError);
        return true;
    }

    return false;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__);

void LoadShader(const char *vertPath, const char *fragPath, Shader *shader) {
    FILE *file = fopen(vertPath, "r");

    // FUCK fgets! Just use something good

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        shader->vertSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        shader->vertSrc = (char *)malloc(shader->vertSize + 1);
        int index = 0;
        char c = fgetc(file);
        while (c != EOF) {
            shader->vertSrc[index++] = c;
            c = fgetc(file);
        }

        shader->vertSrc[index] = 0;

        fclose(file);
    }
    else {
        Print("Error opening file %s", vertPath);
    }

    file = fopen(fragPath, "r");

    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        shader->fragSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        shader->fragSrc = (char *)malloc(shader->fragSize + 1);
        int index = 0;
        char c = fgetc(file);
        while (c != EOF) {
            shader->fragSrc[index++] = c;
            c = fgetc(file);
        }

        shader->fragSrc[index] = 0;

        fclose(file);
    }
    else {
        Print("Error opening file %s", fragPath);
    }
}

bool ShaderCompiled(GLuint shader, char **infoLog) {
    int32 isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        if (infoLog != NULL) {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            maxLength = 256;

            *infoLog = (GLchar *)malloc(sizeof(GLchar) * maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, (GLchar *)*infoLog);
        }

        Print(*infoLog);
        
        glDeleteShader(shader);
    }

    return isCompiled;
}


void CompileShader(Shader *shader, uint32 uniformCount, const char **uniformNames) {
    char *infoLog = NULL;
    
    if (shader->vertSrc != NULL) {
        shader->vertID = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(shader->vertID, 1, (const GLchar**)&shader->vertSrc, (GLint *)&shader->vertSize);
        glCheckError();
        glCompileShader(shader->vertID);
        glCheckError();

        Print("checking vert shader");
        ShaderCompiled(shader->vertID, &infoLog);
    }
    if (shader->fragSrc != NULL) {
        shader->fragID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(shader->fragID, 1, (const GLchar**)&shader->fragSrc, (GLint *)&shader->fragSize);
        glCheckError();
        glCompileShader(shader->fragID);
        glCheckError();

        Print("checking frag shader");
        ShaderCompiled(shader->vertID, &infoLog);
    }


    shader->programID = glCreateProgram();

    
    if (shader->vertID != 0) {
        glAttachShader(shader->programID, shader->vertID);
    }
    if (shader->fragID != 0) {
        glAttachShader(shader->programID, shader->fragID);
    }

    glLinkProgram(shader->programID);

    
    
    shader->uniformCount = uniformCount;

    shader->uniforms = (ShaderUniform *)malloc(sizeof(ShaderUniform) * uniformCount);
    
    for (int i = 0; i < uniformCount; i++) {
        ShaderUniform *uniform = &shader->uniforms[i];
        const char *uniformName = uniformNames[i];

        uint32 nameLen = strlen(uniformName);
        uniform->name = (char *)malloc(nameLen + 1);
        memcpy(uniform->name, uniformName, nameLen);
        uniform->name[nameLen] = 0;
        
        uniform->id = glGetUniformLocation(shader->programID, uniform->name);
        
        if (uniform->id >= 0) {
            Print("Setting uniform %s", uniform->name);    
        }
        else {
            Print("failed to set %s", uniform->name);    
        }
        ShaderCompiled(shader->vertID, &infoLog);
    }
}

void SetShader(Shader *shader) {
    glUseProgram(shader->programID);
    //glCheckError();
}


void OpenGL_InitMesh(Mesh *mesh) {
    GLuint vertexBuffer;

    glGenBuffers(1, &vertexBuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, mesh->size, mesh->data, GL_STATIC_DRAW);

    GLuint indexBuffer;
    glGenBuffers(1, &indexBuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indexCount * sizeof(uint32), mesh->indices, GL_STATIC_DRAW);

    mesh->vertBufferID = vertexBuffer;
    mesh->indexBufferID = indexBuffer;
    
}

void DrawCircle2D(vec2 position, real32 radius) {
        
    Shader *shader = &Game->texturedQuadShader;
    SetShader(shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh *mesh = &Game->quad;

    //mat4 model = TRS(V3(position.x - radius * 0.5f, position.y + radius * 0.5f, 0), IdentityQuaternion(), V3(radius));
    mat4 model = TRS(V3(position.x, position.y, 0), IdentityQuaternion(), V3(radius));
     
    Sprite *texture = &Game->circleTexture;

    //vec4 topLeft = mvp * V4(gameMem->quad.verts[0], 1.0f);
    glUniformMatrix4fv(shader->uniforms[0].id, 1, GL_FALSE, model.data);
    glUniformMatrix4fv(shader->uniforms[1].id, 1, GL_FALSE, Game->camera.viewProjection.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    glUniform1i(shader->uniforms[2].id, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
    // 2nd attribute buffer : texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)((sizeof(vec3) * mesh->vertCount)));
        
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
}

void LoadSprite(Sprite *sprite, char *path) {
    int32 x, y, n;
    uint8 *data = stbi_load(path, &x, &y, &n, 4);

    AllocateSprite(sprite, x, y);

    memcpy(sprite->data, data, sprite->size);

    free(data);
}

void OpenGL_InitTexture(Sprite *texture) {
    glGenTextures(1, (GLuint *)&texture->textureID);
    glCheckError();

    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    glCheckError();

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data);
    glCheckError();

    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
}


void DrawSprite(vec2 position, vec2 scale, Sprite *texture) {
        
    Shader *shader = &Game->texturedQuadShader;
    SetShader(shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh *mesh = &Game->quad;

    //mat4 model = TRS(V3(position.x - radius * 0.5f, position.y + radius * 0.5f, 0), IdentityQuaternion(), V3(radius));
    mat4 model = TRS(V3(position.x, position.y, 0), IdentityQuaternion(), V3(scale.x, scale.y, 1));

    //vec4 topLeft = mvp * V4(gameMem->quad.verts[0], 1.0f);
    glUniformMatrix4fv(shader->uniforms[0].id, 1, GL_FALSE, model.data);
    glUniformMatrix4fv(shader->uniforms[1].id, 1, GL_FALSE, Game->camera.viewProjection.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    glUniform1i(shader->uniforms[2].id, 0);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
    // 2nd attribute buffer : texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)((sizeof(vec3) * mesh->vertCount)));
        
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
}


void DrawRect(vec2 pos, vec2 scale, vec4 color) {
    Shader *shader = &Game->shader;
    SetShader(shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh *mesh = &Game->quad;
    
    mat4 model = TRS(V3(pos.x, pos.y, 0), IdentityQuaternion(), V3(scale.x, scale.y, 0.0f));
     
    glUniformMatrix4fv(shader->uniforms[0].id, 1, GL_FALSE, model.data);
    glUniformMatrix4fv(shader->uniforms[1].id, 1, GL_FALSE, Game->camera.viewProjection.data);

    glUniform4fv(shader->uniforms[2].id, 1, color.data);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
}


void DrawMouseCursor(vec2 position, real32 size) {
        
    Shader *shader = &Game->texturedQuadShader;
    SetShader(shader);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh *mesh = &Game->quad;

    mat4 model = TRS(V3(position.x + (size * 0.5f), position.y - (size * 0.5f), 0), IdentityQuaternion(), V3(size));
     
    Sprite *texture = &Game->mouseCursorTexture;

    //vec4 topLeft = mvp * V4(gameMem->quad.verts[0], 1.0f);
    glUniformMatrix4fv(shader->uniforms[0].id, 1, GL_FALSE, model.data);
    glUniformMatrix4fv(shader->uniforms[1].id, 1, GL_FALSE, Game->camera.viewProjection.data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureID);
    glUniform1i(shader->uniforms[2].id, 0);
        

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBufferID);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
    // 2nd attribute buffer : texcoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)((sizeof(vec3) * mesh->vertCount)));
        
    glDrawElements(GL_TRIANGLES, mesh->indexCount, GL_UNSIGNED_INT, (GLvoid *)0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);    
}

void DrawTile() {
    
}
