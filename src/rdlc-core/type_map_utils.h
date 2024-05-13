#pragma once


void initTypeMap( TypeMap &typeMap )
{
    // n , unsigned bs, bool s, bool i, bool fbi )
    typeMap.addType( TypeInfo( "u8" , 1, false, true , true ) );
    typeMap.addType( TypeInfo( "s8" , 1, true , true , true ) );
    typeMap.addType( TypeInfo( "u16", 2, false, true , true ) );
    typeMap.addType( TypeInfo( "s16", 2, true , true , true ) );
    typeMap.addType( TypeInfo( "u32", 4, false, true , true ) );
    typeMap.addType( TypeInfo( "s32", 4, true , true , true ) );
    typeMap.addType( TypeInfo( "f32", 4, true , false, true ) );

    typeMap.addAlias( "s8" , "i8" , true );
    typeMap.addAlias( "s16", "i16", true );
    typeMap.addAlias( "s32", "i32", true );
}