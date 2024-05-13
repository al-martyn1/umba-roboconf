#pragma once

/*
template<typename TargetType>
class UserDefinedLiteralCrutch
{
    TargetType  val;

public:

    constexpr
    UserDefinedLiteralCrutch( TargetType v ) : val(v) {}

    constexpr
    operator TargetType() const
    {
        return val;
    }

};


#define UMBA_DEFINE_USER_LITERAL_TYPE( TargetType, LiteralType ) typedef UserDefinedLiteralCrutch< TargetType >  LiteralType
*/

#define UMBA_DEFINE_USER_LITERAL_TYPE_BEGIN( TargetType, LiteralType )\
class LiteralType                                                     \
{                                                                     \
    TargetType  value;                                                \
                                                                      \
public:                                                               \
                                                                      \
    constexpr                                                         \
    LiteralType( TargetType v ) : value(v) {}                         \
                                                                      \
    constexpr                                                         \
    operator TargetType() const                                       \
    {

#define UMBA_DEFINE_USER_LITERAL_TYPE_END( )\
                                            \
        /*return val;*/                     \
    }                                       \
                                            \
};
