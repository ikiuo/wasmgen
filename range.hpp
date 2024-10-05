/*
 * range.hpp
 */
#pragma once
#include "compiler.hpp"

namespace wasmgen
{
    ////////////////
    // step_range //
    ////////////////

    template <typename T, int N>
    class step_range
    {
    public:
        using value_type = T;
        using difference_type = typename std::make_signed<T>::type;
        using size_type = value_type;

        template <bool R>
        class base_iterator : public std::random_access_iterator_tag
        {
            using self = base_iterator<R>;

        protected:
            value_type _n;

        public:
            inline base_iterator(value_type n) noexcept : _n(n) {};

            inline value_type operator *() const noexcept
                { return _n - (R ? N : 0); }

            inline bool operator < (const self& r) const noexcept { return _n <  r._n; }
            inline bool operator <=(const self& r) const noexcept { return _n <= r._n; }
            inline bool operator ==(const self& r) const noexcept { return _n == r._n; }
            inline bool operator !=(const self& r) const noexcept { return _n != r._n; }
            inline bool operator >=(const self& r) const noexcept { return _n >= r._n; }
            inline bool operator > (const self& r) const noexcept { return _n >  r._n; }

            inline self operator ++() noexcept
                { return self(_n += N); }
            inline self operator ++(int) noexcept
                { self r(_n); _n += N; return r; };
            inline self operator +(value_type n) const noexcept
                { return self(R ? (_n - n * N) : (_n + n * N)); }
            inline self operator -(value_type n) const noexcept
                { return self(R ? (_n + n * N) : (_n - n * N)); }
            inline difference_type operator -(const self& r) const noexcept
                { return difference_type(R ? (r._n - _n) : (_n - r._n)); }
        };

        // using pointer = base_iterator<false>;
        // using const_pointer = pointer;

        using reference = value_type;
        using const_reference = value_type;

        using iterator = base_iterator<false>;
        using const_iterator = iterator;

        using reverse_iterator = base_iterator<true>;
        using const_reverse_iterator = reverse_iterator;

    protected:
        value_type _b;
        value_type _e;

    public:
        inline step_range(value_type n) noexcept
            : _b(0), _e(n) {}
        inline step_range(value_type b, value_type e) noexcept
            : _b(b), _e(e)
            {
                if (N < 2)
                    return;

                value_type r = (e - b) % N;

                if (r != 0)
                    _e = N - r;
            }

        inline value_type operator [](size_type n) const noexcept
            { return _b + N * n; }

        inline size_type size() const noexcept
            { return (_e - _b) / N; }
        inline size_type max_size() const noexcept
            { return size(); }

        inline reference at(size_type n) noexcept
            { return _b + N * n; }
        inline const_reference at(size_type n) const noexcept
            { return _b + N * n; }
        inline reference back() noexcept
            { return _e - 1; }
        inline const_reference back() const noexcept
            { return _e - 1; }
        inline reference front() noexcept
            { return _b; }
        inline const_reference front() const noexcept
            { return _b; }

        // pointer data() noexcept;
        // const_pointer data() const noexcept;

        inline iterator begin() noexcept
            { return iterator(_b); }
        inline iterator end() noexcept
            { return iterator(_e); }
        inline const_iterator begin() const noexcept
            { return cbegin(); }
        inline const_iterator end() const noexcept
            { return cend(); }
        inline const_iterator cbegin() const noexcept
            { return const_iterator(_b); }
        inline const_iterator cend() const noexcept
            { return const_iterator(_e); }

        inline reverse_iterator rbegin() noexcept
            { return reverse_iterator(_e); }
        inline reverse_iterator rend() noexcept
            { return reverse_iterator(_b); }
        inline const_reverse_iterator rbegin() const noexcept
            { return crbegin(); }
        inline const_reverse_iterator rend() const noexcept
            { return crend(); }
        inline const_reverse_iterator crbegin() const noexcept
            { return const_reverse_iterator(_e); }
        inline const_reverse_iterator crend() const noexcept
            { return const_reverse_iterator(_b); }
    };

    ///////////////
    // inc_range //
    ///////////////

    template <typename T>
    class inc_range : public step_range<T, 1>
    {
    private:
        using super = step_range<T, 1>;

    public:
        inline inc_range(T n) noexcept : super(n) {}
        inline inc_range(T b, T e) noexcept : super(b, e) {}
    };

    ////////////
    // irange //
    ////////////

    template <typename T>
    class irange
    {
    public:
        using value_type = T;
        using difference_type = typename std::make_signed<T>::type;
        using size_type = value_type;

        template <bool R>
        class base_iterator : public std::random_access_iterator_tag
        {
            using self = base_iterator<R>;

        protected:
            value_type _n;
            value_type _s;

        public:
            inline base_iterator(value_type n, value_type s) noexcept
                : _n(n), _s(s) {}

            inline value_type operator *() const noexcept
                { return _n - (R ? _s : 0); }

            inline bool operator < (const self& r) const noexcept { return _n <  r._n; }
            inline bool operator <=(const self& r) const noexcept { return _n <= r._n; }
            inline bool operator ==(const self& r) const noexcept { return _n == r._n; }
            inline bool operator !=(const self& r) const noexcept { return _n != r._n; }
            inline bool operator >=(const self& r) const noexcept { return _n >= r._n; }
            inline bool operator > (const self& r) const noexcept { return _n >  r._n; }

            inline self operator ++() noexcept
                { return self(step(), _s); }
            inline self operator ++(int) noexcept
                { self r(_n, _s); step(); return r; };
            inline self operator +(value_type n) const noexcept
                { return self(R ? (_n - n * _s) : (_n + n * _s), _s); }
            inline self operator -(value_type n) const noexcept
                { return self(R ? (_n + n * _s) : (_n - n * _s), _s); }
            inline difference_type operator -(const self& r) const noexcept
                { return difference_type(R ? (r._n - _n) : (_n - r._n)); }

        protected:
            inline value_type step() noexcept
                { return R ? (_n -= _s) : (_n += _s); }
        };

        // using pointer = base_iterator<false>;
        // using const_pointer = pointer;

        using reference = value_type;
        using const_reference = value_type;

        using iterator = base_iterator<false>;
        using const_iterator = iterator;

        using reverse_iterator = base_iterator<true>;
        using const_reverse_iterator = reverse_iterator;

    protected:
        value_type _b;
        value_type _e;
        value_type _s;
        size_type _l;

    public:
        inline irange(value_type n) noexcept
            : _b(0), _e(n), _s(1), _l(n) {}
        inline irange(value_type b, value_type e) noexcept
            : _b(b), _e(e), _s(b < e ? 1 : T(-1))
            , _l(b < e ? e - b : b - e) {}
        inline irange(value_type b, value_type e, value_type s) noexcept
            : _b(b), _e(e), _s(s)
            {
                value_type r = (e - b) % s;

                if (r != 0)
                    _e = e += s - r;
                _l = (e - b) / s;
            }

        inline value_type operator [](size_type n) const noexcept
            { return _b + _s * n; }

        inline size_type size() const noexcept
            { return _l; }
        inline size_type max_size() const noexcept
            { return size(); }

        inline reference at(size_type n) noexcept
            { return _b + _s * n; }
        inline const_reference at(size_type n) const noexcept
            { return _b + _s * n; }
        inline reference back() noexcept
            { return _e - _s; }
        inline const_reference back() const noexcept
            { return _e - _s; }
        inline reference front() noexcept
            { return _b; }
        inline const_reference front() const noexcept
            { return _b; }

        // pointer data() noexcept;
        // const_pointer data() const noexcept;

        inline iterator begin() noexcept
            { return iterator(_b, _s); }
        inline iterator end() noexcept
            { return iterator(_e, _s); }
        inline const_iterator begin() const noexcept
            { return cbegin(); }
        inline const_iterator end() const noexcept
            { return cend(); }
        inline const_iterator cbegin() const noexcept
            { return const_iterator(_b, _s); }
        inline const_iterator cend() const noexcept
            { return const_iterator(_e, _s); }

        inline reverse_iterator rbegin() noexcept
            { return reverse_iterator(_e, _s); }
        inline reverse_iterator rend() noexcept
            { return reverse_iterator(_b, _s); }
        inline const_reverse_iterator rbegin() const noexcept
            { return crbegin(); }
        inline const_reverse_iterator rend() const noexcept
            { return crend(); }
        inline const_reverse_iterator crbegin() const noexcept
            { return const_reverse_iterator(_e, _s); }
        inline const_reverse_iterator crend() const noexcept
            { return const_reverse_iterator(_b, _s); }
    };

} // wasmgen
