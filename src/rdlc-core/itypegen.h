#pragma once


struct ITypeGen
{
    virtual std::string getDescription() const
    {
        return std::string("don't generate type definition");
    }

    virtual std::string getTargetType( const TypeInfo &typeInfo, const GeneratorOptions &gopts ) const
    {
        return std::string();
    }

    virtual bool generateOutput( std::ostream &os
                               , std::ostream &extraOs
                               , std::string indentBase
                               , const DeviceInfo &device
                               , const TypeMap &typeMap
                               , const TypeInfo &typeInfo
                               , const GeneratorOptions &gopts
                               )
    {
        return true;
    }

};

typedef ITypeGen TypeGenNone;
