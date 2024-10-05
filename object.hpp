/*
 * object.hpp
 */
#pragma once
#include "utility.hpp"
#include "option.hpp"

namespace wasmgen
{
    ////////////
    // Object //
    ////////////

    class Object
    {
    protected:
#if WASMGEN_USE_THREAD
        mutable std::atomic<int> reference_count;
#else
        mutable int reference_count;
#endif

    public:
        Object() noexcept;
        virtual ~Object() = default;

        int REFCNT() const noexcept;
        bool CHKREF(int n = 0) const noexcept;
        void INCREF() const noexcept;
        bool DECREF() const noexcept;
    };

    /////////////
    // Pointer //
    /////////////

    template <typename T>
    class Pointer
    {
        using self = Pointer<T>;

    protected:
        T* object;

    public:
        Pointer() noexcept;

        Pointer(T* object) noexcept;
        Pointer(T& object) noexcept;
        Pointer(self& object) noexcept;
        Pointer(self&& object) noexcept;

        Pointer(const T* object) noexcept;
        Pointer(const T& object) noexcept;
        Pointer(const self& object) noexcept;

        template <typename... Args>
        Pointer(Args... args);

        ~Pointer();

        operator bool() const noexcept;
        bool operator !() const noexcept;

        operator T*() noexcept;
        operator T&() noexcept;

        operator const T*() const noexcept;
        operator const T&() const noexcept;

        T* operator ->() noexcept;
        T& operator *() noexcept;
        T* operator &() noexcept;

        const T* operator ->() const noexcept;
        const T& operator *() const noexcept;
        const T* operator &() const noexcept;

        self& operator =(T* object) noexcept;
        self& operator =(T& object) noexcept;
        self& operator =(self& pointer) noexcept;
        self& operator =(self&& pointer) noexcept;

        void clear() noexcept;
        T* release() noexcept;

    protected:
        static void incref(T* object) noexcept;
        static void decref(T* object) noexcept;
        static void destory(T* object) noexcept;

        void incref() const noexcept;
        void decref() const noexcept;
    };

    ////////////////
    // NewPointer //
    ////////////////

    template <typename T>
    class NewPointer : public Pointer<T>
    {
    private:
        using super = Pointer<T>;

    public:
        using super::super;

        NewPointer();
    };

    ////////////////
    // RefPointer //
    ////////////////

    template <typename T>
    class RefPointer : public Pointer<T>
    {
    private:
        using super = Pointer<T>;
        using self = RefPointer<T>;

    public:
        using super::super;

        RefPointer(T* object) noexcept;
        RefPointer(T& object) noexcept;

        RefPointer(const T* object) noexcept;
        RefPointer(const T& object) noexcept;

        self& operator =(T* object) noexcept;
        self& operator =(T& object) noexcept;
        self& operator =(super& pointer) noexcept;
        self& operator =(super&& pointer) noexcept;
    };

    ///////////////
    // Singleton //
    ///////////////

    template <typename T>
    class Singleton : public Pointer<T>
    {
    private:
        using super = Pointer<T>;

    public:
        Singleton(T* pointer) noexcept;
    };

    ///////////////////
    // StdPtrList //
    ///////////////////

    template <typename T>
    using StdPtrList = StdVector<Pointer<T>>;

    /////////////////
    // ObjectStack //
    /////////////////

    template <typename T>
    class ObjectStack
        : virtual public Object
        , public StdPtrList<T>
    {
        using self = ObjectStack<T>;
    public:
        using stack = StdPtrList<T>;

    public:
        ObjectStack();
        ~ObjectStack();

        operator bool() const noexcept;

        self& push(T* object);
        T* pop();
    };

    ////////////////
    ////////////////

    template <typename T> T* NewRef(T* object) noexcept;
    template <typename T, typename... Args> T* NewRef(Args... args);

    template <typename T>
    constexpr typename std::remove_reference<T>::type&&
    Transfer(T&& pointer) noexcept;

    template <typename T> T* Finish(T* object) noexcept;
    template <typename T> T* Finish(Pointer<T>& pointer) noexcept;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // Object //
    ////////////

    inline int Object::REFCNT() const noexcept
    {
        return reference_count;
    }

    inline bool Object::CHKREF(int n) const noexcept
    {
        return reference_count > n;
    }

    inline void Object::INCREF() const noexcept
    {
        WASMGEN_XDEBUG(4, "INCREF[", this, "]: ", reference_count, "\n");

        ++reference_count;
    }

    inline bool Object::DECREF() const noexcept
    {
        WASMGEN_XDEBUG(4, "DECREF[", this, "]: ", reference_count, "\n");

        assert(reference_count > 0);
        return (--reference_count == 0);
    }

    /////////////
    // Pointer //
    /////////////

    template <typename T>
    inline Pointer<T>::Pointer() noexcept
        : object(nullptr)
    {
        /*NOOP*/
    }

    template <typename T>
    inline Pointer<T>::Pointer(T* object) noexcept
        : object(object)
    {
        incref();
    }

    template <typename T>
    inline Pointer<T>::Pointer(T& object) noexcept
        : self(&object) {}

    template <typename T>
    inline Pointer<T>::Pointer(self& pointer) noexcept
        : self(pointer.object) {}

    template <typename T>
    inline Pointer<T>::Pointer(self&& pointer) noexcept
        : object(pointer.release())
    {
        /*NOOP*/
    }

    template <typename T>
    inline Pointer<T>::Pointer(const T* object) noexcept
        : object(object)
    {
        incref();
    }

    template <typename T>
    inline Pointer<T>::Pointer(const T& object) noexcept
        : self(&object) {}

    template <typename T>
    inline Pointer<T>::Pointer(const self& pointer) noexcept
        : self(pointer.object) {}

    template <typename T>
    template <typename... Args>
    inline Pointer<T>::Pointer(Args... args)
        : self(new T(args...)) {}

    template <typename T>
    inline Pointer<T>::~Pointer()
    {
        decref();
    }

    /**/

    template <typename T>
    inline Pointer<T>::operator bool() const noexcept
    {
        return !!object;
    }

    template <typename T>
    inline bool Pointer<T>::operator !() const noexcept
    {
        return !object;
    }

    /**/

    template <typename T>
    inline Pointer<T>::operator T*() noexcept
    {
        return object;
    }

    template <typename T>
    inline Pointer<T>::operator const T*() const noexcept
    {
        return object;
    }

    template <typename T>
    inline Pointer<T>::operator T&() noexcept
    {
        assert(object);
        return *object;
    }

    template <typename T>
    inline Pointer<T>::operator const T&() const noexcept
    {
        assert(object);
        return *object;
    }

    /**/

    template <typename T>
    inline T* Pointer<T>::operator ->() noexcept
    {
        return object;
    }

    template <typename T>
    inline const T* Pointer<T>::operator ->() const noexcept
    {
        return object;
    }

    template <typename T>
    inline T& Pointer<T>::operator *() noexcept
    {
        return *object;
    }

    template <typename T>
    inline const T& Pointer<T>::operator *() const noexcept
    {
        return *object;
    }

    template <typename T>
    inline T* Pointer<T>::operator &() noexcept
    {
        return object;
    }

    template <typename T>
    inline const T* Pointer<T>::operator &() const noexcept
    {
        return object;
    }

    /**/

    template <typename T>
    inline Pointer<T>& Pointer<T>::operator =(T* pointer) noexcept
    {
        incref(pointer);
        decref(object);
        object = pointer;
        return *this;
    }

    template <typename T>
    inline Pointer<T>& Pointer<T>::operator =(T& pointer) noexcept
    {
        return *this = &pointer;
    }

    template <typename T>
    inline Pointer<T>& Pointer<T>::operator =(self& pointer) noexcept
    {
        return *this = &pointer;
    }

    template <typename T>
    inline Pointer<T>& Pointer<T>::operator =(self&& pointer) noexcept
    {
        decref();
        object = pointer.release();
        return *this;
    }

    /**/

    template <typename T>
    inline void Pointer<T>::clear() noexcept
    {
        *this = nullptr;
    }

    template <typename T>
    inline T* Pointer<T>::release() noexcept
    {
        T* pointer = object;

        object = nullptr;
        return pointer;
    }

    template <typename T>
    inline void Pointer<T>::incref() const noexcept
    {
        incref(object);
    }

    template <typename T>
    inline void Pointer<T>::decref() const noexcept
    {
        decref(object);
    }

    template <typename T>
    inline void Pointer<T>::incref(T* object) noexcept
    {
        if (object)
            object->INCREF();
    }

    template <typename T>
    inline void Pointer<T>::decref(T* object) noexcept
    {
        if (object && object->DECREF())
            destory(object);
    }

    template <typename T>
    inline void Pointer<T>::destory(T* object) noexcept
    {
        delete object;
    }

    ////////////////
    // RefPointer //
    ////////////////

    template <typename T>
    inline RefPointer<T>::RefPointer(T* object) noexcept
    {
        super::object = object;
    }

    template <typename T>
    inline RefPointer<T>::RefPointer(T& object) noexcept
    {
        super::object = &object;
    }

    template <typename T>
    inline RefPointer<T>::RefPointer(const T* object) noexcept
    {
        super::object = object;
    }

    template <typename T>
    inline RefPointer<T>::RefPointer(const T& object) noexcept
    {
        super::object = object;
    }

    /**/

    template <typename T>
    inline RefPointer<T>& RefPointer<T>::operator =(T* object) noexcept
    {
        super::decref();
        super::object = object;
        return *this;
    }

    template <typename T>
    inline RefPointer<T>& RefPointer<T>::operator =(T& object) noexcept
    {
        return *this = &object;
    }

    template <typename T>
    inline RefPointer<T>& RefPointer<T>::operator =(super& pointer) noexcept
    {
        return *this = &pointer;
    }

    template <typename T>
    inline RefPointer<T>& RefPointer<T>::operator =(super&& pointer) noexcept
    {
        return *this = pointer.release();
    }

    ////////////////
    // NewPointer //
    ////////////////

    template <typename T>
    inline NewPointer<T>::NewPointer()
        : super(new T) {}

    ///////////////
    // Singleton //
    ///////////////

    template <typename T>
    inline Singleton<T>::Singleton(T* pointer) noexcept
        : super(pointer)
    {
        super::incref();
    }

    /////////////////
    // ObjectStack //
    /////////////////

    template <typename T>
    ObjectStack<T>::ObjectStack()
    {
        /*NOOP*/
    }

    template <typename T>
    ObjectStack<T>::~ObjectStack()
    {
        /*NOOP*/
    }

    /**/

    template <typename T>
    inline ObjectStack<T>::operator bool() const noexcept
    {
        return !stack::empty();
    }

    /**/

    template <typename T>
    inline ObjectStack<T>& ObjectStack<T>::push(T* object)
    {
        stack::push_back(object);
        return *this;
    }

    template <typename T>
    inline T* ObjectStack<T>::pop()
    {
        if (stack::empty())
            return NULL;

        T* object = Finish(stack::back());

        stack::pop_back();
        return object;
    }

    ////////////
    // NewRef //
    ////////////

    template <typename T>
    inline T* NewRef(T* object) noexcept
    {
        object->INCREF();
        return object;
    }

    template <typename T, typename... Args>
    inline T* NewRef(Args... args)
    {
        T* object = new T(args...);

        object->INCREF();
        return object;
    }

    //////////////
    // Transfer //
    //////////////

    template <typename T>
    inline constexpr typename std::remove_reference<T>::type&&
    Transfer(T&& pointer) noexcept
    {
        return std::move(pointer);
    }

    ////////////
    // Finish //
    ////////////

    template <typename T>
    inline T* Finish(T* object) noexcept
    {
        assert(!object || object->CHKREF());
        return object;
    }

    template <typename T>
    inline T* Finish(Pointer<T>& pointer) noexcept
    {
        T *object = pointer.release();

        assert(!object || object->CHKREF());
        return object;
    }

} // wasmgen
