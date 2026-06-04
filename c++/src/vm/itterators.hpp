#ifndef ITTERATORS_HPP
#define ITTERATORS_HPP

#include <uttility>

class MapEnumerator
{
    private:
    std::map<f_type, s_type> map;
    
    int i = -1;

    public:
    template<typename f_type, typename s_type> 
    MapEnumerator(std::map<f_type, s_type> map);
    template<typename f_type>
    std::pair<int, f_type> advance();
}

#endif