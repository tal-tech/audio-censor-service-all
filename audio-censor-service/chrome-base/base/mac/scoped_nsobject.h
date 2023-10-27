// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MAC_SCOPED_NSOBJECT_H_
#define BASE_MAC_SCOPED_NSOBJECT_H_

#include <type_traits>

// Include NSObject.h directly because Foundation.h pulls in many dependencies.
// (Approx 100k lines of code versus 1.5k for NSObject.h). scoped_nsobject gets
// singled out because it is most typically included from other header files.
#import <Foundation/NSObject.h>

#include "base/compiler_specific.h"
#include "base/mac/scoped_typeref.h"

@class NSAutoreleasePool;

namespace base {

// scoped_nsobject<> is patterned after scoped_ptr<>, but maintains ownership
// of an NSObject subclass object.  Style deviations here are solely for
// compatibility with scoped_ptr<>'s interface, with which everyone is already
// familiar.
//
// scoped_nsobject<> takes ownership of an object (in the constructor or in
// reset()) by taking over the caller's existing ownership claim.  The caller
// must own the object it gives to scoped_nsobject<>, and relinquishes an
// ownership claim to that object.  scoped_nsobject<> does not call -retain,
// callers have to call this manually if appropriate.
//
// scoped_nsprotocol<> has the same behavior as scoped_nsobject, but can be used
// with protocols.
//
// scoped_nsobject<> is not to be used for NSAutoreleasePools. For
// NSAutoreleasePools use ScopedNSAutoreleasePool from
// scoped_nsautorelease_pool.h instead.
// We check for bad uses of scoped_nsobject and NSAutoreleasePool at compile
// time with a template specialization (see below).

namespace internal {

template <typename NST>
struct ScopedNSProtocolTraits {
  static NST InvalidValue() { return nil; }
  static NST Retain(NST nst) { return [nst retain]; }
  static void Release(NST nst) { [nst release]; }
};

}  // namespace internal

template <typename NST>
class scoped_nsprotocol
    : public ScopedTypeRef<NST, internal::ScopedNSProtocolTraits<NST>> {
 public:
  using ScopedTypeRef<NST,
                      internal::ScopedNSProtocolTraits<NST>>::ScopedTypeRef;

  // Shift reference to the autorelease pool to be released later.
  NST autorelease() { return [this->release() autorelease]; }
};

// Free functions
template <class C>
void swap(scoped_nsprotocol<C>& p1, scoped_nsprotocol<C>& p2) {
  p1.swap(p2);
}

template <class C>
bool operator==(C p1, const scoped_nsprotocol<C>& p2) {
  return p1 == p2.get();
}

template <class C>
bool operator!=(C p1, const scoped_nsprotocol<C>& p2) {
  return p1 != p2.get();
}

template <typename NST>
class scoped_nsobject : public scoped_nsprotocol<NST*> {
 public:
  using scoped_nsprotocol<NST*>::scoped_nsprotocol;

  static_assert(std::is_same<NST, NSAutoreleasePool>::value == false,
                "Use ScopedNSAutoreleasePool instead");
};

// Specialization to make scoped_nsobject<id> work.
template<>
class scoped_nsobject<id> : public scoped_nsprotocol<id> {
 public:
  using scoped_nsprotocol<id>::scoped_nsprotocol;
};

}  // namespace base

#endif  // BASE_MAC_SCOPED_NSOBJECT_H_
