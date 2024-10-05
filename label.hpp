/*
 * label.hpp
 */
#pragma once
#include "expression.hpp"

namespace wasmgen
{
    ////////////////
    // Identifier //
    ////////////////

    class Identifier
        : virtual public Object
        , public StdStringMap<ExpressionPtr>
    {
    private:
        using super = StdStringMap<ExpressionPtr>;

    public:
        using super::super;

        Expression* getexpr(const key_type& key);
    };

    using IdentifierPtr = Pointer<Identifier>;
    using IdentifierRef = RefPointer<Identifier>;
    using NewIdentifier = NewPointer<Identifier>;

    ////////////////////
    // IdentifierList //
    ////////////////////

    class IdentifierList : public PointerArray<Identifier>
    {
    private:
        using super = PointerArray<Identifier>;

    public:
        using super::super;

        bool has(const Identifier::key_type& key) const;
        Expression* getexpr(const Identifier::key_type& key);
    };

    using IdentifierListPtr = Pointer<IdentifierList>;
    using IdentifierListRef = ArrayRef<IdentifierList>;
    using NewIdentifierList = NewArray<IdentifierList>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////////
    // Identifier //
    ////////////////

    inline Expression* Identifier::getexpr(const key_type& key)
    {
        auto it = find(key);

        return it != end() ? &it->second : nullptr;
    }

    ////////////////////
    // IdentifierList //
    ////////////////////

    inline bool IdentifierList::has(const Identifier::key_type& key) const
    {
        for (auto& lv : *this)
            if (lv && lv->has(key))
                return true;
        return false;
    }

    inline Expression* IdentifierList::getexpr(const Identifier::key_type& key)
    {
        Expression* r = nullptr;

        for (auto& lv : *this)
            if (lv && (r = lv->getexpr(key)))
                break;
        return r;
    }

} // wasmgen
