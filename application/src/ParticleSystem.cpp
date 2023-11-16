#include "ParticleSystem.h"
#include "Random.h"
#include "glm/gtc/constants.hpp"

std::mt19937 Random::s_RandomEngine;
std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;

ParticleSystem::ParticleSystem(uint32_t maxParticles)
    :m_PoolIndex(maxParticles -1)
{
    m_ParticlePool.resize(maxParticles);
}

void ParticleSystem::OnUpdate(oil::Timestep dt)
{
    for (auto& particle : m_ParticlePool){
        if(!particle.Active)
            continue;

        if (particle.LifeRemaining <= 0.0f){
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= dt;
        particle.Position += particle.Velocity * (float)dt;
        particle.Rotation += 0.01f*dt;
    }
}

void ParticleSystem::OnRender(oil::OrthographicCamera &camera)
{
    oil::Renderer2D::BeginScene(camera);
    for (auto& particle : m_ParticlePool){
        if(!particle.Active)
            continue;

        // Particle fading
        float life = particle.LifeRemaining / particle.LifeTime;

        glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);

        float size = glm::mix(particle.SizeEnd, particle.SizeBegin, life);

        oil::Renderer2D::DrawQuad({particle.Position.x, particle.Position.y, 0.1f}, {size, size}, color, particle.Rotation);
    
    }
    oil::Renderer2D::EndScene();


}

void ParticleSystem::Emit(const ParticleProps &particleProps)
{
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;
    particle.Position = particleProps.Position;
    particle.Rotation = Random::Float() * 2 * glm::pi<float>();

    // Velocity
    //particle.Velocity = particleProps.Position;
    particle.Velocity.x = particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
    particle.Velocity.y = particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

    //Color
    particle.ColorBegin = particleProps.ColorBegin;
    particle.ColorEnd = particleProps.ColorEnd;

    particle.LifeTime = particleProps.LifeTime;
    particle.LifeRemaining = particleProps.LifeTime;
    particle.SizeBegin = particleProps.SizeBegin;
    particle.SizeEnd = particleProps.SizeEnd;

    m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();

}
