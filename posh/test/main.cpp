#include <iostream>
#include <deque>
#include "vector_syj.hpp"
#include "deque_syj.hpp"
#include <algorithm>
#include "mman.hpp"

int main() {
//    std::deque<char> val_std;
//    std::cout << sizeof val_std << std::endl;
//    std::cout << sizeof(void *) << std::endl;
//    stl_::deque<char> deque_val;
//    std::cout << sizeof deque_val <<std::endl;

//    reload_shm_open("",1,0);

    stl_::vector<int> val;
    val.push_back(10);
    val.push_back(20);
    val.push_back(30);
    val.push_back(40);
    val.push_back(50);
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    val.pop_back();
    std::cout << "size:" << val.size() << ",0:" << val.at(0) << ",1:" << val.at(1) << ",2:" << val.at(2) << ",3:" << val.at(3) << ",4:" << val.at(4) << ",5:" << val.at(5) << std::endl;
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
