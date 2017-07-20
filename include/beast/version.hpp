//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_BEAST_VERSION_HPP
#define BOOST_BEAST_VERSION_HPP

#include <beast/config.hpp>
#include <boost/config.hpp>

/** @def BOOST_BEAST_API_VERSION 

    Identifies the API version of Beast.

    This is a simple integer that is incremented by one every time
    a set of code changes is merged to the master or develop branch.
*/
#define BOOST_BEAST_VERSION 84

#define BOOST_BEAST_VERSION_STRING "Beast/" BOOST_STRINGIZE(BOOST_BEAST_VERSION)

#endif

