/* ===================================================
Flag.h
* ====================================================
* Managing internal 16 bits flag with enum
*
* Created on: 24 june 2015
* Author: 
*
* ===================================================
*/

#ifndef __FLAG_H__
#define __FLAG_H__

template<class eType> class Flags {
 public:
  Flags(eType init = 0);
  void setFlags(eType flags);
  void unsetFlags(eType flags);  
  bool isFlagsSet(eType flags) const;
  void reset();
  
 private:
  eType _flags;
};


template<class eType> Flags<eType>::Flags(eType init) :
_flags(init)
{

}

template<class eType> void Flags<eType>::setFlags(eType flags)
{
  _flags |= flags;
}

template<class eType> void Flags<eType>::unsetFlags(eType flags)
{
  _flags &= ~flags;
}

template<class eType> bool Flags<eType>::isFlagsSet(eType flags) const
{
  return ((_flags & flags) == flags);
}

template<class eType> void Flags<eType>::reset()
{
  _flags = 0;
}

#endif //__FLAG_H__
