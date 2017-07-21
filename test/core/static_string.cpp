//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/core/static_string.hpp>

#include <boost/beast/unit_test/suite.hpp>

namespace boost {
namespace beast {

class static_string_test : public beast::unit_test::suite
{
public:
    void
    testConstruct()
    {
        {
            static_string<1> s;
            BOOST_BEAST_EXPECT(s.empty());
            BOOST_BEAST_EXPECT(s.size() == 0);
            BOOST_BEAST_EXPECT(s == "");
            BOOST_BEAST_EXPECT(*s.end() == 0);
        }
        {
            static_string<4> s1(3, 'x');
            BOOST_BEAST_EXPECT(! s1.empty());
            BOOST_BEAST_EXPECT(s1.size() == 3);
            BOOST_BEAST_EXPECT(s1 == "xxx");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2(3, 'x');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("12345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            static_string<3> s2(s1, 2);
            BOOST_BEAST_EXPECT(s2 == "345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<0> s3(s1, 5);
            BOOST_BEAST_EXPECT(s3.empty());
            BOOST_BEAST_EXPECT(s3.front() == 0);
            BOOST_BEAST_EXPECT(*s3.end() == 0);
        }
        {
            static_string<5> s1("12345");
            static_string<2> s2(s1, 1, 2);
            BOOST_BEAST_EXPECT(s2 == "23");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<0> s3(s1, 5, 1);
            BOOST_BEAST_EXPECT(s3.empty());
            BOOST_BEAST_EXPECT(s3.front() == 0);
            BOOST_BEAST_EXPECT(*s3.end() == 0);
            try
            {
                static_string<5> s4(s1, 6);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("UVXYZ", 3);
            BOOST_BEAST_EXPECT(s1 == "UVX");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            static_string<5> s2("X\0""Y\0""Z", 3);
            BOOST_BEAST_EXPECT(std::memcmp(
                s2.data(), "X\0""Y", 3) == 0);
            BOOST_BEAST_EXPECT(*s2.end() == 0);
        }
        {
            static_string<5> s1("12345");
            static_string<3> s2(
                s1.begin() + 1, s1.begin() + 3);
            BOOST_BEAST_EXPECT(s2 == "23");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
        }
        {
            static_string<5> s1("12345");
            static_string<5> s2(s1);
            BOOST_BEAST_EXPECT(s2 == "12345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<6> s3(s1);
            BOOST_BEAST_EXPECT(s3 == "12345");
            BOOST_BEAST_EXPECT(*s3.end() == 0);
            try
            {
                static_string<4> s4(s1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1({'1', '2', '3'});
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            BOOST_BEAST_EXPECT(
                static_string<0>({}) == static_string<0>());
            try
            {
                static_string<2> s2({'1', '2', '3'});
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1(
                string_view("123"));
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2(
                    string_view("123"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1(
                std::string("12345"), 2, 2);
            BOOST_BEAST_EXPECT(s1 == "34");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2(
                    std::string("12345"), 1, 3);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
    }

    void
    testAssign()
    {
        {
            static_string<3> s1("123");
            static_string<3> s2;
            s2 = s1;
            BOOST_BEAST_EXPECT(s2 == "123");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
        }
        {
            static_string<3> s1("123");
            static_string<5> s2;
            s2 = s1;
            BOOST_BEAST_EXPECT(s2 == "123");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<1> s3;
                s3 = s1;
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1;
            s1 = "123";
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<1> s2;
                s2 = "123";
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<1> s1;
            s1 = 'x';
            BOOST_BEAST_EXPECT(s1 == "x");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<0> s2;
                s2 = 'x';
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1;
            s1 = {'1', '2', '3'};
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<1> s2;
                s2 = {'1', '2', '3'};
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1;
            s1 = string_view("123");
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<1> s2;
                s2 = string_view("123");
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }

        {
            static_string<4> s1;
            s1.assign(3, 'x');
            BOOST_BEAST_EXPECT(s1 == "xxx");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2;
                s2.assign(3, 'x');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("12345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            static_string<5> s2;
            s2.assign(s1);
            BOOST_BEAST_EXPECT(s2 == "12345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
        }
        {
            static_string<5> s1("12345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            static_string<7> s2;
            s2.assign(s1);
            BOOST_BEAST_EXPECT(s2 == "12345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<3> s3;
                s3.assign(s1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("12345");
            static_string<5> s2;
            s2.assign(s1, 1);
            BOOST_BEAST_EXPECT(s2 == "2345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            s2.assign(s1, 1, 2);
            BOOST_BEAST_EXPECT(s2 == "23");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            s2.assign(s1, 1, 100);
            BOOST_BEAST_EXPECT(s2 == "2345");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                s2.assign(s1, 6);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
            try
            {
                static_string<3> s3;
                s3.assign(s1, 1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1;
            s1.assign("12");
            BOOST_BEAST_EXPECT(s1 == "12");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.assign("12345");
            BOOST_BEAST_EXPECT(s1 == "12345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
        }
        {
            static_string<5> s1;
            s1.assign("12345", 3);
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
        }
        {
            static_string<5> s1("12345");
            static_string<3> s2;
            s2.assign(s1.begin(), s1.begin() + 2);
            BOOST_BEAST_EXPECT(s2 == "12");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                s2.assign(s1.begin(), s1.end());
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1;
            s1.assign({'1', '2', '3'});
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<1> s2;
                s2.assign({'1', '2', '3'});
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1;
            s1.assign(string_view("123"));
            BOOST_BEAST_EXPECT(s1 == "123");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.assign(string_view("12345"));
            BOOST_BEAST_EXPECT(s1 == "12345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                s1.assign(string_view("1234567"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1;
            s1.assign(std::string("12345"), 2, 2);
            BOOST_BEAST_EXPECT(s1 == "34");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.assign(std::string("12345"), 3);
            BOOST_BEAST_EXPECT(s1 == "45");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2;
                s2.assign(
                    std::string("12345"), 1, 3);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
    }

    void
    testAccess()
    {
        {
            static_string<5> s("12345");
            BOOST_BEAST_EXPECT(s.at(1) == '2');
            BOOST_BEAST_EXPECT(s.at(4) == '5');
            try
            {
                BOOST_BEAST_EXPECT(s.at(5) == 0);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<5> const s("12345");
            BOOST_BEAST_EXPECT(s.at(1) == '2');
            BOOST_BEAST_EXPECT(s.at(4) == '5');
            try
            {
                BOOST_BEAST_EXPECT(s.at(5) == 0);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<5> s("12345");
            BOOST_BEAST_EXPECT(s[1] == '2');
            BOOST_BEAST_EXPECT(s[4] == '5');
            s[1] = '_';
            BOOST_BEAST_EXPECT(s == "1_345");
        }
        {
            static_string<5> const s("12345");
            BOOST_BEAST_EXPECT(s[1] == '2');
            BOOST_BEAST_EXPECT(s[4] == '5');
            BOOST_BEAST_EXPECT(s[5] == 0);
        }
        {
            static_string<3> s("123");
            BOOST_BEAST_EXPECT(s.front() == '1');
            BOOST_BEAST_EXPECT(s.back() == '3');
            s.front() = '_';
            BOOST_BEAST_EXPECT(s == "_23");
            s.back() = '_';
            BOOST_BEAST_EXPECT(s == "_2_");
        }
        {
            static_string<3> const s("123");
            BOOST_BEAST_EXPECT(s.front() == '1');
            BOOST_BEAST_EXPECT(s.back() == '3');
        }
        {
            static_string<3> s("123");
            BOOST_BEAST_EXPECT(std::memcmp(
                s.data(), "123", 3) == 0);
        }
        {
            static_string<3> const s("123");
            BOOST_BEAST_EXPECT(std::memcmp(
                s.data(), "123", 3) == 0);
        }
        {
            static_string<3> s("123");
            BOOST_BEAST_EXPECT(std::memcmp(
                s.c_str(), "123\0", 4) == 0);
        }
        {
            static_string<3> s("123");
            string_view sv = s;
            BOOST_BEAST_EXPECT(static_string<5>(sv) == "123");
        }
    }

    void
    testIterators()
    {
        {
            static_string<3> s;
            BOOST_BEAST_EXPECT(std::distance(
                s.begin(), s.end()) == 0);
            BOOST_BEAST_EXPECT(std::distance(
                s.rbegin(), s.rend()) == 0);
            s = "123";
            BOOST_BEAST_EXPECT(std::distance(
                s.begin(), s.end()) == 3);
            BOOST_BEAST_EXPECT(std::distance(
                s.rbegin(), s.rend()) == 3);
        }
        {
            static_string<3> const s("123");
            BOOST_BEAST_EXPECT(std::distance(
                s.begin(), s.end()) == 3);
            BOOST_BEAST_EXPECT(std::distance(
                s.cbegin(), s.cend()) == 3);
            BOOST_BEAST_EXPECT(std::distance(
                s.rbegin(), s.rend()) == 3);
            BOOST_BEAST_EXPECT(std::distance(
                s.crbegin(), s.crend()) == 3);
        }
    }

    void
    testCapacity()
    {
        static_string<3> s;
        BOOST_BEAST_EXPECT(s.empty());
        BOOST_BEAST_EXPECT(s.size() == 0);
        BOOST_BEAST_EXPECT(s.length() == 0);
        BOOST_BEAST_EXPECT(s.max_size() == 3);
        BOOST_BEAST_EXPECT(s.capacity() == 3);
        s = "123";
        BOOST_BEAST_EXPECT(! s.empty());
        BOOST_BEAST_EXPECT(s.size() == 3);
        BOOST_BEAST_EXPECT(s.length() == 3);
        s.reserve(0);
        s.reserve(3);
        try
        {
            s.reserve(4);
            fail("", __FILE__, __LINE__);
        }
        catch(std::length_error const&)
        {
            pass();
        }
        s.shrink_to_fit();
        BOOST_BEAST_EXPECT(! s.empty());
        BOOST_BEAST_EXPECT(s.size() == 3);
        BOOST_BEAST_EXPECT(s.length() == 3);
        BOOST_BEAST_EXPECT(*s.end() == 0);
    }

    void
    testOperations()
    {
        //
        // clear
        //

        {
            static_string<3> s("123");
            s.clear();
            BOOST_BEAST_EXPECT(s.empty());
            BOOST_BEAST_EXPECT(*s.end() == 0);
        }

        //
        // insert
        //

        {
            static_string<7> s1("12345");
            s1.insert(2, 2, '_');
            BOOST_BEAST_EXPECT(s1 == "12__345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<6> s2("12345");
                s2.insert(2, 2, '_');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<6> s2("12345");
                s2.insert(6, 2, '_');
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<7> s1("12345");
            s1.insert(2, "__");
            BOOST_BEAST_EXPECT(s1 == "12__345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<6> s2("12345");
                s2.insert(2, "__");
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<6> s2("12345");
                s2.insert(6, "__");
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<7> s1("12345");
            s1.insert(2, "TUV", 2);
            BOOST_BEAST_EXPECT(s1 == "12TU345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<6> s2("12345");
                s2.insert(2, "TUV", 2);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<6> s2("12345");
                s2.insert(6, "TUV", 2);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<7> s1("12345");
            s1.insert(2, static_string<3>("TU"));
            BOOST_BEAST_EXPECT(s1 == "12TU345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<6> s2("12345");
                s2.insert(2, static_string<3>("TUV"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<6> s2("12345");
                s2.insert(6, static_string<3>("TUV"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<7> s1("12345");
            s1.insert(2, static_string<3>("TUV"), 1);
            BOOST_BEAST_EXPECT(s1 == "12UV345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1 = "12345";
            s1.insert(2, static_string<3>("TUV"), 1, 1);
            BOOST_BEAST_EXPECT(s1 == "12U345");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<6> s2("12345");
                s2.insert(2, static_string<3>("TUV"), 1, 2);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<6> s2("12345");
                s2.insert(6, static_string<3>("TUV"), 1, 2);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<4> s1("123");
            s1.insert(s1.begin() + 1, '_');
            BOOST_BEAST_EXPECT(s1 == "1_23");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<3> s2("123");
                s2.insert(s2.begin() + 1, '_');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<4> s1("12");
            s1.insert(s1.begin() + 1, 2, '_');
            BOOST_BEAST_EXPECT(s1 == "1__2");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<4> s2("123");
                s2.insert(s2.begin() + 1, 2, ' ');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1("123");
            static_string<5> s2("UV");
            s2.insert(s2.begin() + 1, s1.begin(), s1.end());
            BOOST_BEAST_EXPECT(s2 == "U123V");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<4> s3("UV");
                s3.insert(s3.begin() + 1, s1.begin(), s1.end());
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("123");
            s1.insert(1, string_view("UV"));
            BOOST_BEAST_EXPECT(s1 == "1UV23");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<4> s2("123");
                s2.insert(1, string_view("UV"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<5> s2("123");
                s2.insert(5, string_view("UV"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("123");
            s1.insert(1, std::string("UV"));
            BOOST_BEAST_EXPECT(s1 == "1UV23");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                s1.insert(1, std::string("UV"));
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<6> s1("123");
            s1.insert(1, std::string("UVX"), 1);
            BOOST_BEAST_EXPECT(s1 == "1VX23");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.insert(4, std::string("PQR"), 1, 1);
            BOOST_BEAST_EXPECT(s1 == "1VX2Q3");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                s1.insert(4, std::string("PQR"), 1, 1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }

        //
        // erase
        //

        {
            static_string<9> s1("123456789");
            BOOST_BEAST_EXPECT(s1.erase(1, 1) == "13456789");
            BOOST_BEAST_EXPECT(s1 == "13456789");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            BOOST_BEAST_EXPECT(s1.erase(5) == "13456");
            BOOST_BEAST_EXPECT(s1 == "13456");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                s1.erase(7);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
        }
        {
            static_string<9> s1("123456789");
            BOOST_BEAST_EXPECT(*s1.erase(s1.begin() + 5) == '7');
            BOOST_BEAST_EXPECT(s1 == "12345789");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
        }
        {
            static_string<9> s1("123456789");
            BOOST_BEAST_EXPECT(*s1.erase(
                s1.begin() + 5, s1.begin() + 7) == '8');
            BOOST_BEAST_EXPECT(s1 == "1234589");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
        }

        //
        // push_back
        //

        {
            static_string<3> s1("12");
            s1.push_back('3');
            BOOST_BEAST_EXPECT(s1 == "123");
            try
            {
                s1.push_back('4');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            static_string<0> s2;
            try
            {
                s2.push_back('_');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }

        //
        // pop_back
        //

        {
            static_string<3> s1("123");
            s1.pop_back();
            BOOST_BEAST_EXPECT(s1 == "12");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.pop_back();
            BOOST_BEAST_EXPECT(s1 == "1");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.pop_back();
            BOOST_BEAST_EXPECT(s1.empty());
            BOOST_BEAST_EXPECT(*s1.end() == 0);
        }

        //
        // append
        //

        {
            static_string<3> s1("1");
            s1.append(2, '_');
            BOOST_BEAST_EXPECT(s1 == "1__");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<2> s2("1");
                s2.append(2, '_');
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<2> s1("__");
            static_string<3> s2("1");
            s2.append(s1);
            BOOST_BEAST_EXPECT(s2 == "1__");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<2> s3("1");
                s3.append(s1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1("XYZ");
            static_string<4> s2("12");
            s2.append(s1, 1);
            BOOST_BEAST_EXPECT(s2 == "12YZ");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<3> s3("12");
            s3.append(s1, 1, 1);
            BOOST_BEAST_EXPECT(s3 == "12Y");
            BOOST_BEAST_EXPECT(*s3.end() == 0);
            try
            {
                static_string<3> s4("12");
                s4.append(s1, 3);
                fail("", __FILE__, __LINE__);
            }
            catch(std::out_of_range const&)
            {
                pass();
            }
            try
            {
                static_string<3> s4("12");
                s4.append(s1, 1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<4> s1("12");
            s1.append("XYZ", 2);
            BOOST_BEAST_EXPECT(s1 == "12XY");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<3> s3("12");
                s3.append("XYZ", 2);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("12");
            s1.append("XYZ");
            BOOST_BEAST_EXPECT(s1 == "12XYZ");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<4> s2("12");
                s2.append("XYZ");
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1("XYZ");
            static_string<5> s2("12");
            s2.append(s1.begin(), s1.end());
            BOOST_BEAST_EXPECT(s2 == "12XYZ");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<4> s3("12");
                s3.append(s1.begin(), s1.end());
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<5> s1("123");
            s1.append({'X', 'Y'});
            BOOST_BEAST_EXPECT(s1 == "123XY");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<4> s2("123");
                s2.append({'X', 'Y'});
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            string_view s1("XYZ");
            static_string<5> s2("12");
            s2.append(s1);
            BOOST_BEAST_EXPECT(s2 == "12XYZ");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<4> s3("12");
                s3.append(s1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<6> s1("123");
            s1.append(std::string("UVX"), 1);
            BOOST_BEAST_EXPECT(s1 == "123VX");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            s1.append(std::string("PQR"), 1, 1);
            BOOST_BEAST_EXPECT(s1 == "123VXQ");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            try
            {
                static_string<3> s2("123");
                s2.append(std::string("PQR"), 1, 1);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }

        //
        // operator+=
        //

        {
            static_string<2> s1("__");
            static_string<3> s2("1");
            s2 += s1;
            BOOST_BEAST_EXPECT(s2 == "1__");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            try
            {
                static_string<2> s3("1");
                s3 += s1;
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<3> s1("12");
            s1 += '3';
            BOOST_BEAST_EXPECT(s1 == "123");
            try
            {
                s1 += '4';
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<4> s1("12");
            s1 += "34";
            BOOST_BEAST_EXPECT(s1 == "1234");
            try
            {
                s1 += "5";
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            static_string<4> s1("12");
            s1 += {'3', '4'};
            BOOST_BEAST_EXPECT(s1 == "1234");
            try
            {
                s1 += {'5'};
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        {
            string_view s1("34");
            static_string<4> s2("12");
            s2 += s1;
            BOOST_BEAST_EXPECT(s2 == "1234");
            try
            {
                s2 += s1;
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
    }

    void
    testCompare()
    {
        using str1 = static_string<1>;
        using str2 = static_string<2>;
        {
            str1 s1;
            str2 s2;
            s1 = "1";
            s2 = "22";
            BOOST_BEAST_EXPECT(s1.compare(s2) < 0);
            BOOST_BEAST_EXPECT(s2.compare(s1) > 0);
            BOOST_BEAST_EXPECT(s1 < "10");
            BOOST_BEAST_EXPECT(s2 > "1");
            BOOST_BEAST_EXPECT("10" > s1);
            BOOST_BEAST_EXPECT("1" < s2);
            BOOST_BEAST_EXPECT(s1 < "20");
            BOOST_BEAST_EXPECT(s2 > "1");
            BOOST_BEAST_EXPECT(s2 > "2");
        }
        {
            str2 s1("x");
            str2 s2("x");
            BOOST_BEAST_EXPECT(s1 == s2);
            BOOST_BEAST_EXPECT(s1 <= s2);
            BOOST_BEAST_EXPECT(s1 >= s2);
            BOOST_BEAST_EXPECT(! (s1 < s2));
            BOOST_BEAST_EXPECT(! (s1 > s2));
            BOOST_BEAST_EXPECT(! (s1 != s2));
        }
        {
            str1 s1("x");
            str2 s2("x");
            BOOST_BEAST_EXPECT(s1 == s2);
            BOOST_BEAST_EXPECT(s1 <= s2);
            BOOST_BEAST_EXPECT(s1 >= s2);
            BOOST_BEAST_EXPECT(! (s1 < s2));
            BOOST_BEAST_EXPECT(! (s1 > s2));
            BOOST_BEAST_EXPECT(! (s1 != s2));
        }
        {
            str2 s("x");
            BOOST_BEAST_EXPECT(s == "x");
            BOOST_BEAST_EXPECT(s <= "x");
            BOOST_BEAST_EXPECT(s >= "x");
            BOOST_BEAST_EXPECT(! (s < "x"));
            BOOST_BEAST_EXPECT(! (s > "x"));
            BOOST_BEAST_EXPECT(! (s != "x"));
            BOOST_BEAST_EXPECT("x" == s);
            BOOST_BEAST_EXPECT("x" <= s);
            BOOST_BEAST_EXPECT("x" >= s);
            BOOST_BEAST_EXPECT(! ("x" < s));
            BOOST_BEAST_EXPECT(! ("x" > s));
            BOOST_BEAST_EXPECT(! ("x" != s));
        }
        {
            str2 s("x");
            BOOST_BEAST_EXPECT(s <= "y");
            BOOST_BEAST_EXPECT(s < "y");
            BOOST_BEAST_EXPECT(s != "y");
            BOOST_BEAST_EXPECT(! (s == "y"));
            BOOST_BEAST_EXPECT(! (s >= "y"));
            BOOST_BEAST_EXPECT(! (s > "x"));
            BOOST_BEAST_EXPECT("y" >= s);
            BOOST_BEAST_EXPECT("y" > s);
            BOOST_BEAST_EXPECT("y" != s);
            BOOST_BEAST_EXPECT(! ("y" == s));
            BOOST_BEAST_EXPECT(! ("y" <= s));
            BOOST_BEAST_EXPECT(! ("y" < s));
        }
        {
            str1 s1("x");
            str2 s2("y");
            BOOST_BEAST_EXPECT(s1 <= s2);
            BOOST_BEAST_EXPECT(s1 < s2);
            BOOST_BEAST_EXPECT(s1 != s2);
            BOOST_BEAST_EXPECT(! (s1 == s2));
            BOOST_BEAST_EXPECT(! (s1 >= s2));
            BOOST_BEAST_EXPECT(! (s1 > s2));
        }
        {
            str1 s1("x");
            str2 s2("xx");
            BOOST_BEAST_EXPECT(s1 < s2);
            BOOST_BEAST_EXPECT(s2 > s1);
        }
        {
            str1 s1("x");
            str2 s2("yy");
            BOOST_BEAST_EXPECT(s1 < s2);
            BOOST_BEAST_EXPECT(s2 > s1);
        }
    }

    void
    testSwap()
    {
        {
            static_string<3> s1("123");
            static_string<3> s2("XYZ");
            swap(s1, s2);
            BOOST_BEAST_EXPECT(s1 == "XYZ");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            BOOST_BEAST_EXPECT(s2 == "123");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<3> s3("UV");
            swap(s2, s3);
            BOOST_BEAST_EXPECT(s2 == "UV");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            BOOST_BEAST_EXPECT(s3 == "123");
            BOOST_BEAST_EXPECT(*s3.end() == 0);
        }
        {
            static_string<5> s1("123");
            static_string<7> s2("XYZ");
            swap(s1, s2);
            BOOST_BEAST_EXPECT(s1 == "XYZ");
            BOOST_BEAST_EXPECT(*s1.end() == 0);
            BOOST_BEAST_EXPECT(s2 == "123");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            static_string<3> s3("UV");
            swap(s2, s3);
            BOOST_BEAST_EXPECT(s2 == "UV");
            BOOST_BEAST_EXPECT(*s2.end() == 0);
            BOOST_BEAST_EXPECT(s3 == "123");
            BOOST_BEAST_EXPECT(*s3.end() == 0);
            try
            {
                static_string<5> s4("12345");
                static_string<3> s5("XYZ");
                swap(s4, s5);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
            try
            {
                static_string<3> s4("XYZ");
                static_string<5> s5("12345");
                swap(s4, s5);
                fail("", __FILE__, __LINE__);
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
    }

    void
    testGeneral()
    {
        using str1 = static_string<1>;
        using str2 = static_string<2>;
        {
            str1 s1;
            BOOST_BEAST_EXPECT(s1 == "");
            BOOST_BEAST_EXPECT(s1.empty());
            BOOST_BEAST_EXPECT(s1.size() == 0);
            BOOST_BEAST_EXPECT(s1.max_size() == 1);
            BOOST_BEAST_EXPECT(s1.capacity() == 1);
            BOOST_BEAST_EXPECT(s1.begin() == s1.end());
            BOOST_BEAST_EXPECT(s1.cbegin() == s1.cend());
            BOOST_BEAST_EXPECT(s1.rbegin() == s1.rend());
            BOOST_BEAST_EXPECT(s1.crbegin() == s1.crend());
            try
            {
                BOOST_BEAST_EXPECT(s1.at(0) == 0);
                fail();
            }
            catch(std::exception const&)
            {
                pass();
            }
            BOOST_BEAST_EXPECT(s1.data()[0] == 0);
            BOOST_BEAST_EXPECT(*s1.c_str() == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.begin(), s1.end()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.cbegin(), s1.cend()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.rbegin(), s1.rend()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.crbegin(), s1.crend()) == 0);
            BOOST_BEAST_EXPECT(s1.compare(s1) == 0);
        }
        {
            str1 const s1;
            BOOST_BEAST_EXPECT(s1 == "");
            BOOST_BEAST_EXPECT(s1.empty());
            BOOST_BEAST_EXPECT(s1.size() == 0);
            BOOST_BEAST_EXPECT(s1.max_size() == 1);
            BOOST_BEAST_EXPECT(s1.capacity() == 1);
            BOOST_BEAST_EXPECT(s1.begin() == s1.end());
            BOOST_BEAST_EXPECT(s1.cbegin() == s1.cend());
            BOOST_BEAST_EXPECT(s1.rbegin() == s1.rend());
            BOOST_BEAST_EXPECT(s1.crbegin() == s1.crend());
            try
            {
                BOOST_BEAST_EXPECT(s1.at(0) == 0);
                fail();
            }
            catch(std::exception const&)
            {
                pass();
            }
            BOOST_BEAST_EXPECT(s1.data()[0] == 0);
            BOOST_BEAST_EXPECT(*s1.c_str() == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.begin(), s1.end()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.cbegin(), s1.cend()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.rbegin(), s1.rend()) == 0);
            BOOST_BEAST_EXPECT(std::distance(s1.crbegin(), s1.crend()) == 0);
            BOOST_BEAST_EXPECT(s1.compare(s1) == 0);
        }
        {
            str1 s1;
            str1 s2("x");
            BOOST_BEAST_EXPECT(s2 == "x");
            BOOST_BEAST_EXPECT(s2[0] == 'x');
            BOOST_BEAST_EXPECT(s2.at(0) == 'x');
            BOOST_BEAST_EXPECT(s2.front() == 'x');
            BOOST_BEAST_EXPECT(s2.back() == 'x');
            str1 const s3(s2);
            BOOST_BEAST_EXPECT(s3 == "x");
            BOOST_BEAST_EXPECT(s3[0] == 'x');
            BOOST_BEAST_EXPECT(s3.at(0) == 'x');
            BOOST_BEAST_EXPECT(s3.front() == 'x');
            BOOST_BEAST_EXPECT(s3.back() == 'x');
            s2 = "y";
            BOOST_BEAST_EXPECT(s2 == "y");
            BOOST_BEAST_EXPECT(s3 == "x");
            s1 = s2;
            BOOST_BEAST_EXPECT(s1 == "y");
            s1.clear();
            BOOST_BEAST_EXPECT(s1.empty());
            BOOST_BEAST_EXPECT(s1.size() == 0);
        }
        {
            str2 s1("x");
            str1 s2(s1);
            BOOST_BEAST_EXPECT(s2 == "x");
            str1 s3;
            s3 = s2;
            BOOST_BEAST_EXPECT(s3 == "x");
            s1 = "xy";
            BOOST_BEAST_EXPECT(s1.size() == 2);
            BOOST_BEAST_EXPECT(s1[0] == 'x');
            BOOST_BEAST_EXPECT(s1[1] == 'y');
            BOOST_BEAST_EXPECT(s1.at(0) == 'x');
            BOOST_BEAST_EXPECT(s1.at(1) == 'y');
            BOOST_BEAST_EXPECT(s1.front() == 'x');
            BOOST_BEAST_EXPECT(s1.back() == 'y');
            auto const s4 = s1;
            BOOST_BEAST_EXPECT(s4[0] == 'x');
            BOOST_BEAST_EXPECT(s4[1] == 'y');
            BOOST_BEAST_EXPECT(s4.at(0) == 'x');
            BOOST_BEAST_EXPECT(s4.at(1) == 'y');
            BOOST_BEAST_EXPECT(s4.front() == 'x');
            BOOST_BEAST_EXPECT(s4.back() == 'y');
            try
            {
                s3 = s1;
                fail();
            }
            catch(std::exception const&)
            {
                pass();
            }
            try
            {
                str1 s5(s1);
                fail();
            }
            catch(std::exception const&)
            {
                pass();
            }
        }
        {
            str1 s1("x");
            str2 s2;
            s2 = s1;
            try
            {
                s1.resize(2);
                fail();
            }
            catch(std::length_error const&)
            {
                pass();
            }
        }
        pass();
    }

    void
    testToStaticString()
    {
        BOOST_BEAST_EXPECT(to_static_string<long>(0) == "0");
        BOOST_BEAST_EXPECT(to_static_string<long>(1) == "1");
        BOOST_BEAST_EXPECT(to_static_string<long>(0xffff) == "65535");
        BOOST_BEAST_EXPECT(to_static_string<long>(0x10000) == "65536");
        BOOST_BEAST_EXPECT(to_static_string<long long>(0xffffffff) == "4294967295");

        BOOST_BEAST_EXPECT(to_static_string<long>(-1) == "-1");
        BOOST_BEAST_EXPECT(to_static_string<long>(-65535) == "-65535");
        BOOST_BEAST_EXPECT(to_static_string<long>(-65536) == "-65536");
        BOOST_BEAST_EXPECT(to_static_string<long long>(-4294967295ll) == "-4294967295");

        BOOST_BEAST_EXPECT(to_static_string<unsigned long>(0) == "0");
        BOOST_BEAST_EXPECT(to_static_string<unsigned long>(1) == "1");
        BOOST_BEAST_EXPECT(to_static_string<unsigned long>(0xffff) == "65535");
        BOOST_BEAST_EXPECT(to_static_string<unsigned long>(0x10000) == "65536");
        BOOST_BEAST_EXPECT(to_static_string<unsigned long>(0xffffffff) == "4294967295");
    }

    void
    run() override
    {
        testConstruct();
        testAssign();
        testAccess();
        testIterators();
        testCapacity();
        testOperations();
        testCompare();
        testSwap();

        testGeneral();
        testToStaticString();
    }
};

BOOST_BEAST_DEFINE_TESTSUITE(static_string,core,beast);

} // beast
} // boost
