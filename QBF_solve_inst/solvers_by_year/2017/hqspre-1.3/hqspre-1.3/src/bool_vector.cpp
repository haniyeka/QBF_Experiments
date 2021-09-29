/*
 * This file is part of HQSpre.
 *
 * Copyright 2016/17 Ralf Wimmer, Sven Reimer, Paolo Marin, Bernd Becker
 * Albert-Ludwigs-Universitaet Freiburg, Freiburg im Breisgau, Germany
 *
 * HQSpre is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * HQSpre is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with HQSpre. If not, see <http://www.gnu.org/licenses/>.
 */

#include "bool_vector.hpp"

#include <cstring>
#include <ostream>

#include "aux.hpp"

namespace hqspre {

/**
 * \brief Constructs an empty bit vector
 */
BoolVector::BoolVector()
    :_size( 0 ),
     _binCount( 0 ),
     _lastBinMask( 0 ),
     _bins( nullptr )
{}

/**
 * \brief Constructs a bit vector with a certain size and initial values
 * \param size the size of the bit vector
 * \param initial the initial value of all entries
 */
BoolVector::BoolVector(const std::size_t size, bool initial)
    :_size( 0 ),
     _binCount( 0 ),
     _lastBinMask( 0 ),
     _bins( nullptr )
{
    if( size > 0 )
    {
        initialize( size, initial );
    }
}

/**
 * \brief Creates a copy of a given bit vector
 */
BoolVector::BoolVector( const BoolVector& v )
    :_size( v._size ),
     _binCount( v._binCount ),
     _lastBinMask( v._lastBinMask ),
     _bins( nullptr )
{
    if (_binCount != 0)
    {
        _bins = new BinType[ _binCount ];
        memcpy( _bins, v._bins, _binCount * sizeof( BinType ) );
    }
}

/**
 * \brief Moves a bit vector into a new one.
 *
 * After moving the original vector is empty.
 */
BoolVector::BoolVector( BoolVector&& v ) noexcept
    :_size( v._size ),
     _binCount( v._binCount ),
     _lastBinMask( v._lastBinMask ),
     _bins( v._bins )
{
    v._size = 0;
    v._binCount = 0;
    v._lastBinMask = 0;
    v._bins = nullptr;
}

/**
 * \brief Destroys the bit vector and frees its memory.
 */
BoolVector::~BoolVector() noexcept
{
    if( _bins != nullptr ) 
    {
        delete[] _bins;
    }
}

/**
 * \brief Assignment operator for bit vectors
 */
BoolVector& BoolVector::operator=( const BoolVector& v )
{
    if( this == &v ) return *this;

    if( _binCount != v._binCount )
    {
        _binCount = v._binCount;

        if( _bins != nullptr )
        {
            delete[] _bins;
        }

        if( v._bins != nullptr )
        {
            _bins = new BinType[ _binCount ];
        }
        else
        {
            _bins = nullptr;
        }
    }

    _size = v._size;
    _lastBinMask = v._lastBinMask;

    if( _binCount != 0 )
    {
        memcpy( _bins, v._bins, _binCount * sizeof( BinType ) );
    }

    return *this;
}


/**
 * \brief Move operator for bit vectors
 */
BoolVector& BoolVector::operator=( BoolVector&& v ) noexcept
{
    if( this == &v ) return *this;
    if( _bins != nullptr ) delete[] _bins;

    _size = v._size;
    _lastBinMask = v._lastBinMask;
    _binCount = v._binCount;
    _bins = v._bins;

    v._bins = nullptr;
    v._binCount = 0;
    v._lastBinMask = 0;
    v._size = 0;

    return *this;
}

/**
 * \brief Returns the number of bits whose value is `true`
 */
std::size_t BoolVector::countTrue() const
{
    /*
     * Use Kerninghan's method for counting 1-bits in a word. Published in 1988,
     * the C Programming Language 2nd Ed. (by Brian W. Kernighan and Dennis M. Ritchie)
     * mentions this in exercise 2-9. See also:
     * http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
     */

    std::size_t count = 0;

    std::size_t fullBins = _binCount;
    if( _lastBinMask != 0 ) --fullBins;

    /* use the fast method for the first "fullBins" bins */
    for( std::size_t i = 0; i != fullBins; ++i )
    {
        BinType b = _bins[ i ];

        while( b != 0 )
        {
            /* clear the least significant bit set */
            b &= b - 1;
            ++count;
        }
    }

    /* count 1-bits  in the last (partial) bin */
    if( _lastBinMask != 0 )
    {
        BinType b = _bins[ _binCount - 1 ] & _lastBinMask;
        while( b != 0 )
        {
            /* clear the least significant bit set */
            b &= b - 1;
            ++count;
        }
    }

    return count;
}

/**
 * \brief Returns the first index whose value is `true`; -1 if none exists.
 */
long BoolVector::getFirstTrue() const
{
    std::size_t fullBins = _binCount;
    if( _lastBinMask != 0 ) --fullBins;

    long index = 0;

    for( std::size_t i = 0; i != fullBins; ++i )
    {
        BinType b = _bins[ i ];

        if( b != 0 )
        {
            while( true )
            {
                if( b & 1 )
                {
                    return index;
                }
                else 
                {
                    b >>= 1;
                    ++index;
                }
            }
        }
        else
        {
            index += BinSize;
        }
    }

    if( _lastBinMask != 0 )
    {
        BinType b = _bins[ _binCount - 1 ] & _lastBinMask;
        if( b != 0 )
        {
            while( true )
            {
                if( b & 1 )
                {
                    return index;
                }
                else 
                {
                    b >>= 1;
                    ++index;
                }
            }
        }
    }

    return -1;
}


/**
 * \brief Initializes a bit vector to a given size and initial values
 */
void BoolVector::initialize( std::size_t size, bool initial )
{
    val_assert( uninitialized() );
    val_assert( size > 0 );

    _size = size;
    _binCount = _size / BinSize;
    _lastBinMask = 0;

    if( _size % BinSize != 0 )
    {
        ++_binCount;

        std::size_t additionalBits = _size - ( ( _binCount - 1 ) * BinSize );
        val_assert( additionalBits > 0 );

        for( std::size_t i = 0; i != additionalBits; ++i )
        {
            _lastBinMask |= (BinType)1ul << i;
        }
    }

    _bins = new BinType[ _binCount ];

    memset( _bins, ( initial ? ~0ul : 0ul ), _binCount * sizeof( BinType ) );
}


/**
 * \brief Sets a given entry to a given value.
 */
void BoolVector::set( const std::size_t index, const bool value )
{
    const std::size_t bin = index / BinSize;
    const std::size_t bit = index % BinSize;

    if( value )
    {
        _bins[ bin ] |= (BinType)1ul << bit;
    }
    else
    {
        _bins[ bin ] &= ~( (BinType)1ul << bit );
    }
}

/**
 * \brief Flips the value of a given entry
 */
void BoolVector::flip( const std::size_t index )
{
    const std::size_t bin = index / BinSize;
    const std::size_t bit = index % BinSize;

    _bins[ bin ] ^= (BinType)1ul << bit;
}

/**
 * \brief Sets all entries to a given value.
 */
void BoolVector::set( const bool value )
{
    val_assert( _binCount != 0 );
    memset( _bins, ( value ? ~0ul : 0ul ), _binCount * sizeof( BinType ) );
}

/**
 * \brief Checks if all entries have the value `true`.
 */
bool BoolVector::allTrue() const
{
    if( _lastBinMask == 0 )
    {
        for( std::size_t i = 0; i != _binCount; ++i )
        {
            if( _bins[ i ] != ~(BinType)0ul )
            {
                return false;
            }
        }
    }
    /* last bin is not full */
    else
    {
        val_assert( _binCount >= 1 );
        for( std::size_t i = 0; i != _binCount - 1; ++i )
        {
            if( _bins[ i ] != ~(BinType)0ul )
            {
                return false;
            }
        }
        if( ( _bins[ _binCount - 1 ] & _lastBinMask ) != _lastBinMask ) 
        {
            return false;
        }
    }

    return true;
}


/**
 * \brief Checks if all entries have the value `false`.
 */
bool BoolVector::allFalse() const
{
    if( _lastBinMask == 0 )
    {
        for( std::size_t i = 0; i != _binCount; ++i )
        {
            if( _bins[ i ] != (BinType)0ul )
            {
                return false;
            }
        }
    }
    /* last bin is not full */
    else
    {
        val_assert( _binCount >= 1 );
        for( std::size_t i = 0; i != _binCount - 1; ++i )
        {
            if( _bins[ i ] != (BinType)0ul )
            {
                return false;
            }
        }

        if( ( _bins[ _binCount - 1 ] & _lastBinMask ) != (BinType)0ul ) 
        {
            return false;
        }
    }

    return true;
}

/**
 * \brief Compares two bit vectors for equality.
 */
bool BoolVector::operator==( const BoolVector& b ) const
{
    if( _size != b._size ) return false;

    if( _lastBinMask == 0 )
    {
        for( std::size_t i = 0; i != _binCount; ++i )
        {
            if( _bins[ i ] != b._bins[ i ] )
            {
                return false;
            }
        }
    }
    else
    {
        val_assert( _binCount >= 1 );

        for( std::size_t i = 0; i != _binCount - 1; ++i )
        {
            if( _bins[ i ] != b._bins[ i ] )
            {
                return false;
            }
        }

        if( ( _bins[ _binCount - 1 ] & _lastBinMask ) != ( b._bins[ _binCount - 1 ] & _lastBinMask ) )
        {
            return false;
        }
    }

    return true;
}

BoolVector BoolVector::operator~() const
{
    BoolVector result( *this );
    result.flip();
    return result;
}

/**
 * \brief Flips all values in the bit vector
 */
void BoolVector::flip()
{
    for( std::size_t i = 0; i != _binCount; ++i )
    {
        _bins[ i ] = ~_bins[ i ];
    }
}

BoolVector& BoolVector::operator &=( const BoolVector& v )
{
    val_assert( _size == v._size );

    for( std::size_t i = 0; i != _binCount; ++i )
    {
        _bins[ i ] &= v._bins[ i ];
    }

    return *this;
}

BoolVector& BoolVector::operator |=( const BoolVector& v )
{
    val_assert( _size == v._size );

    for( std::size_t i = 0; i != _binCount; ++i )
    {
        _bins[ i ] |= v._bins[ i ];
    }

    return *this;
}

BoolVector& BoolVector::operator ^=( const BoolVector& v )
{
    val_assert( _size == v._size );

    for( std::size_t i = 0; i != _binCount; ++i )
    {
        _bins[ i ] ^= v._bins[ i ];
    }

    return *this;
}

bool BoolVector::intersectAndCheckEmpty( BoolVector& v1, const BoolVector& v2 )
{
    val_assert( v1._size == v2._size );

    bool empty = true;

    if( v1._lastBinMask == 0 )
    {
        for( std::size_t i = 0; i != v1._binCount; ++i )
        {
            v1._bins[ i ] &= v2._bins[ i ];
            if( v1._bins[ i ] != (BinType)0ul ) empty = false;
        }
    }
    else
    {
        for( unsigned int i = 0; i != v1._binCount - 1; ++i )
        {
            v1._bins[ i ] &= v2._bins[ i ];
            if( v1._bins[ i ] != (BinType)0ul ) empty = false;
        }
        v1._bins[ v1._binCount - 1 ] &= v2._bins[ v1._binCount - 1 ];
        if( ( v1._bins[ v1._binCount - 1 ] & v1._lastBinMask ) != (BinType)0 )
        {
            empty = false;
        }
    }

    return empty;
}

std::ostream& operator<<( std::ostream& os, const BoolVector& b )
{
    for ( std::size_t i = 0; i != b.size(); ++i )
    {
        if (b.get( i ) ) os << '1';
        else os << '0';
    }

    return os;
}

} // end namespace hqspre
