/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <vector>
#include <map>
#include <list>

using bufferIT = std::map<BufferID, std::vector<uint8_t>>::iterator;

/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    //buffers
    std::map<BufferID, std::vector<uint8_t>> buffers;
    //variables to help with selecting free ID:
    uint64_t nextFreeID;
    std::list<uint64_t> freeIDs;
    //vertex pullers
    struct Head {
        bool enabled;
        AttributeType type;
        uint64_t stride;
        uint64_t offset;
        BufferID buffer;
        Head() {
            enabled = false;
            buffer = emptyID;
            stride = 0;
            offset = 0;
            type = AttributeType::EMPTY;
        }
    };
    struct VertexPuller {
        bool indexing;
        IndexType index_type;
        BufferID index_buffer;
        Head heads[maxAttributes];
        VertexPuller() {
            indexing = false;
            index_type = IndexType::UINT8;
            index_buffer = emptyID;
        }
    };
    std::map<VertexPullerID, VertexPuller> vertexPullers;
    VertexPullerID currVertexPuller;
    //program
    struct Program {
        VertexShader vertex_shader;
        FragmentShader fragment_shader;
        Uniforms uniforms;
        AttributeType types[maxAttributes];
        Program() {
            vertex_shader = nullptr;
            fragment_shader = nullptr;
            for (auto type : types) {
                type = AttributeType::EMPTY;
            }
        }
        void attachShaders(VertexShader vs, FragmentShader fs) {
            vertex_shader = vs;
            fragment_shader = fs;
        }
    };
    std::map<ProgramID, Program> programs;
    ProgramID currProgram;
    //framebuffer
    //TODO shouldn't be in header, but kinda didn't work outside
    struct FrameBuffer {
        uint32_t width;
        uint32_t height;
        std::vector<uint8_t>* color_buffer;
        std::vector<float>* depth_buffer;
        FrameBuffer() {
            width = 0;
            height = 0;
            color_buffer = nullptr;
            depth_buffer = nullptr;
        }
        void FrameBuffer::set_up(uint32_t new_width, uint32_t new_height) {
            color_buffer = new std::vector<uint8_t>(size_t((uint64_t)new_width * (uint64_t)new_height * 4));
            depth_buffer = new std::vector<float>(size_t((uint64_t)new_width * (uint64_t)new_height));
            width = new_width;
            height = new_height;
        }
    };
    FrameBuffer* currFrameBuffer;
    /// @}
};


