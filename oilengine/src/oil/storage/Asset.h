#pragma once

#include <oil/core/UUID.h>


//This file contains wrappers around in engine constructs that specifies loading and saving them as assets
//All types of assets will be included in this file going forward, and should be included from here
namespace oil{

    //forward declarations
    class AssetManager;
    class Entity;

    enum class ContentType{
        None        = 0,
        Directory   = 0x00444952, //DIR
        Scene       = 0x53434e45, //SCNE
        Model       = 0x4d4f444c, //MODL
        Shader      = 0x53484452, //SHDR
        Material    = 0x004d4154, //MAT
        Texture1D   = 0x54455831, //TEX1 
        Texture2D   = 0x54455832, //TEX2 
        Texture3D   = 0x54455833, //TEX3 
        VFX         = 0x00564658, //VFX
        Sound       = 0x00534658, //SFX
        Widget      = 0x57494447, //WIDG
        Template    = 0x54454d50, //TEMP
        Custom      = 0x43555354  //CUST
    };


    template<typename T>
    class Asset{
    public:
        Asset()
            : m_ID(0){};
        Asset(const Ref<T> ref)
            : m_AssetReference(ref) {};
        Asset(UUID id)
            : m_ID(id) {};
        Asset(UUID id, const Ref<T> ref)
            : m_ID(id), m_AssetReference(ref) {};
        ~Asset() = default;

        
        UUID GetID() const { return m_ID; }

        void SetID(UUID id) { m_ID = id; }
        Ref<T> GetContent() {return m_AssetReference; };

        void Referesh() { AssetManager::RefreshAsset(&this); }

        //operator overloads
        T* operator->(){
            return m_AssetReference.get();
        }

        operator Ref<T>() const{
            return m_AssetReference;
        }

        operator UUID() const{
            return m_ID;
        };


        //Defined for each Type of content that extends asset
        inline static ContentType GetType();

    private:
        void SetAsset(Asset<T> asset) { this = asset; }
        void SetContent(Ref<T> content) { m_AssetReference = content; }
        friend class AssetManager;

    private:
        UUID m_ID;
        Ref<T> m_AssetReference = nullptr;
    };

}

