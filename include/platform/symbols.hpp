//
// Created by sava on 10/24/19.
//

#ifndef __SYMBOLS_HPP_
#define __SYMBOLS_HPP_

namespace os
{
    const char PathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

}

#endif //__SYMBOLS_HPP_
