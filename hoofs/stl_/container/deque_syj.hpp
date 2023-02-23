//
// Created by syj on 2023/2/10.
//

#ifndef STL_STUDY_DEQUE_SYJ_HPP
#define STL_STUDY_DEQUE_SYJ_HPP

#include <iostream>

namespace stl_{
    template<typename _Tp, size_t Chunk_Size=512>
    struct _Deque_iterator{
        using _Elt_pointer=_Tp*; //数据指针
        using _Map_pointer=_Tp**; //中控器
        using size_type=size_t;
        
        typedef _Deque_iterator _Self;
        _Elt_pointer _M_first;//当前连续内存的起始位置
        _Elt_pointer _M_last; //当前连续内存的末尾位置
        _Elt_pointer _M_cur;  //当前数据的节点位置
        _Map_pointer _M_node; //存了一个中控器地址, 地址信息为 _M_first, 即存储了 chunk 的地址信息

        bool operator==(_Deque_iterator& _itr){
            return ((_M_first==_itr._M_first) && (_M_cur == _itr._M_cur)
                && (_M_last == _itr._M_last) && (_M_node == _itr._M_node));
        }
        bool operator!=(_Deque_iterator & _itr){return !(*this==_itr);}

        _Self& operator+=(size_type _n)
        {
            const size_type _offset = _n + (_M_cur - _M_first);
            if(_offset >= 0 && _offset < size_type(Chunk_Size / sizeof(_Tp)))
            {
                _M_cur += _n;
            }
            else
            {
                const size_type _node_offset =
                        _offset >0 ? _offset/size_type(Chunk_Size / sizeof(_Tp))
                        : -size_type((-_offset-1)/(Chunk_Size / sizeof(_Tp)))-1;
                _M_set_node(_M_node + _node_offset);
                _M_cur = _M_first + (_offset - _node_offset*size_type(Chunk_Size/sizeof(_Tp)));
            }
            return *this;
        }

        _Self operator+(size_type _n){
            _Self __tmp = *this;
            return __tmp += _n;
        }

        _Self& operator-=(size_type _n){return *this += -_n;}
        _Self operator-(size_type _n){
            _Self __tmp = *this;
            return __tmp -= _n;
        }

        void _M_set_node(_Map_pointer __new_node) _GLIBCXX_NOEXCEPT
        {
            _M_node = __new_node;
            _M_first = *__new_node;
            _M_last = _M_first + size_type(Chunk_Size / sizeof(_Tp));
        }

//        template<typename _Tp, size_t Chunk_Size>
//        inline size_type operator-(const _Deque_iterator<_Tp,Chunk_Size>& _x,const _Deque_iterator<_Tp,Chunk_Size>& _y) noexcept
//        {}

        void operator=(_Deque_iterator& _map){
            this->_M_first = _map._M_first;
            this->_M_node = _map._M_node;
            this->_M_cur = _map._M_cur;
            this->_M_last = _map._M_last;
        }
    };

    template<typename _Tp>
    class deque{ //用sizeof看出来，里面有10个指针
        using iterator=_Deque_iterator<_Tp>;

        typedef typename iterator::_Map_pointer _Map_pointer;
        typedef typename iterator::size_type size_type;
    public:
        deque()
        :_M_map_size(0),
        _M_map(),_M_start(),_M_finish()
        {}
        ~deque(){
            this->clear();
        }

        void clear();
        iterator begin(){return _M_start;}
        iterator end(){return _M_finish;}
        size_t size(){return size_type(_M_start-_M_finish);}

    protected:
        void GetMap()
        {}
    private:
        _Map_pointer _M_map;//中控器指针, 中控器指向一块内存, 这块内存由指针组成, 指向每一个内存块
        size_t _M_map_size; //中控器个数, 连续内存
        iterator _M_start;  //容器起始位置 begin()
        iterator _M_finish; //容器使用的最后位置 end()
    };

    template<typename _Tp>
    void deque<_Tp>::clear()
    {
//        if(_M_finish != _M_start)
//        {
//            delete [] _M_start;
//            delete [] _M_map;
//            _M_map_size = 0;
//            _M_start = nullptr;
//            _M_finish = nullptr;
//            _M_map = nullptr;
//        }
    }
}

#endif //STL_STUDY_DEQUE_SYJ_HPP
