//
// Created by syj on 2023/2/21.
//

#ifndef STUDY_PROJECT_BASE_TREE_HPP
#define STUDY_PROJECT_BASE_TREE_HPP

#include <iostream>
//#include <map>
namespace stl_{
    template<typename _Key, typename _Value>
    class tree{
        using value_type=std::pair<_Key, _Value>;
        using reference=value_type&;
        using pointer=value_type*;
        using BiTree=tree*;
    public:
        tree():left(nullptr),right(nullptr)
        {}

    private:
        value_type m_data;
        BiTree left;
        BiTree right;
    };
}

#endif //STUDY_PROJECT_BASE_TREE_HPP
