#pragma once

#include <memory>
#include "oil/Renderer/Buffer.h"

typedef uint32_t OILVertenum;

#define OIL_VERT_POINTS             0x00000001
#define OIL_VERT_TRIANGLES          0x00000002
#define OIL_VERT_TRIANGLE_STRIP     0x00000003

namespace oil{
    class VertexArray{
    public:
        virtual ~VertexArray(){}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const oil::Ref<VertexBuffer>& VertexBuffer) = 0;
        virtual void SetIndexBuffer(const oil::Ref<IndexBuffer>& IndexBuffer) = 0;
        virtual void SetDrawMode(OILVertenum drawMode) { m_DrawMode = drawMode; }

        virtual const std::vector<oil::Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const oil::Ref<IndexBuffer>& GetIndexBuffer() const = 0;
        OILVertenum GetDrawMode() { return m_DrawMode; }

        static Ref<VertexArray> Create();
    protected:
        OILVertenum m_DrawMode = OIL_VERT_TRIANGLES;
    };

}