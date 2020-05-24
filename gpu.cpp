/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>



/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
    nextFreeID = 0;
    currVertexPuller = nullptr;
    currProgram = nullptr;
    currFrameBuffer = nullptr;
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
    BufferID id = emptyID;
    if (freeIDs.empty()) {
        id = nextFreeID;
        nextFreeID++;
    }
    else {
        id = freeIDs.front();
        freeIDs.pop_front();
    }
    auto buffer = std::vector<uint8_t>(size_t(size));
    buffers.emplace(id, move(buffer));
    return id; 
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
    auto it = buffers.find(buffer);
    if (it != buffers.end()) {
        BufferID removedID = it->first;
        freeIDs.push_back(removedID);
        buffers.erase(it);
    }
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>
    auto it = buffers.find(buffer);
    if (it != buffers.end()) {
        std::copy((uint8_t*) data, (uint8_t*) data + size, it->second.begin() + offset);
    }
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.
    auto it = buffers.find(buffer);
    if (it != buffers.end()) {
        std::copy(it->second.begin() + offset, it->second.begin() + offset + size, (uint8_t*)data);
    }
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
    if (buffers.find(buffer) != buffers.end()) return true;
    else return false; 
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
    VertexPullerID id = emptyID;
    if (freeIDs.empty()) {
        id = nextFreeID;
        nextFreeID++;
    }
    else {
        id = freeIDs.front();
        freeIDs.pop_front();
    }

    auto vertex = VertexPuller();
    vertexPullers.emplace(id, vertex);

    return id;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        VertexPullerID removedID = it->first;
        freeIDs.push_back(removedID);
        vertexPullers.erase(it);
    }
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        it->second.heads[head].buffer = buffer;
        it->second.heads[head].offset = offset;
        it->second.heads[head].stride = stride;
        it->second.heads[head].type = type;
    }
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        it->second.indexing = true;
        it->second.index_type = type;
        it->second.index_buffer = buffer;
    }
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        it->second.heads[head].enabled = true;
    }
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        it->second.heads[head].enabled = false;
    }
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) {
        currVertexPuller = &vertexPullers[vao];
    }
    else currVertexPuller = nullptr;
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
    currVertexPuller = nullptr;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
    auto it = vertexPullers.find(vao);
    if (it != vertexPullers.end()) return true;
    else return false;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
    ProgramID id = emptyID;
    if (freeIDs.empty()) {
        id = nextFreeID;
        nextFreeID++;
    }
    else {
        id = freeIDs.front();
        freeIDs.pop_front();
    }

    auto program = Program();
    programs.emplace(id, program);

    return id;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
    auto it = programs.find(prg);
    if (it != programs.end()) {
        ProgramID removedID = it->first;
        freeIDs.push_back(removedID);
        programs.erase(it);
    }
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.attachShaders(vs, fs);
    }
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.types[attrib] = type;
    }
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
    auto it = programs.find(prg);
    if (it != programs.end()) {
        currProgram = &(it->second);
    }
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>
    auto it = programs.find(prg);
    if (it != programs.end()) return true;
    else return false;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.uniforms.uniform[uniformId].v1 = d;
    }
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.uniforms.uniform[uniformId].v2 = d;
    }
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.uniforms.uniform[uniformId].v3 = d;
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.uniforms.uniform[uniformId].v4 = d;
    }
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
    auto it = programs.find(prg);
    if (it != programs.end()) {
        it->second.uniforms.uniform[uniformId].m4 = d;
    }
}

/// @}


/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.
    currFrameBuffer = new FrameBuffer();
    currFrameBuffer->set_up(width, height);
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.
    delete currFrameBuffer;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
    currFrameBuffer->color_buffer->resize(size_t((uint64_t)width * (uint64_t)height * 4));
    currFrameBuffer->depth_buffer->resize(size_t((uint64_t)width * (uint64_t)height));
    currFrameBuffer->width = width;
    currFrameBuffer->height = height;
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
    return currFrameBuffer->color_buffer->data();
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
    return currFrameBuffer->depth_buffer->data();
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
    if (currFrameBuffer == nullptr) return 0;
    return currFrameBuffer->width;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
    if (currFrameBuffer == nullptr) return 0;
    return currFrameBuffer->height;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
    //TODO co s cisly mezi (0,1)??
    
    uint8_t* color = getFramebufferColor();
    float* depth = getFramebufferDepth();

    uint64_t max = (uint64_t) getFramebufferHeight() * (uint64_t) getFramebufferWidth();

    for (int i = 0; i < max; i++) {
        depth[i] = 2;
    }

    uint8_t red, green, blue, alpha;
    if (r <= 0) red = 0;
    else if (r >= 1) red = 255;
    else red = r * 255;

    if (g <= 0) green = 0;
    else if (g >= 1) green = 255;
    else green = g * 255;

    if (b <= 0) blue = 0;
    else if (b >= 1) blue = 255;
    else blue = b * 255;

    if (a <= 0) alpha = 0;
    else if (a >= 1) alpha = 255;
    else alpha = a * 255;

    max *= 4; //max pro color buffer
    int order = 0;

    for (int i = 0; i < max; i++) {
        if (order == 0) {
            color[i] = red;
            order++;
        }
        else if (order == 1) {
            color[i] = green;
            order++;
        }
        else if (order == 2) {
            color[i] = blue;
            order++;
        }
        else {
            color[i] = alpha;
            order = 0;
        }
    }
}

/* @brief Function extracts and returns one InVertex from currVertexPuller settings.
   @param timesCalled used as index in index mode, as id in non-index
   @return Extracted InVertex.
 */
InVertex GPU::fetchInVertex(uint32_t timesCalled) {
    InVertex iv;
    if (currVertexPuller->indexing) {
        //indexing
        void* data = calloc(1, sizeof(uint32_t));
        if (currVertexPuller->index_type == IndexType::UINT8) {
            getBufferData(currVertexPuller->index_buffer, timesCalled * sizeof(uint8_t), sizeof(uint8_t), data);
            iv.gl_VertexID = *(uint8_t*)data;
        }
        else if (currVertexPuller->index_type == IndexType::UINT16) {
            getBufferData(currVertexPuller->index_buffer, timesCalled * sizeof(uint16_t), sizeof(uint16_t), data);
            iv.gl_VertexID = *(uint16_t*)data;
        }
        else {
            //indextype::UINT32
            getBufferData(currVertexPuller->index_buffer, timesCalled * sizeof(uint32_t), sizeof(uint32_t), data);
            iv.gl_VertexID = *(uint32_t*)data;
        }
    }
    else {
        //not indexing
        iv.gl_VertexID = timesCalled;
    }

    for (int i = 0; i < maxAttributes; i++) {
        Head* head = &currVertexPuller->heads[i];

        if (head->enabled) {
            
            if (!isBuffer(head->buffer)) continue;

            void* data = calloc(1, sizeof(glm::vec4));
            uint32_t offset = head->offset + head->stride * iv.gl_VertexID;

            if (head->type == AttributeType::FLOAT) {
                getBufferData(head->buffer, offset, sizeof(float), data);
                iv.attributes[i].v1 = *(float*)data;
                //printf("%f\n", iv.attributes[i].v1);
            }
            else if (head->type == AttributeType::VEC2) {
                getBufferData(head->buffer, offset, sizeof(glm::vec2), data);
                iv.attributes[i].v2 = *(glm::vec2*) data;
                //printf("%f\n", *(float*)data);
                //printf("X: %f\nY: %f\n\n", iv.attributes[i].v2.x, iv.attributes[i].v2.y);
            }
            else if (head->type == AttributeType::VEC3) {
                getBufferData(head->buffer, offset, sizeof(glm::vec3), data);
                iv.attributes[i].v3 = *(glm::vec3*)data;
                //printf("X: %f\nY: %f\nZ: %f\n\n", iv.attributes[i].v3.x, iv.attributes[i].v3.y, iv.attributes[i].v3.z);
            }
            else if (head->type == AttributeType::VEC4) {
                getBufferData(head->buffer, offset, sizeof(glm::vec4), data);
                iv.attributes[i].v4 = *(glm::vec4*)data;
            }
            //EMPTY type ommited
            free(data);
        }
    }
    
    return iv;
}

void GPU::clipPlane(std::list<Triangle*>::iterator it) {
    Triangle* t = (*it);
    OutVertex* a = &(t->point[0]);
    OutVertex* b = &(t->point[1]);
    OutVertex* c = &(t->point[2]);

    //near plane
    bool a_out, b_out, c_out;
    bool no_new = true;
    if (-a->gl_Position.w <= a->gl_Position.x && -a->gl_Position.w <= a->gl_Position.y && -a->gl_Position.w <= a->gl_Position.z) a_out = false;
    else a_out = true;
    if (-b->gl_Position.w <= b->gl_Position.x && -b->gl_Position.w <= b->gl_Position.y && -b->gl_Position.w <= b->gl_Position.z) b_out = false;
    else b_out = true;
    if (-c->gl_Position.w <= c->gl_Position.x && -c->gl_Position.w <= c->gl_Position.y && -c->gl_Position.w <= c->gl_Position.z) c_out = false;
    else c_out = true;

    int out_cnt = a_out + b_out + c_out;
    if (out_cnt > 2) {
        //all out, whole triagle gets thrown out
        t->valid = false;
        return;
    }
    else if (out_cnt == 2) {
        //two points out of clipping space
        //TODO
    }
    else if (out_cnt == 1) {
        //one point out of clipping space
        //TODO
    }
    //if no point is out, nothing happens

    //far plane
    if (no_new) {
        //no clipping done so we can use original vertexes
        if (a->gl_Position.w >= a->gl_Position.x && a->gl_Position.w >= a->gl_Position.y && a->gl_Position.w >= a->gl_Position.z) a_out = false;
        else a_out = true;
        if (b->gl_Position.w >= b->gl_Position.x && b->gl_Position.w >= b->gl_Position.y && b->gl_Position.w >= b->gl_Position.z) b_out = false;
        else b_out = true;
        if (c->gl_Position.w >= c->gl_Position.x && c->gl_Position.w >= c->gl_Position.y && c->gl_Position.w >= c->gl_Position.z) c_out = false;
        else c_out = true;
    }
    else {
        //TODO figure out bools differently
    }

    out_cnt = a_out + b_out + c_out;
    if (out_cnt > 2) {
        t->valid = false;
    }
    else if (out_cnt == 2) {
        //two points out of clipping space
        //TODO
    }
    else if (out_cnt == 1) {
        //one point out of clipping space
        //TODO
    }
    //if no point is out, nothing happens
}

void GPU::interpolate(InFragment* inF, Triangle* t) {
    OutVertex a = t->point[0];
    OutVertex b = t->point[1];
    OutVertex c = t->point[2];

    Vector v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = Dot(v0, v0);
    float d01 = Dot(v0, v1);
    float d11 = Dot(v1, v1);
    float d20 = Dot(v2, v0);
    float d21 = Dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float l0 = (d11 * d20 - d01 * d21) / denom;
    float l1 = (d00 * d21 - d01 * d20) / denom;
    float l2 = 1.0f - l0 - l1;

    float h0 = a.gl_Position.w;
    float h1 = b.gl_Position.w;
    float h2 = c.gl_Position.w;

    for (int i = 0; i < maxAttributes; i++) {
        AttributeType type = currProgram->types[i];
        if()
    }
}

void GPU::createFragment(Triangle* t, float x, float y) {
    uint8_t* buffer = getFramebufferColor();
    uint32_t ix = (uint32_t)std::floor(x);
    uint32_t iy = (uint32_t)std::floor(y);
    InFragment inF;
    inF.gl_FragCoord.x = x;
    inF.gl_FragCoord.y = y;
    interpolate(&inF, t);

    OutFragment outF;
    currProgram->fragment_shader(outF, inF, currProgram->uniforms);
    
    /* temp draw
    buffer[iy * currFrameBuffer->width * 4 + ix * 4] = 255;
    buffer[iy * currFrameBuffer->width * 4 + ix * 4 + 1] = 255;
    buffer[iy * currFrameBuffer->width * 4 + ix * 4 + 2] = 255;
    */
}

void GPU::createFragments(Triangle* t) {
    uint32_t width = getFramebufferWidth();
    uint32_t height = getFramebufferHeight();

    glm::vec4* a = &(t->point[0].gl_Position);
    glm::vec4* b = &(t->point[1].gl_Position);
    glm::vec4* c = &(t->point[2].gl_Position);

    uint32_t minX = (uint32_t) std::max(std::min(std::min(std::floor(a->x), std::floor(b->x)), std::floor(c->x)), 0.f);
    uint32_t minY = (uint32_t) std::max(std::min(std::min(std::floor(a->y), std::floor(b->y)), std::floor(c->y)), 0.f);
    uint32_t maxX = std::min(width, (uint32_t) std::max(std::ceil(a->x), std::max(std::ceil(b->x), std::ceil(c->x))));
    uint32_t maxY = std::min(width, (uint32_t) std::max(std::ceil(a->y), std::max(std::ceil(b->y), std::ceil(c->y))));

    float abdely = b->y - a->y;
    float abdelx = b->x - a->x;
    float bcdely = c->y - b->y;
    float bcdelx = c->x - b->x;
    float cadely = a->y - c->y;
    float cadelx = a->x - c->x;

    //This is the worst, stupid slow version, because faster didn't work
    float abfunc, bcfunc, cafunc;
    for (float x = (float)minX + 0.5f; x < maxX; x += 1.f) {
        for (float y = (float)minY + 0.5f; y < maxY; y += 1.f) {
            abfunc = (x - a->x) * abdely - (y - a->y) * abdelx;
            bcfunc = (x - b->x) * bcdely - (y - b->y) * bcdelx;
            cafunc = (x - c->x) * cadely - (y - c->y) * cadelx;
            if (abfunc < 0 && bcfunc < 0 && cafunc < 0) createFragment(t, x, y);
            else if (abfunc >= 0 && bcfunc >= 0 && cafunc >= 0) createFragment(t, x, y);
        }
    }
    return;

    /* Fas version that doesn't work
    bool first = true;
    bool first_on_line = true;
    bool left_to_right = true;

    for (float x = (float)minX + 0.5f; x < maxX; x += 1.f) {
        if (left_to_right) {
            first_on_line = true;
            for (float y = (float)minY + 0.5f; y < maxY; y += 1.f) {
                if (first) {
                    first = false;
                    first_on_line = false;
                    abfunc = (x - a->x) * abdely - (y - a->y) * abdelx;
                    bcfunc = (x - b->x) * bcdely - (y - b->y) * bcdelx;
                    cafunc = (x - c->x) * cadely - (y - c->y) * cadelx;
                }
                else if (first_on_line) {
                    first_on_line = false;
                    abfunc += abdely;
                    bcfunc += bcdely;
                    cafunc += cadely;
                }
                else {
                    abfunc += abdelx;
                    bcfunc += bcdelx;
                    cafunc += cadelx;
                }
                if (abfunc >= 0 && bcfunc >= 0 && cafunc >= 0) createFragment(t, x, y);
            }
            left_to_right = false;
        }
        else {
            first_on_line = true;
            for (float y = (float)maxY - 0.5f; y > maxY; y -= 1.f) {
                if (first_on_line) {
                    first_on_line = false;
                    abfunc += abdely;
                    bcfunc += bcdely;
                    cafunc += cadely;
                }
                else {
                    abfunc -= abdelx;
                    bcfunc -= bcdelx;
                    cafunc -= cadelx;
                }
                if (abfunc >= 0 && bcfunc >= 0 && cafunc >= 0) createFragment(t, x, y);
            }
            left_to_right = true;
        }
    }*/
}

void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
    
    //buffers -> 2D graphics (unclipped triangles)
    //as indexing mode doesn't change between function, i is also used as index to
    //index buffer at each call or as ID in non-indexing mode
    triangles.clear();
    outfrags.clear();

    int n = 0;
    for (uint32_t i = 0; i < nofVertices; i++) {
        InVertex inv = fetchInVertex(i);
        Triangle t;
        currProgram->vertex_shader(t.point[n], inv, currProgram->uniforms);
        
        if (n < 2) n++;
        else {
            n = 0;
            triangles.push_back(&t); //pushes finished triangle to list
        }
    }

    //printf("START\n\n");
    //debugTriangles();

    //at this point, there is list with Triangles, each of three OutVertexes
    //clipping here TODO
    //triangle.gl_position.w -> clip space

    for (auto it = triangles.begin(); it != triangles.end(); it++) {
        clipPlane(it);
        if (!(*it)->valid) it = triangles.erase(it);
        if (it == triangles.end()) break;
    }

    //reshaping to normalized
    for (Triangle* t : triangles) {
        t->point[0].gl_Position.x /= t->point[0].gl_Position.w;
        t->point[0].gl_Position.y /= t->point[0].gl_Position.w;
        t->point[0].gl_Position.z /= t->point[0].gl_Position.w;

        t->point[1].gl_Position.x /= t->point[1].gl_Position.w;
        t->point[1].gl_Position.y /= t->point[1].gl_Position.w;
        t->point[1].gl_Position.z /= t->point[1].gl_Position.w;

        t->point[2].gl_Position.x /= t->point[2].gl_Position.w;
        t->point[2].gl_Position.y /= t->point[2].gl_Position.w;
        t->point[2].gl_Position.z /= t->point[2].gl_Position.w;
    }

    uint32_t width = currFrameBuffer->width;
    uint32_t height = currFrameBuffer->height;

    //resizing to screen size
    for (Triangle* t : triangles) {
        t->point[0].gl_Position.x = (t->point[0].gl_Position.x + 1.f) / 2.f * width;
        t->point[0].gl_Position.y = (t->point[0].gl_Position.y + 1.f) / 2.f * height;
        t->point[1].gl_Position.x = (t->point[1].gl_Position.x + 1.f) / 2.f * width;
        t->point[1].gl_Position.y = (t->point[1].gl_Position.y + 1.f) / 2.f * height;
        t->point[2].gl_Position.x = (t->point[2].gl_Position.x + 1.f) / 2.f * width;
        t->point[2].gl_Position.y = (t->point[2].gl_Position.y + 1.f) / 2.f * height;
    }

    for (Triangle* t : triangles) createFragments(t);


    /* //draw test 
    uint8_t* cb = getFramebufferColor();
    for (Triangle* t : triangles) {
        for (int i = 0; i < 3; i++) {
            float x = t->point[i].gl_Position.x;
            float y = t->point[i].gl_Position.y;
            uint64_t ix = (int)round(x);
            uint64_t iy = (int)round(y);

            //TODO czech flag still broken
            if (iy > height || ix > width) {
                continue;
            }
            if (iy == height) iy--;
            if (ix == width) ix--;
            cb[iy * width * 4 + ix * 4] = 255;
            cb[iy * width * 4 + ix * 4 + 1] = 255;
            cb[iy * width * 4 + ix * 4 + 2] = 255;
        }
    }*/
}

/// @}

void GPU::debugTriangles() {
    printf("Width: %d\nHeight: %d\n-----------\n\n", getFramebufferWidth(), getFramebufferHeight());
    for (Triangle* t : triangles) {
        for (int i = 0; i < 3; i++) {
            printf("X: ");
            printf("%f", t->point[i].gl_Position.x);
            printf("\nY: ");
            printf("%f", t->point[i].gl_Position.y);
            printf("\nZ: ");
            printf("%f", t->point[i].gl_Position.z);
            printf("\nW: ");
            printf("%f", t->point[i].gl_Position.w);
            printf("\n\n");
        }
    }
}