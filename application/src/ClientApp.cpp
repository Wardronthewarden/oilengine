#include <oil.h>

#include "oilengine_export.h"


class ExampleLayer : public oil::Layer{
public:
    ExampleLayer()
    : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) {
            m_VertexArray.reset(oil::VertexArray::Create());

    float vertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
    };

    std::shared_ptr<oil::VertexBuffer> vertexBuffer;
    vertexBuffer.reset(oil::VertexBuffer::Create(vertices, sizeof(vertices)));
    
        oil::BufferLayout layout = {
            {oil::ShaderDataType::Float3, "a_Position"},
            {oil::ShaderDataType::Float4, "a_Color"}
        };
    

    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);

    
    uint32_t indices[3] = {0,1,2};
    std::shared_ptr<oil::IndexBuffer> indexBuffer;
    indexBuffer.reset(oil::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

    m_VertexArray->SetIndexBuffer(indexBuffer);

    m_SquareVA.reset(oil::VertexArray::Create());

    float squareVertices[3 * 4] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f, 
        0.5f, 0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f
    };    

    std::shared_ptr<oil::VertexBuffer> squareVB; 
    squareVB.reset(oil::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
    squareVB->SetLayout({
            {oil::ShaderDataType::Float3, "a_Position"}
});
    m_SquareVA->AddVertexBuffer(squareVB);

    uint32_t squareIndices[6] = {0,1,2, 2,3,0};
    std::shared_ptr<oil::IndexBuffer> squareIB;
    squareIB.reset(oil::IndexBuffer::Create(squareIndices, sizeof(squareIndices)/sizeof(uint32_t)));
    m_SquareVA->SetIndexBuffer(squareIB);

    std::string vertexSrc = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;    
        layout(location = 1) in vec4 a_Color;    

        uniform mat4 u_VPMat;

        out vec3 v_Position;
        out vec4 v_Color;

        void main(){
            v_Position = a_Position;
            v_Color = a_Color;
            gl_Position = u_VPMat * vec4(a_Position, 1.0f);
        }
    )";

    std::string fragmentSrc = R"(
        #version 330 core

        layout(location = 0) out vec4 o_Color;    
        in vec3 v_Position;
        in vec4 v_Color;

        void main(){
            o_Color = v_Color;
        }
    )";

    m_Shader.reset(new oil::Shader(vertexSrc, fragmentSrc));

    std::string vertexSrc2 = R"(
        #version 330 core

        layout(location = 0) in vec3 a_Position;   

        uniform mat4 u_VPMat;

        out vec3 v_Position;

        void main(){
            v_Position = a_Position;
            gl_Position = u_VPMat * vec4(a_Position, 1.0f);
        }
    )";

    std::string fragmentSrc2 = R"(
        #version 330 core

        layout(location = 0) out vec4 o_Color;    
        in vec3 v_Position;

        void main(){
            o_Color = vec4(0.2, 0.3, 0.8, 1.0);
        }
    )";

    m_Shader2.reset(new oil::Shader(vertexSrc2, fragmentSrc2));

        }

    void OnUpdate() override{
        oil::RenderCommand::SetClearColor({0.1f,0.1f,0.1f,1});
        oil::RenderCommand::Clear();

        //Full implementation
        //Renderer::BeginScene(camera, lights, environment);
        m_Camera.SetPosition({0.5f, 0.5f, 0.0f});
        m_Camera.SetRotation(45.0f);

        //Temporary impl.
        oil::Renderer::BeginScene(m_Camera);
        m_Shader2->Bind();
        oil::Renderer::Submit(m_Shader2, m_SquareVA);
        m_Shader->Bind();
        oil::Renderer::Submit(m_Shader, m_VertexArray);

        oil::Renderer::EndScene();

    }

    virtual void OnImGuiRender() override{
    }

    void OnEvent(oil::Event& event) override{
       // OIL_TRACE("{0}", event);
    }

    private:
        
        std::shared_ptr<oil::Shader> m_Shader;
        std::shared_ptr<oil::VertexArray> m_VertexArray;
        

        std::shared_ptr<oil::VertexArray> m_SquareVA;
        std::shared_ptr<oil::Shader> m_Shader2;

        oil::OrthographicCamera m_Camera;
};

class Barrel : public oil::Application{
public:
    Barrel() {
        PushLayer(new ExampleLayer());
    };
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    OIL_INFO("Initialized client app!");
    return new Barrel();
}