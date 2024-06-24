#pragma once

namespace oil{

    enum class ShaderDataType : uint8_t{
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type){
        switch (type){
            case ShaderDataType::Float:         return 4;
            case ShaderDataType::Float2:        return 4 * 2;
            case ShaderDataType::Float3:        return 4 * 3;
            case ShaderDataType::Float4:        return 4 * 4;
            case ShaderDataType::Mat3:          return 4 * 3 * 3;
            case ShaderDataType::Mat4:          return 4 * 4 * 4;
            case ShaderDataType::Int:           return 4;
            case ShaderDataType::Int2:          return 4 * 2;
            case ShaderDataType::Int3:          return 4 * 3;
            case ShaderDataType::Int4:          return 4 * 4;
            case ShaderDataType::Bool:          return 1;
        }

        OIL_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement{
        std::string Name;
        ShaderDataType Type;
        uint32_t Offset;
        uint32_t Size;
        bool Normalized;

        BufferElement() {}

        BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
            : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized){       
        }
        uint32_t GetComponentCount() const{
            switch (Type){
                case ShaderDataType::Float:         return 1;
                case ShaderDataType::Float2:        return 2;
                case ShaderDataType::Float3:        return 3;
                case ShaderDataType::Float4:        return 4;
                case ShaderDataType::Mat3:          return 3 * 3;
                case ShaderDataType::Mat4:          return 4 * 4;
                case ShaderDataType::Int:           return 1;
                case ShaderDataType::Int2:          return 2;
                case ShaderDataType::Int3:          return 3;
                case ShaderDataType::Int4:          return 4;
                case ShaderDataType::Bool:          return 1;
            }

            OIL_CORE_ASSERT(false, "Unknown ShaderDataType!");
            return 0;
        }
    };

    class BufferLayout{
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : m_Elements(elements) {
                CalculateOffsetsAndStride();
            }

        inline uint32_t GetStride() const { return m_Stride; }
        void SetStride(const uint32_t stride) { m_Stride = stride; }
        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }

        void AddElement(const BufferElement& elem) { m_Elements.push_back(elem); }

        //TODO: add support to dynamically remove elements
        //This is to help with loading and saving from and to files
        //Offset and stride will be loaded from "Accessors" and "Buffer Views"

        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
    private:
        void CalculateOffsetsAndStride(){
            uint32_t offset = 0;
            m_Stride = 0;
            for (auto& element : m_Elements){
                element.Offset = offset;
                offset += element.Size;
                m_Stride += element.Size;
            }
        }
    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
    };

    class VertexBuffer{
    public:
        virtual ~VertexBuffer(){}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& GetLayout() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        static Ref<VertexBuffer> Create(uint32_t size);
        static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
    };

    class UniformBuffer{
    public:
        virtual ~UniformBuffer(){}

        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual void SetLayout(const BufferLayout& layout) = 0;
        virtual const BufferLayout& GetLayout() const = 0;

        virtual void SetData(const void* data, uint32_t size) = 0;

        virtual void SetBinding(uint32_t binding) = 0;
        virtual const uint32_t GetBinding() = 0;

        static Ref<UniformBuffer> Create(uint32_t size);
        static Ref<UniformBuffer> Create(float* vertices, uint32_t size);
    };

    // currently only 32-bit index buffers
    class IndexBuffer{
    public:
        virtual ~IndexBuffer(){}
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;

        virtual uint32_t GetCount() const = 0;

        virtual void SetData(const void* data, uint32_t count) = 0;

        static Ref<IndexBuffer> Create(uint32_t count);
        static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
    };


    template<typename T>
    class DataBuffer{
    public:
        DataBuffer() = default;
        DataBuffer(const T* data, const size_t size)
            :  m_Size(size){
                m_Data = new T[size];
                memcpy(m_Data, data, size);
        }

        DataBuffer(const size_t size)
            :  m_Size(size){
                m_Data = new T[size];
        }
        DataBuffer(const DataBuffer& buff){
                SetData(buff.GetData(), buff.GetSize());
        }

        ~DataBuffer(){
            if(m_Data)
                delete[] m_Data;
        };

        T operator[](size_t pos) const {
            return pos >= m_Size ? m_Data[pos] : T(); 
        };

        operator void*(){
            return (void*)m_Data;
        }

        operator T*(){
            return m_Data;
        }

        const T* GetData() const { return m_Data; }
        const size_t GetSize() const { return m_Size; }

        void Resize(size_t size){
            if ((size > m_Size) && m_Data ){
                T* ptr = new T[size];
                memcpy(ptr, m_Data, m_Size);
                delete[] m_Data;
                m_Data = ptr;
            }

            m_Size = size;
            
        }

        T* Begin() { return m_Data; }
        T* End() { return m_Data + (m_Size / sizeof(T)); }

        void SetData(DataBuffer buffer){
            SetData(buffer.GetData(), buffer.GetSize());
        }
        void SetData(const T* data, const size_t size){
            delete[] m_Data;
            m_Data = new T[size];
            memcpy(m_Data, data, size);
            m_Size = size;
        }

        operator bool() const{
            if (m_Data)
                return true;
            return false;
        }

    private:
        size_t m_Size = 0;
        T* m_Data = nullptr;
    };
}