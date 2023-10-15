#pragma once

#include <memory>
#include "oil/Renderer/Buffer.h"

namespace oil{
    class VertexArray{
    public:
        virtual ~VertexArray(){}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(const oil::Ref<VertexBuffer>& VertexBuffer) = 0;
        virtual void SetIndexBuffer(const oil::Ref<IndexBuffer>& IndexBuffer) = 0;

        virtual const std::vector<oil::Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const oil::Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static VertexArray* Create();
    };

}