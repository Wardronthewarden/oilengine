#pragma once

#include <oil/core/UUID.h>


//This file contains wrappers around in engine constructs that specifies loading and saving them as assets
//All types of assets will be included in this file going forward, and should be included from here
namespace oil{

    using AssetHandle = UUID;

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
    class AssetRef{
    public:
        AssetRef()
            : m_Handle(0){};
        AssetRef(AssetHandle handle) {
            SetHandle(handle);
        };
        AssetRef(AssetHandle handle, Ref<Ref<T>> refLocation) {
            SetHandle(handle);
            SetReferenceLocation(refLocation);
        };
        ~AssetRef() = default;

        
        AssetHandle GetHandle() const { return m_Handle; }
        void SetHandle(AssetHandle handle) { m_Handle = handle; }

        void SetReferenceLocation(Ref<Ref<T>> refLocation){
            m_ReferenceLocation = refLocation;
            m_Reference = *m_ReferenceLocation.get();    
        }

        Ref<T> Get(){
            if(!m_Handle){
                return Ref<T>();
            }
            if(m_Reference.expired()){
                m_Reference = *m_ReferenceLocation.get();
            }
            return m_Reference.lock();
        }

        T* operator ->(){
            return Get().get();
        }

        operator Ref<T>(){
            return Get();
        }

        operator AssetHandle() const{
            return m_Handle;
        };

        operator bool() const{
            return bool(m_Handle);
        }


        //Defined for each Type of content that extends asset
        inline static ContentType GetType();


    private:
        AssetHandle m_Handle;
        std::weak_ptr<T> m_Reference;
        Ref<Ref<T>> m_ReferenceLocation = nullptr;
    };

}

