//
// Distributed under the ITensor Library License, Version 1.2
//    (See accompanying LICENSE file.)
//
#ifndef __ITENSOR_INDEX_H
#define __ITENSOR_INDEX_H
#include "global.h"

namespace itensor {

enum IndexType { Link, Site, All, NullIndex, Xind, Yind, Zind, Wind, Vind };

//Forward declarations
class IndexVal;

//
// Index
//
// Represents a tensor index of fixed bond dimension m.
//
// Can be compared with == operator (returns true if both
// are copies of the same Index instance).
//
// To make an Index distinct from other copies, increase its primeLevel.
//
class Index
    {
    public:
    using IDGenerator = mt19937;
    using IDType = IDGenerator::result_type;
    using IndexValT = IndexVal;

    //
    // Constructors
    //

    Index();

    // Name of Index is used for printing purposes
    explicit
    Index(const std::string& name, 
          long m = 1, 
          IndexType it = Link, 
          int primelev = 0);

    //
    // Accessor Methods
    //

    // Returns the bond dimension
    long 
    m() const { return m_; }

    // Returns the prime level
    int 
    primeLevel() const { return primelevel_; }
    // Sets the prime level to a specified value.
    Index& 
    primeLevel(int plev);

    // Returns the IndexType
    IndexType 
    type() const { return type_; }

    // Returns the name of this Index
    std::string 
    name() const;

    // Returns the name of this Index with primes removed
    const std::string&
    rawname() const { return sname_; }

    // Evaluates to false if Index is default constructed.
    explicit operator bool() const { return (type_!=NullIndex); }

    // (Explicitly) convertible to integer types
    explicit operator int() const { return m(); }
    explicit operator long() const { return m(); }


    // Returns the Arrow direction of this Index
    Arrow 
    dir() const { return Out; }
    void 
    dir(Arrow ndir) const {  }


    //
    // Prime level methods
    //

    // Increase primelevel by 1 (or by optional amount inc)
    Index& 
    prime(int inc = 1);

    // Increase primelevel by 1 (or optional amount inc)
    // if type matches this Index or type==All
    Index& 
    prime(IndexType type, int inc = 1);

    // Set primelevel to zero (optionally only if type matches)
    Index& 
    noprime(IndexType type = All) { prime(type,-primelevel_); return *this; }

    // Switch primelevel from plevold to plevnew
    // Has no effect if plevold doesn't match current primelevel
    Index& 
    mapprime(int plevold, int plevnew, IndexType type = All);

    //
    // Operators
    //

    // Equality (==) operator
    // True if other Index is a copy of this Index and has same primelevel
    bool 
    operator==(const Index& other) const;

    bool 
    operator!=(const Index& other) const { return !operator==(other); }

    // Check if other Index is a copy of this, ignoring primeLevel
    bool 
    noprimeEquals(const Index& other) const;

    // Useful for sorting Index objects
    bool 
    operator>(const Index& other) const;
    bool 
    operator<(const Index& other) const;

    // Creates an IndexVal from this Index with value i
    IndexVal 
    operator()(long i) const;

    //
    // Other methods
    //

    // Conjugate this Index.
    // Currently has no effect; exists for forward compatibility
    // with Arrows and interface compatibility with class IQIndex.
    void 
    dag() { } //for forward compatibility with arrows

    // Write Index to binary output stream.
    void 
    write(std::ostream& s) const;

    // Read Index from binary input stream.
    Index& 
    read(std::istream& s);

    IDType
    id() const { return id_; }


    private:

    /////////////
    IDType id_;
    int primelevel_; 
    long m_;
    IndexType type_;
    std::string sname_;
    /////////////

    Index::IDType 
    generateID()
        {
        static Index::IDGenerator rng(std::time(NULL) + getpid());
        return rng();
        }

    }; //class Index


//
// IndexVal
//
// Class pairing an Index of dimension m
// with a specific value i where 1 <= i <= m
//
class IndexVal
    {
    public:

    Index index;
    long val;

    IndexVal();

    IndexVal(const Index& index, long val_);

    bool
    operator==(const IndexVal& other) const;

    bool
    operator!=(const IndexVal& other) const { return !operator==(other); }

    bool
    operator==(const Index& I) const { return index == I; }

    long
    m() const { return index.m(); }

    explicit operator bool() const { return bool(index); }

    IndexVal& 
    prime(int inc = 1) { index.prime(inc); return *this; }

    IndexVal& 
    prime(IndexType type, int inc = 1) { index.prime(type,inc); return *this; }

    IndexVal& 
    noprime(IndexType type = All) { index.noprime(type); return *this; }

    IndexVal& 
    mapprime(int plevold, int plevnew, IndexType type = All) 
        { index.mapprime(plevold,plevnew,type); return *this; }

    void
    dag() { }

    //static const IndexVal& 
    //Null();
    };

bool inline
operator==(const Index& I, const IndexVal& iv)
    {
    return iv.operator==(I);
    }

Index inline
dag(Index res) { res.dag(); return res; }

IndexVal inline
dag(IndexVal res) { res.dag(); return res; }


//Return a copy of I, increasing primelevel.
template<class T>
T
prime(T I, int inc = 1) { I.prime(inc); return I; }

//Return a copy of I, increasing primelevel if I.type() == type
template<class T>
T 
prime(T I, IndexType type, int inc = 1) { I.prime(type,inc); return I; }

template<class T>
T
noprime(T I, IndexType type = All) { I.noprime(type); return I; }

//Return a copy of I with prime level changed to plevnew if
//old prime level was plevold. Otherwise has no effect.
template<class T>
T
mapPrime(T I, int plevold, int plevnew, IndexType type = All)
    { I.mapprime(plevold,plevnew,type); return I; }


//Returns a string version of this Index's bond dimension.
std::string
showm(const Index& I);

std::string 
nameint(const std::string& f, int n);

std::ostream& 
operator<<(std::ostream & s, const Index &t);

std::ostream& 
operator<<(std::ostream& s, const IndexVal& iv);

std::ostream& 
operator<<(std::ostream& s, IndexType it);



//
//
// Implementations
//
//

inline Index::
Index() 
    : 
    id_(0),
    primelevel_(0),
    m_(1),
    type_(NullIndex)
    { }

inline Index::
Index(const std::string& name, long m, IndexType type, int plev) 
    : 
    id_(generateID()),
    primelevel_(plev),
    m_(m),
    type_(type),
    sname_(name)
    { 
#ifdef DEBUG
    if(type_ == All) Error("Constructing Index with type All disallowed");
    if(type_ == NullIndex) Error("Constructing Index with type NullIndex disallowed");
#endif
    }

bool inline Index::
operator==(const Index& other) const 
    { 
    return (id_ == other.id_) && (primelevel_ == other.primelevel_); 
    }

bool inline Index::
noprimeEquals(const Index& other) const
    { 
    return (id_ == other.id_);
    }

bool inline Index::
operator>(const Index& other) const 
    { 
    if(m_ == other.m_) 
        {
        if(id_ == other.id_) return primelevel_ > other.primelevel_;
        return id_ > other.id_;
        }
    return m_ > other.m_;
    }

bool inline Index::
operator<(const Index& other) const
    {
    if(m_ == other.m_) 
        {
        if(id_ == other.id_) return primelevel_ < other.primelevel_;
        return id_ < other.id_;
        }
    return m_ < other.m_;
    }

IndexVal inline Index::
operator()(long val) const { return IndexVal(*this,val); }

inline
Index& Index::
prime(int inc) 
    { 
    primelevel_ += inc; 
#ifdef DEBUG
    if(primelevel_ < 0)
        {
        Error("Negative primeLevel");
        }
#endif
    return *this;
    }

}; //namespace itensor

#endif
