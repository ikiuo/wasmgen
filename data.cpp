/*
 * data.cpp
 */
#include "data.hpp"

namespace wasmgen
{
    /*
     *
     */

    const ValTypeDict numtype_dict = {
        { "i32", ValType::I32 /*0x7f*/ },
        { "i64", ValType::I64 /*0x7e*/ },
        { "f32", ValType::F32 /*0x7d*/ },
        { "f64", ValType::F64 /*0x7c*/ },
    };

    const ValTypeDict vectype_dict = {
        { "v128", ValType::V128 /*0x7b*/ },
        { "i8x16", ValType::V128 /*0x7b*/ },
        { "i16x8", ValType::V128 /*0x7b*/ },
        { "i32x4", ValType::V128 /*0x7b*/ },
        { "i64x2", ValType::V128 /*0x7b*/ },
        { "f32x4", ValType::V128 /*0x7b*/ },
        { "f64x2", ValType::V128 /*0x7b*/ },
    };

    const ValTypeDict reftype_dict = {
        { "funcref",   ValType::FUNCREF   /*0x70*/ },
        { "externref", ValType::EXTERNREF /*0x6f*/ },
    };

    const ValTypeDict valtype_dict =
        numtype_dict | vectype_dict | reftype_dict;

    const ValTypeDict functype_dict = {
        { "functype", ValType::FUNCTYPE /*0x60*/ },
    };

    /**/

    const ValTypeName valtype_name = {
        { ValType::I32, "i32" },
        { ValType::I64, "i64" },
        { ValType::F32, "f32" },
        { ValType::F64, "f64" },
        { ValType::V128, "v128" },
        { ValType::FUNCREF, "funcref" },
        { ValType::EXTERNREF, "externref" },
        { ValType::FUNCTYPE, "functype" },
    };

    /*
     *
     */

    ///////////////
    // ExprValue //
    ///////////////

    StdString ExprValue::str() const
    {
        switch (type)
        {
        case ST_NONE:
            return "(invalid)";
        case ST_INT:
            return StringBlock{ivalue};
        case ST_FLOAT:
            return StringBlock{fvalue};
        case ST_NULL:
            return "null";
        case ST_VALTYPE:
            {
                auto it = valtype_name.find(ValType(ivalue));
                if (it !=  valtype_name.end())
                    return it->second;
            }
            fallthrough;
        default:
            return StringBlock{"(valtype=", ivalue, ")"};
        case ST_NAME:
        case ST_STRING:
            return string ? *string : "(null)";
        }
    }

    StdString ExprValue::dump() const
    {
        switch (type)
        {
        case ST_NONE:
            return "(empty)";
        case ST_INT:
            return StringBlock{"int64=", ivalue};
        case ST_FLOAT:
            return StringBlock{"double=", fvalue};
        case ST_NULL:
            return "null";
        case ST_NAME:
            return StringBlock{"name=", *string};
        case ST_VALTYPE:
            {
                auto it = valtype_name.find(ValType(ivalue));
                if (it !=  valtype_name.end())
                    return StringBlock{"valtype=", it->second};
            }
            return StringBlock{"valtype=", ivalue};
        case ST_STRING:
            return StringBlock{"string=", *string};
        default:
            return StringBlock{"(valtype=", int(type), ")"};
        }
    }

} // wasmgen
