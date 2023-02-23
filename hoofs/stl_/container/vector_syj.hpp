//
// Created by syj on 2023/2/10.
//

#ifndef STL_STUDY_COMM_VECTOR_HPP
#define STL_STUDY_COMM_VECTOR_HPP
#include <iostream>

namespace stl_
{
    template<typename _Tp>
    class vector{
        typedef size_t					size_type;
        using value_type=_Tp;
        using reference=_Tp&;
        using const_reference=_Tp const &;
        using pointer=_Tp*;
        using const_pointer=_Tp const*; //不能用这个去修改值
        using iterator=_Tp*;
        using const_iterator=_Tp const*;
        using reverse_iterator=_Tp*;
        using const_reverse_iterator=_Tp const*;
    public:
        vector()=default;
        vector(size_type _n, const value_type& _value);
        vector(const vector & _x);
        vector(vector && _x);
        ~vector()noexcept;
        vector& operator=(const vector& _x);
        vector& operator=(vector&& _x);
        iterator begin()noexcept{return iterator(_M_start);}
        const_iterator begin()const noexcept{return const_iterator(_M_start);}
        iterator end()noexcept{return iterator(_M_finish);}
        const_iterator end()const noexcept{return const_iterator(_M_finish);}
        reverse_iterator rbegin()noexcept{return reverse_iterator(end());}
        const_reverse_iterator rbegin()const noexcept{return const_reverse_iterator(end());}
        reverse_iterator rend()noexcept{ return reverse_iterator(begin()); }
        const_reverse_iterator rend() const noexcept{ return const_reverse_iterator(begin()); }
        const_iterator cbegin()const noexcept{return const_iterator(_M_start);}
        const_iterator cend()const noexcept{return const_iterator(_M_finish);}
        const_reverse_iterator crbegin()const noexcept{ return const_reverse_iterator(end()); }
        const_reverse_iterator crend()const noexcept{ return const_reverse_iterator(begin()); }
        size_type size()const noexcept{return size_type(_M_finish-_M_start);}
        size_type max_size()const noexcept{return static_cast<size_type>(static_cast<unsigned long long>(1<<32)-1);}
        void resize(size_type _new_size){
            reserve(_new_size);
        }
        void resize(size_type _new_size, const value_type & _x)
        {
            if(_new_size>this->capacity()  && _new_size < this->max_size())
            {
                pointer _start = _M_start;
                pointer _finish = _M_finish;
//                pointer _end = _M_end_of_storage;
                _M_start = new value_type[_new_size];
                _M_end_of_storage = _M_start+(_new_size)*sizeof(size_type);
                init_vector(0);
                set_value(size_type(_finish-_start),_x);
                delete []_start; //释放之前的内存
            }
        }
        size_type capacity()const noexcept{return size_type(_M_end_of_storage-_M_start);}
        bool empty()const noexcept;
        void reserve(size_type _n);
        void shrink_to_fit(); //TODO

        reference operator[](size_type _n)noexcept{
            return *(_M_start + _n);
        }
        const_reference operator[](size_type _n)const noexcept{
            return *(_M_start + _n);
        }
        reference at(size_type _n)noexcept{
            /*if (_n >= this->size())
                __throw_out_of_range_fmt(__N("vector::_M_range_check: __n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         _n, this->size());*/
            return (*this)[_n];
        }
        const_reference at(size_type _n)const noexcept{
            /*if (_n >= this->size())
                __throw_out_of_range_fmt(__N("vector::_M_range_check: __n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         _n, this->size());*/
            return (*this)[_n];
        }
        reference front()noexcept{
            return *begin();
        }
        const_reference front()const noexcept{return *begin();}
        reference back()noexcept{return *(end()-1);}
        const_reference back()const noexcept{return *(end() - 1);}
        value_type* data()noexcept{return (value_type*)(_M_start);}
        const value_type * data() const noexcept{return (value_type*)(_M_start);}
//        void assign(size_type _n, const value_type &val);
        void push_back(const value_type &_x);
        void push_back(value_type && _x);
        void pop_back()noexcept;
        iterator insert(const_iterator _position, const value_type & _x)
        {
            static_assert(_position > this->begin() && _position < this->end());
            auto __position = const_cast<iterator>(_position);
            if(_M_finish == _M_end_of_storage)
            {
                reserve(capacity()<<1);
            }
            for (auto itr = end(); itr != __position; itr--) {
                *itr = *(itr-1);
            }
            *__position = _x;
            return __position;
        }
        iterator insert(const_iterator _position, value_type && _x)
        {
            value_type __tem = std::move(_x);
            return insert(_position,__tem);
        }
        iterator insert(const_iterator _position, size_type _n, const value_type & _x){
            for (int i = 0; i < _n; ++i) {
                insert(_position+i, _x);
            }
        }
        iterator erase(const_iterator _position);
        iterator erase(const_iterator _first, const_iterator _last);
        void clear()noexcept
        {
            delete []_M_start;
            _M_start = nullptr;
            _M_finish = nullptr;
            _M_end_of_storage = nullptr;
        }
        void swap(vector&);

        template<typename... _Args>
        iterator emplace(const_iterator _position, _Args&&... _args);
        template<typename... _Args>
        reference emplace_back(_Args&&... _args);
    private:
        void init_vector(const size_type & _val);
        void init_vector(size_type && _val);
        void set_value(size_type _n, const value_type & _value);
    private:
        pointer _M_start{nullptr}; //申请的内存起始位置
        pointer _M_finish{nullptr}; //用到了哪儿
        pointer _M_end_of_storage{nullptr}; //申请的内存末尾
    };

    template<typename _Tp>
    vector<_Tp>::vector(size_type _n, const value_type& _value)
            : _M_start(new value_type[_n])
            ,_M_end_of_storage(_M_start+_n*sizeof(size_type))
    {
        set_value(_n,_value);
    }
    template<typename _Tp>
    vector<_Tp>::vector(const vector &_x)
            : _M_start(new value_type[_x.capacity()])
            ,_M_end_of_storage(_M_start+_x.capacity())
    {
        for (int i = 0; i < _x.size(); i++) {
            operator[](i)=_x[i];
        }
    }
    template<typename _Tp>
    vector<_Tp>::vector(vector &&_x)
            : _M_start(new value_type[std::move(_x).capacity()])
            ,_M_end_of_storage(_M_start+std::move(_x).capacity())
    {
        for (int i = 0; i < std::move(_x).size(); i++) {
            operator[](i)=std::move(_x).at(i);
        }
    }
    template<typename _Tp>
    vector<_Tp>::~vector() noexcept {
        this->clear();
//        delete []_M_start;
    }

    template<typename _Tp>
    vector<_Tp> & vector<_Tp>::operator=(const vector &_x){}
    template<typename _Tp>
    vector<_Tp> & vector<_Tp>::operator=(vector &&_x){}


    template<typename _Tp>
    bool vector<_Tp>::empty() const noexcept {
        return ((_M_finish == _M_start)? true: false);
    }
    template<typename _Tp>
    void vector<_Tp>::reserve(size_type _n)
    {
        if(this->max_size() > _n && _n > this->capacity())
        {
            pointer __start = new value_type[_n];
            for (int i = 0; i < _n; i++) {
                *(__start+i) = 0;
            }
            pointer __finish = (_M_finish-_M_start)+__start;
            for (int i = 0; i < size_type(__finish-__start); i++) {
                *(__start+i) = this->at(i);
            }
            this->clear(); //释放资源
            _M_start = __start;
            _M_finish = __finish;
            _M_end_of_storage = __start + _n*(sizeof(size_type));
        }
    }

    /*template<typename _Tp>
    void vector<_Tp>::assign(size_type _n, const value_type &val)
    {}*/

    template<typename _Tp>
    void vector<_Tp>::push_back(const value_type &_x)
    {
        if(size() == capacity())
        {
            if(nullptr != _M_end_of_storage)
            {
                resize(2*capacity());
            }
            else
            {
                resize(4);
            }
            *_M_finish = _x;
            _M_finish++;
        }
        else
        {
            *_M_finish = _x;
            _M_finish++;
        }
    }
    template<typename _Tp>
    void vector<_Tp>::push_back(value_type &&_x)
    {
        value_type __tem = std::move(_x);
        push_back(__tem);
    }
    template<typename _Tp>
    void vector<_Tp>::pop_back() noexcept
    {
        if(_M_finish > _M_start)
        {
            for (int i = 0; i < size(); ++i) {
                *(_M_start+i) = *(_M_start+i+1);
            }
            --_M_finish;
        }
    }

    template<typename _Tp>
    void vector<_Tp>::init_vector(const size_type & _val) {
        for (int i = 0; i < size_type(_M_end_of_storage-_M_start); i++) {
            this->at(i) = _val;
        }
    }
    template<typename _Tp>
    void vector<_Tp>::init_vector(size_type && _val) {
        size_type tem_val = std::move(_val);
        for (int i = 0; i < size_type(_M_end_of_storage-_M_start); i++) {
            this->at(i) = tem_val;
        }
    }
    template<typename _Tp>
    void vector<_Tp>::set_value(size_type _n, const value_type &_value)
    {
        for (int i = 0; i < _n; i++) {
            this->operator[](i) = _value;
        }
    }
}

#endif //STL_STUDY_COMM_VECTOR_HPP
