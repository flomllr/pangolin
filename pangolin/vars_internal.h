#ifndef PANGOLIN_VARS_INTERNAL_H
#define PANGOLIN_VARS_INTERNAL_H

#include <boost/utility.hpp>
#include <boost/type_traits.hpp>
#include <boost/lexical_cast.hpp>

namespace pangolin
{

struct _Var
{
  _Var() {}
  _Var(void* val, const char* type_name)
    : val(val),type_name(type_name) {}

  void* val;
  const char* type_name;

  std::string meta_friendly;
  double meta_range[2];
  int meta_flags;
};

// Forward declaration
template<typename T, typename S, class Enable1 = void, class Enable2 = void, class Enable3 = void>
struct _Accessor;

struct UnknownTypeException : std::exception {
  char const* what() const throw() { return "Unknown type in generic container"; }
};

template<typename T>
struct Accessor
{
  virtual const T& Get() const = 0;
  virtual void Set(const T& val) = 0;
  static Accessor<T>* Create(_Var& var)
  {
    if( var.type_name == typeid(double).name() ) {
      return new _Accessor<T,double>( *(double*)var.val);
    } else if( var.type_name == typeid(int).name() ) {
      return new _Accessor<T,int>( *(int*)var.val );
    } else if( var.type_name == typeid(std::string).name() ) {
      return new _Accessor<T,std::string>( *(std::string*)var.val );
    } else if( var.type_name == typeid(bool).name() ) {
      return new _Accessor<T,bool>( *(bool*)var.val );
    } else {
      throw UnknownTypeException();
    }
  }
};

template<typename T, typename S>
struct _Accessor<T,S, typename boost::enable_if_c<
    (boost::is_scalar<T>::value || boost::is_same<T,bool>::value) &&
    (boost::is_scalar<S>::value || boost::is_same<S,bool>::value) &&
      !boost::is_same<T,S>::value
  >::type> : Accessor<T>
{
  _Accessor(S& var) : var(var) {
//    std::cout << "scalar" << std::endl;
  }

  const T& Get() const
  {
    cache = (T)var;
    return cache;
  }

  void Set(const T& val)
  {
    var = (S)val;
  }

  S& var;
  mutable T cache;
};

template<typename T>
struct _Accessor<T,T> : Accessor<T>
{
  _Accessor(T& var) : var(var) {
//    std::cout << "same" << std::endl;
  }

  const T& Get() const
  {
    return var;
  }

  void Set(const T& val)
  {
    var = val;
  }
  T& var;
};

template<typename T, typename S>
struct _Accessor<T,S ,typename boost::enable_if_c<
    !((boost::is_scalar<T>::value || boost::is_same<T,bool>::value) &&
    (boost::is_scalar<S>::value || boost::is_same<S,bool>::value)) &&
    !boost::is_same<T,S>::value
>::type> : Accessor<T>
{
  _Accessor(S& var) : var(var) {
//    std::cout << "lexical" << std::endl;
  }

  const T& Get() const
  {
    cache = boost::lexical_cast<T>(var);
    return cache;
  }

  void Set(const T& val)
  {
    var = boost::lexical_cast<S>(val);
  }
  S& var;
  mutable T cache;
};

}

#endif // PANGOLIN_VARS_INTERNAL_H

