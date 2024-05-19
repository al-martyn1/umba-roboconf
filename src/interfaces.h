#pragma once

// MCU communication interfaces

#include "connections.h"

struct CommInterfaceLineInfo
{
    bool            manda; // mandatory
    std::string     name; // canonical name

    CommInterfaceLineInfo( bool m = false, std::string n = std::string() ) : manda(m), name(n) {}

}; // struct CommInterfaceLineInfo


struct CommInterfaceTypeInfo
{
    enum InterfaceType
    {
         InterfaceTypeUnknown = 0,
         InterfaceTypeSensor  = 1,
         InterfaceTypeDriver  = 2,
         InterfaceTypeBoth    = 3
    };

    InterfaceType                                    interfaceType;
    std::string                                      typeName; // UART/I2C/SPI/PWM/ADC/DAC/GPIO etc

    std::map< std::string, CommInterfaceLineInfo >   interfaceLines;


    CommInterfaceTypeInfo( InterfaceType it = InterfaceTypeUnknown
                         , std::string n = std::string()
                         )
    : interfaceType(it), typeName(n), interfaceLines() {}


    void addInterfaceLine( std::string name, bool manda = true )
    {
        interfaceLines[name] = CommInterfaceLineInfo( manda );
    }

    void addInterfaceLine( std::string name, std::string canonicalName, bool manda = true )
    {
        interfaceLines[name] = CommInterfaceLineInfo( manda, canonicalName );
    }

}; // struct CommInterfaceTypeInfo


inline
std::map< std::string, CommInterfaceTypeInfo > getCommInterfaces()
{
    static std::map< std::string, CommInterfaceTypeInfo > im;

    if (!im.empty())
        return im;

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeBoth, "I2C");

        typeInfo.addInterfaceLine( "SCL" );
        typeInfo.addInterfaceLine( "SDA" );

        im[typeInfo.typeName] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeBoth, "SPI");

        typeInfo.addInterfaceLine( "MOSI", false );
        typeInfo.addInterfaceLine( "SDI", "MOSI", false );

        typeInfo.addInterfaceLine( "MISO", false );
        typeInfo.addInterfaceLine( "SDO", "MISO", false );

        typeInfo.addInterfaceLine( "SCLK" );
        typeInfo.addInterfaceLine( "CLK", "SCLK" );
        typeInfo.addInterfaceLine( "SCK", "SCLK" );

        im[typeInfo.typeName] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeBoth, "UART");

        typeInfo.addInterfaceLine("RX", false);
        typeInfo.addInterfaceLine("RXD", "RX", false);

        typeInfo.addInterfaceLine("TX", false);
        typeInfo.addInterfaceLine("TXD", "TX", false);

        im[typeInfo.typeName] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeBoth, "TIM");
        typeInfo.addInterfaceLine( "PWM" );
        im[typeInfo.typeName] = typeInfo;
        im["PWM"] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeSensor, "ADC");
        typeInfo.addInterfaceLine( "ADC" );
        im[typeInfo.typeName] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeDriver, "DAC");
        typeInfo.addInterfaceLine( "DAC" );
        im[typeInfo.typeName] = typeInfo;
    }

    {
        CommInterfaceTypeInfo typeInfo(CommInterfaceTypeInfo::InterfaceTypeBoth, "GPIO");
        typeInfo.addInterfaceLine( "GPIO" );
        im[typeInfo.typeName] = typeInfo;
    }


/*
CommInterfaceTypeInfo::InterfaceTypeSensor
CommInterfaceTypeInfo::InterfaceTypeDriver
CommInterfaceTypeInfo::InterfaceTypeBoth  



    {
        CommInterfaceTypeInfo typeInfo("");
        typeInfo.addInterfaceLine( std::string name, bool manda )
        typeInfo.addInterfaceLine( std::string name, std::string canonicalName, bool manda )

        im[typeInfo.typeName] = typeInfo;
    }
*/

    return im;
}




/*
struct ConnectionsGroup
{
    std::string                  groupDesignator; // target designator for all of group items
    std::string                  groupTitle;      // target designator for all of group items
    ComponentInfo                dstComponentInfo;
    std:: vector< Connection >    connections;
*/

