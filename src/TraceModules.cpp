#include "TraceModules.h"
#include "Log.h"
#include "ZTrace.pb.h"
#include "ZTracer.h"

LOG_INIT("TraceModules");

#define IMG_MAP(x) case x: return ztrace::ModuleLoad::x
#define SEC_MAP(x) case x: return ztrace::Section::x

static ztrace::ModuleLoad::ImageType
convertImageType(IMG_TYPE t)
{
    switch(t) {
        IMG_MAP( IMG_TYPE_INVALID );
        IMG_MAP( IMG_TYPE_STATIC );
        IMG_MAP( IMG_TYPE_SHARED );
        IMG_MAP( IMG_TYPE_SHAREDLIB );
        IMG_MAP( IMG_TYPE_RELOCATABLE );
    default:
        return ztrace::ModuleLoad::IMG_TYPE_UNKNOWN;
    }
}

static ztrace::Section::SectionType
convertSectionType(SEC_TYPE t)
{
    switch(t) {
        SEC_MAP( SEC_TYPE_INVALID );
        SEC_MAP( SEC_TYPE_UNUSED );
        SEC_MAP( SEC_TYPE_REGREL );
        SEC_MAP( SEC_TYPE_DYNREL );
        SEC_MAP( SEC_TYPE_EXEC );
        SEC_MAP( SEC_TYPE_DATA );
        SEC_MAP( SEC_TYPE_DYNAMIC );
        SEC_MAP( SEC_TYPE_OPD );
        SEC_MAP( SEC_TYPE_GOT );
        SEC_MAP( SEC_TYPE_STACK );
        SEC_MAP( SEC_TYPE_PLTOFF );
        SEC_MAP( SEC_TYPE_HASH );
        SEC_MAP( SEC_TYPE_LSDA );
        SEC_MAP( SEC_TYPE_UNWIND );
        SEC_MAP( SEC_TYPE_UNWINDINFO );
        SEC_MAP( SEC_TYPE_REGSYM );
        SEC_MAP( SEC_TYPE_DYNSYM );
        SEC_MAP( SEC_TYPE_DEBUG );
        SEC_MAP( SEC_TYPE_BSS );
        SEC_MAP( SEC_TYPE_SYMSTR );
        SEC_MAP( SEC_TYPE_DYNSTR );
        SEC_MAP( SEC_TYPE_SECSTR );
        SEC_MAP( SEC_TYPE_COMMENT );
        SEC_MAP( SEC_TYPE_LOOS );
        SEC_MAP( SEC_TYPE_USER );
    default:
        return ztrace::Section::SEC_TYPE_UNKNOWN;
    }
}

#undef IMG_MAP
#undef SEC_MAP

VOID ImgTraceModuleLoad(IMG img, VOID *v)
{
    ZTracer *ztracer = static_cast<ZTracer *>(v);
    
    LOG_DEBUG("Loading %s, Image id = %d", IMG_Name(img).c_str(), IMG_Id(img));

    // Dump module information

    ztrace::Event ev = ztracer->MakeEvent();
    ztrace::ModuleLoad *modLoadEv = ev.mutable_mod_load();
    modLoadEv->set_id( IMG_Id(img) );
    modLoadEv->set_name( IMG_Name(img) );
    modLoadEv->set_low_address( IMG_LowAddress(img)  );
    modLoadEv->set_high_address( IMG_HighAddress(img) );
    modLoadEv->set_load_offset( IMG_LoadOffset(img) );
    modLoadEv->set_image_type( convertImageType( IMG_Type(img) ) );
    modLoadEv->set_is_main_executable( IMG_IsMainExecutable(img) );
    modLoadEv->set_is_static_executable( IMG_IsStaticExecutable(img) );

    std::vector<UINT8> contents;
    size_t size = IMG_SizeMapped(img);
    contents.resize(size);
    size_t ret = PIN_SafeCopy(&contents[0], (const void *) IMG_StartAddress(img), size);
    if(ret != size) {
        LOG_ERROR("Error copying data for module %s - expected %d, got %d bytes",
                  IMG_Name(img).c_str(), size, ret);
    }
    modLoadEv->set_contents( &contents[0], size );

    for(SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec) )
    {
        LOG_DEBUG("  Section: %s (mapped: %d)", SEC_Name(sec).c_str(), SEC_Mapped(sec));

        ztrace::Section *section = modLoadEv->add_section();
        section->set_name( SEC_Name(sec) );
        section->set_type( convertSectionType( SEC_Type(sec) ) );
        section->set_mapped( SEC_Mapped(sec) );
        section->set_is_readable( SEC_IsReadable(sec) );
        section->set_is_writeable( SEC_IsWriteable(sec) );
        section->set_is_executable( SEC_IsExecutable(sec) );
        section->set_address( SEC_Address(sec) );
        section->set_size( SEC_Size(sec) );
    }

    ztracer->WriteEvent(&ev);

    // Dump symbol information from modules. Note that PIN only works with
    // function symbols.

    ev = ztracer->MakeEvent();
    ztrace::ModuleSymbols *symbols = ev.mutable_mod_symbols();
    symbols->set_module_id( IMG_Id(img) );
    symbols->set_module_name( IMG_Name(img) );
    for( SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym) ) {
        ztrace::SymbolEntry *ent = symbols->add_sym();
        ent->set_address( SYM_Address(sym) );
        ent->set_name( SYM_Name(sym) );
        ent->set_undecorated_name( PIN_UndecorateSymbolName(SYM_Name(sym),
                                                            UNDECORATION_COMPLETE) );
        ent->set_is_dynamic( SYM_Dynamic(sym) );
        ent->set_is_ifunc( SYM_IFunc(sym) );
    }

    ztracer->WriteEvent(&ev);
}

VOID ImgTraceModuleUnload(IMG img, VOID *v)
{
    ZTracer *ztracer = static_cast<ZTracer *>(v);
    
    LOG_DEBUG("Unloading %s, Img id = %d", IMG_Name(img).c_str(), IMG_Id(img));

    ztrace::Event ev = ztracer->MakeEvent();
    ztrace::ModuleUnload *modUnloadEv = ev.mutable_mod_unload();
    modUnloadEv->set_id( IMG_Id(img) );    
    ztracer->WriteEvent(&ev);
}
