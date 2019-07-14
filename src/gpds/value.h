#pragma once

#include <type_traits>
#include <variant>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include "attributes.h"
#include "utils.h"
#include "types.h"

namespace Gpds
{
    class Container;

    template<class ...Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class ...Ts> overload(Ts...) -> overload<Ts...>;

    template<typename T, typename ...Ts>
    static constexpr bool contains() {
        return std::disjunction_v< std::is_same< T, Ts > ... >;
    }

    template<typename T>
    static constexpr bool isValidType() {
        return contains< T, gBool, gInt, gReal, gString, gContainer >;
    }

    class Value
    {
    public:
        Attributes attributes;
        gString comment;

        Value( ) = default;

        Value( const Value& other ) :
            attributes( other.attributes ),
            comment( other.comment ),
            _value( other._value )
        {
            if ( std::holds_alternative<Container*>( _value ) ) {
                allocateContainerMemory( *std::get<Container*>( _value ) );
            }
        }

        Value( Value&& other ) :
            attributes( std::move( other.attributes ) ),
            comment( std::move( other.comment ) ),
            _value( std::move( other._value) )
        {
            other._value = nullptr;
        }

        template<class T,
            typename std::enable_if< not std::is_class<T>::value, T >::type* = nullptr>
        Value( const T& value)
        {
            set<T>(value);
        }

        template <class T,
                typename std::enable_if< std::is_class<T>::value, T >::type* = nullptr>
        Value( T&& value )
        {
            set<T>( std::move( value ) );
        }

        virtual ~Value() noexcept
        {
            // Ensure that we won't throw
            GPDS_ASSERT( not _value.valueless_by_exception() );

            freeContainerMemory();
        }

        template<typename T>
        constexpr bool isType() const noexcept
        {
            return std::holds_alternative<T>( _value );
        }

        constexpr bool isEmpty() const
        {
            return _value.valueless_by_exception();
        }

        constexpr const char* typeString() const
        {
            if ( std::holds_alternative<Container*>( _value ) ) {
                return "nested";
            }

            return std::visit(overload{
                    [](const gBool&)    { return "bool"; },
                    [](const gInt&)     { return "int"; },
                    [](const gReal&)    { return "double"; },
                    [](const gString&)  { return "string"; }
            }, _value);

            return "n/a";
        }

        template<typename T>
        void set(const T& value)
        {
            _value = value;
        }

        template<typename T>
        void set(T&& value)
        {
            _value = std::move( value );
        }

        template<typename T = Container&>
        void set(const Container& container)
        {
            freeContainerMemory();
            allocateContainerMemory(container);
        }

        template<typename T = Container&&>
        void set(Container&& container)
        {
            freeContainerMemory();
            allocateContainerMemory( std::move( container ) );
        }

        template<typename T>
        constexpr T get() const
        {
            return std::get<T>( _value );
        }

        Value& addAttribute(gString&& key, gString&& value)
        {
            attributes.add( std::forward< gString >( key ), std::forward< gString >( value ) );

            return *this;
        }

        Value& addAttribute(gString&& key, const gString& value)
        {
            return addAttribute( std::forward< gString >( key ), gString( value ) );
        }

        std::optional< gString > getAttribute(gString&& key) const
        {
            return attributes.get( std::forward< gString >( key ) );
        }

        Value& setComment(const gString& comment)
        {
            this->comment = comment;

            return *this;
        }

        Value& setComment(gString&& comment)
        {
            this->comment = std::move( comment );

            return *this;
        }

    private:
        std::variant<
            gBool,
            gInt,
            gReal,
            gString,
            gContainer
        > _value;

        // Implementation is located in value.cpp to prevent
        // circular dependency.
        // This keeps this library from being a header-only library. Maybe
        // someone could tell me how to fix this in a way that we can achieve
        // a header-only library again.
        void allocateContainerMemory(const Container& container);
        void allocateContainerMemory(Container&& container);
        void freeContainerMemory();
    };
}