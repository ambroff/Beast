//
// Copyright (c) 2013-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BEAST_HTTP_BASIC_PARSER_HPP
#define BEAST_HTTP_BASIC_PARSER_HPP

#include <beast/config.hpp>
#include <beast/core/error.hpp>
#include <beast/core/string_view.hpp>
#include <beast/http/detail/basic_parser.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/optional.hpp>
#include <boost/assert.hpp>
#include <memory>
#include <utility>

namespace beast {
namespace http {

/** A parser for decoding HTTP/1 wire format messages.

    This parser is designed to efficiently parse messages in the
    HTTP/1 wire format. It allocates no memory when input is
    presented as a single contiguous buffer, and uses minimal
    state. It will handle chunked encoding and it understands
    the semantics of the Connection, Content-Length, and Upgrade
    fields.

    The interface uses CRTP (Curiously Recurring Template Pattern).
    To use this class, derive from @ref basic_parser. When bytes
    are presented, the implementation will make a series of zero
    or more calls to derived class members functions (referred to
    as "callbacks" from here on) matching a specific signature.

    Every callback must be provided by the derived class, or else
    a compilation error will be generated. This exemplar shows
    the signature and description of the callbacks required in
    the derived class.

    @par Derived Example

    @code
    template<bool isRequest>
    struct derived
        : basic_parser<isRequest, derived<isRequest>>
    {
        // When isRequest == true, called
        // after the Request Line is received.
        //
        void
        on_request(
            string_view method,
            string_view target,
            int version,
            error_code& ec);

        // When isRequest == false, called
        // after the Status Line is received.
        //
        void
        on_response(
            int status,
            string_view reason,
            int version,
            error_code& ec);

        // Called after receiving a field/value pair.
        //
        void
        on_field(
            string_view name,
            string_view value,
            error_code& ec);

        // Called after the header is complete.
        //
        void
        on_header(
            error_code& ec);

        // Called once before the body, if any, is started.
        //
        void
        on_body(
            boost::optional<std::uint64_t> content_length,
            error_code& ec);

        // Called zero or more times to provide body data.
        //
        void
        on_data(
            string_view s,
            error_code& ec);

        // If the Transfer-Encoding is specified, and the
        // last item in the list of encodings is "chunked",
        // called after receiving a chunk header or a final
        // chunk.
        //
        void
        on_chunk(
            std::uint64_t length,       // Length of this chunk
            string_view const& ext,     // The chunk extensions, if any
            error_code& ec);

        // Called once when the message is complete.
        // This will be called even if there is no body.
        //
        void
        on_complete(error_code& ec);
    };
    @endcode

    If a callback sets the error code, the error will be propagated
    to the caller of the parser.

    The parser is optimized for the case where the input buffer
    sequence consists of a single contiguous buffer. The
    @ref beast::flat_buffer class is provided, which guarantees
    that the input sequence of the stream buffer will be represented
    by exactly one contiguous buffer. To ensure the optimum performance
    of the parser, use @ref beast::flat_buffer with HTTP algorithms
    such as @ref beast::http::read, @ref beast::http::read_some,
    @ref beast::http::async_read, and @ref beast::http::async_read_some.
    Alternatively, the caller may use custom techniques to ensure that
    the structured portion of the HTTP message (header or chunk header)
    is contained in a linear buffer.

    @tparam isRequest A `bool` indicating whether the parser will be
    presented with request or response message.

    @tparam Derived The derived class type. This is part of the
    Curiously Recurring Template Pattern interface.
*/
template<bool isRequest, class Derived>
class basic_parser
    : private detail::basic_parser_base
{
    template<bool OtherIsRequest, class OtherDerived>
    friend class basic_parser;

    // Message will be complete after reading header
    static unsigned constexpr flagSkipBody              = 1<<  0;

    // Consume input buffers across semantic boundaries
    static unsigned constexpr flagEager                 = 1<<  1;

    // The parser has read at least one byte
    static unsigned constexpr flagGotSome               = 1<<  2;

    // Message semantics indicate a body is expected.
    // cleared if flagSkipBody set
    //
    static unsigned constexpr flagHasBody               = 1<<  3;

    static unsigned constexpr flagHTTP11                = 1<<  4;
    static unsigned constexpr flagNeedEOF               = 1<<  5;
    static unsigned constexpr flagExpectCRLF            = 1<<  6;
    static unsigned constexpr flagFinalChunk            = 1<<  7;
    static unsigned constexpr flagConnectionClose       = 1<<  8;
    static unsigned constexpr flagConnectionUpgrade     = 1<<  9;
    static unsigned constexpr flagConnectionKeepAlive   = 1<< 10;
    static unsigned constexpr flagContentLength         = 1<< 11;
    static unsigned constexpr flagChunked               = 1<< 12;
    static unsigned constexpr flagUpgrade               = 1<< 13;

    std::uint64_t len_;     // size of chunk or body
    std::unique_ptr<char[]> buf_;
    std::size_t buf_len_ = 0;
    std::size_t skip_ = 0;  // search from here
    std::size_t x_;         // scratch variable
    state state_ = state::nothing_yet;
    unsigned f_ = 0;        // flags

public:
    /// Copy constructor (disallowed)
    basic_parser(basic_parser const&) = delete;

    /// Copy assignment (disallowed)
    basic_parser& operator=(basic_parser const&) = delete;

    /// Default constructor
    basic_parser() = default;

    /// `true` if this parser parses requests, `false` for responses.
    using is_request =
        std::integral_constant<bool, isRequest>;

    /// Destructor
    ~basic_parser() = default;

    /** Move constructor

        After the move, the only valid operation on the
        moved-from object is destruction.
    */
    template<class OtherDerived>
    basic_parser(basic_parser<isRequest, OtherDerived>&&);

    /** Returns a reference to this object as a @ref basic_parser.

        This is used to pass a derived class where a base class is
        expected, to choose a correct function overload when the
        resolution would be ambiguous.
    */
    basic_parser&
    base()
    {
        return *this;
    }

    /** Returns a constant reference to this object as a @ref basic_parser.

        This is used to pass a derived class where a base class is
        expected, to choose a correct function overload when the
        resolution would be ambiguous.
    */
    basic_parser const&
    base() const
    {
        return *this;
    }

    /// Returns `true` if the parser has received at least one byte of input.
    bool
    got_some() const
    {
        return state_ != state::nothing_yet;
    }

    /** Returns `true` if the message is complete.

        The message is complete after the full header is prduced
        and one of the following is true:

        @li The skip body option was set.

        @li The semantics of the message indicate there is no body.

        @li The semantics of the message indicate a body is expected,
        and the entire body was parsed.
    */
    bool
    is_done() const
    {
        return state_ == state::complete;
    }

    /** Returns `true` if a the parser has produced the full header.
    */
    bool
    is_header_done() const
    {
        return state_ > state::header;
    }

    /** Returns `true` if the message is an upgrade message.

        @note The return value is undefined unless
        @ref is_header_done would return `true`.
    */
    bool
    is_upgrade() const
    {
        return (f_ & flagConnectionUpgrade) != 0;
    }

    /** Returns `true` if the last value for Transfer-Encoding is "chunked".

        @note The return value is undefined unless
        @ref is_header_done would return `true`.
    */
    bool
    is_chunked() const
    {
        return (f_ & flagChunked) != 0;
    }

    /** Returns `true` if the message has keep-alive connection semantics.

        @note The return value is undefined unless
        @ref is_header_done would return `true`.
    */
    bool
    is_keep_alive() const;

    /** Returns the optional value of Content-Length if known.

        @note The return value is undefined unless
        @ref is_header_done would return `true`.
    */
    boost::optional<std::uint64_t>
    content_length() const;

    /** Returns `true` if the message semantics require an end of file.

        Depending on the contents of the header, the parser may
        require and end of file notification to know where the end
        of the body lies. If this function returns `true` it will be
        necessary to call @ref put_eof when there will never be additional
        data from the input.
    */
    bool
    need_eof() const
    {
        return (f_ & flagNeedEOF) != 0;
    }

    /// Returns `true` if the eager parse option is set.
    bool
    eager() const
    {
        return (f_ & flagEager) != 0;
    }

    /** Set the eager parse option.

        This option controls whether or not the parser will attempt
        to consume all of the octets provided during parsing, or
        if it will stop when it reaches one of the following positions
        within the serialized message:

        @li Immediately after the header

        @li Immediately after any chunk header

        The default is to stop after the header or any chunk header.
    */
    void
    eager(bool v)
    {
        if(v)
            f_ |= flagEager;
        else
            f_ &= ~flagEager;
    }

    /// Returns `true` if the parser will ignore the message body.
    bool
    skip()
    {
        return (f_ & flagSkipBody) != 0;
    }

    /** Set the skip body option.

        The option controls whether or not the parser expects to
        see an HTTP body, regardless of the presence or absence of
        certain fields such as Content-Length.

        Depending on the request, some responses do not carry a
        body. For example, a 200 response to a CONNECT request
        from a tunneling proxy. In these cases, callers may use
        this function inform the parser that no body is expected.
        The parser will consider the message complete after the
        header has been received.

        @note This function must called before any bytes are processed.
    */
    void
    skip(bool v);

    /** Write a buffer sequence to the parser.

        This function attempts to incrementally parse the HTTP
        message data stored in the caller provided buffers. Upon
        success, a positive return value indicates that the parser
        made forward progress, consuming that number of
        bytes.

        In some cases there may be an insufficient number of octets
        in the input buffer in order to make forward progress. This
        is indicated by the the code @ref error::need_more. When
        this happens, the caller should place additional bytes into
        the buffer sequence and call @ref put again.

        The error code @ref error::need_more is special. When this
        error is returned, a subsequent call to @ref put may succeed
        if the buffers have been updated. Otherwise, upon error
        the parser may not be restarted.

        @param buffers An object meeting the requirements of
        @b ConstBufferSequence that represents the message.

        @param ec Set to the error, if any occurred.

        @return The number of octets consumed in the buffer
        sequence. The caller should remove these octets even if the
        error is set.
    */
    template<class ConstBufferSequence>
    std::size_t
    put(ConstBufferSequence const& buffers, error_code& ec);

#if ! BEAST_DOXYGEN
    std::size_t
    put(boost::asio::const_buffers_1 const& buffer,
        error_code& ec);
#endif

    /** Inform the parser that the end of stream was reached.

        In certain cases, HTTP needs to know where the end of
        the stream is. For example, sometimes servers send
        responses without Content-Length and expect the client
        to consume input (for the body) until EOF. Callbacks
        and errors will still be processed as usual.

        This is typically called when a read from the
        underlying stream object sets the error code to
        `boost::asio::error::eof`.

        @note Only valid after parsing a complete header.

        @param ec Set to the error, if any occurred. 
    */
    void
    put_eof(error_code& ec);

private:
    inline
    Derived&
    impl()
    {
        return *static_cast<Derived*>(this);
    }

    template<class ConstBufferSequence>
    string_view
    maybe_flatten(
        ConstBufferSequence const& buffers);

    void
    parse_header(char const*& p,
        std::size_t n, error_code& ec);

    void
    parse_header(char const*& p, char const* term,
        error_code& ec, std::true_type);

    void
    parse_header(char const*& p, char const* term,
        error_code& ec, std::false_type);

    void
    parse_body(char const*& p,
        std::size_t n, error_code& ec);

    void
    parse_body_to_eof(char const*& p,
        std::size_t n, error_code& ec);

    void
    parse_chunk_header(char const*& p,
        std::size_t n, error_code& ec);

    void
    parse_chunk_body(char const*& p,
        std::size_t n, error_code& ec);

    void
    parse_fields(char const*& p,
        char const* last, error_code& ec);

    void
    do_field(string_view const& name,
        string_view const& value,
            error_code& ec);
};

} // http
} // beast

#include <beast/http/impl/basic_parser.ipp>

#endif
