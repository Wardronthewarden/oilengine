#pragma once


// FOR USE BY APPLICATIONS

#include "oilengine_export.h"

#include "oil/core/Application.h"
#include "oil/core/Layer.h"
#include "oil/core/Log.h"
#include "oil/core/Timer.h"

#include "oil/Core/Timestep.h"

#include "imgui.h"

// INPUT
#include "oil/core/Input.h"
#include "oil/core/KeyCodes.h"
#include "oil/core/MouseButtonCodes.h"

#include "oil/imGui/imGuiLayer.h"

// Scene and assets
#include "oil/core/AssetManager.h"
#include "oil/Scene/Scene.h"
#include "oil/Scene/Entity.h"
#include "oil/Scene/ScriptableEntity.h"
#include "oil/Scene/Component.h"


// ------Rendering-------------

#include "oil/Renderer/Renderer.h"
#include "oil/Renderer/Renderer2D.h"
#include "oil/Renderer/Renderer3D.h"
#include "oil/Renderer/RenderCommand.h"

#include "oil/Renderer/Buffer.h"
#include "oil/Renderer/FrameBuffer.h"
#include "oil/Renderer/VertexArray.h"
#include "oil/Renderer/Texture.h"
#include "oil/Renderer/SubTexture2D.h"
#include "oil/Renderer/Shader.h"

#include "oil/Renderer/OrthographicCamera.h"
#include "oil/Renderer/OrthographicCameraController.h"

