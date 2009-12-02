/*
  This file is part of MADNESS.

  Copyright (C) <2007> <Oak Ridge National Laboratory>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  For more information please contact:

  Robert J. Harrison
  Oak Ridge National Laboratory
  One Bethel Valley Road
  P.O. Box 2008, MS-6367

  email: harrisonrj@ornl.gov
  tel:   865-241-3937
  fax:   865-572-0680


  $Id$
*/


#ifndef MADNESS_WORLD_WORLDREF_H__INCLUDED
#define MADNESS_WORLD_WORLDREF_H__INCLUDED

/// \file worldref.h
/// \brief Implements RemoteReference which is for internal use

namespace madness {

    template <typename T> class RemoteReference;

    template <typename T>
    std::ostream& operator<<(std::ostream& s, const RemoteReference<T>& ref);

    /// Simple structure used to manage references/pointers to remote instances

    /// This class was intended only for internal use and is still rather
    /// poorly thought through, however, it seems to fill a wider need.
    ///
    /// Can be copied/sent as a simple, contiguous block of memory.
    ///
    /// !!! Unlike SharedPtr, copying or assigning does NOT cause any
    /// reference count to be incremented.  A good policy is that
    /// ownership is transferred to whomever you give a copy.
    ///
    /// !!! Every time you send a reference to another process use a
    /// new RemoteReference so that any SharedPtr reference count is
    /// correctly incremented.
    ///
    /// !!! If a shared pointer is passed into the constructor, its
    /// reference count is incremented.  It is YOUR RESPONSIBILITY to
    /// eventually invoke (locally or remotely) RemoteReference.dec()
    /// to decrement the reference count.  This is so that you can
    /// eliminate the implied communcation by invoking it locally as a
    /// side-effect of other communication.  If dec() is not called,
    /// you will have a memory leak.
    template <typename T>
    class RemoteReference {
        friend std::ostream& operator<< <T> (std::ostream& s, const RemoteReference<T>& ref);

    private:
        mutable SharedPtr<T> ptr;  ///< Shared pointer (internally marked as not owned)
        mutable ProcessID rank;    ///< MPI rank of the owner
        mutable unsigned long id;  ///< Id of the world valid in all participating processes

        void static dec_handler(const AmArg& arg) {
            RemoteReference r;
            arg & r;
            //madness::print(arg.get_world()->rank(),"RemoteRef::dec_handler",arg.get_src(),(void *) r.get());
            r.dec();
        };

    public:
        /// Makes a non-shared (no reference count) null pointer
        RemoteReference()
                : ptr(0)
                , rank(-1)
                , id(0xffffffff) {};


        /// Makes a shared reference and increments ptr reference count
        explicit RemoteReference(World& world, SharedPtr<T>& shptr)
                : ptr(shptr)
                , rank(world.mpi.rank())
                , id(world.id()) {
            MADNESS_ASSERT(ptr.owned());
            ptr.mark_as_unowned();
        };


        /// Call this when you logically release the remote reference

        /// Can be called locally or remotely.
        ///
        /// When invoked, the internal information is destroyed by
        /// default since the reference is no longer valid.
        void dec() const {
            if (ptr) {
                World* world = World::world_from_id(id);
                //madness::print(world->mpi.rank(),"RemoteRef::dec owner", owner(), "ptr",  (void *) get(), " count ", ptr.use_count());
                if (rank == world->mpi.rank()) {
                    ptr.dec_not_owned();
                }
                else {
                    world->am.send(rank, dec_handler, new_am_arg(*this));
                }
                ptr = SharedPtr<T>(0);   // Invalidate all contents
                id = 0xffffffff;
                rank = -1;
            }
        };


        /// Returns true if holding a non-zero pointer
        inline operator bool() const {
            return ptr;
        };


        /// Returns rank of owning process, or -1 if not initialized
        inline ProcessID owner() const {
            return rank;
        };


        /// Returns possibly remote pointer which will be 0 if not initialized
        inline T* get() const {
            return ptr.get();
        };

        /// Returns id of owning world, or -1 if not initialized
        inline int world_id() const {
            return id;
        };

        void print(const char* msg = "") const {
            madness::print("RemoteReference:",msg,": ptr rank id :", (void *) ptr, rank, id);
        };

        template <typename Archive>
        void serialize(const Archive& ar) {
            ar & archive::wrap_opaque(*this);
        }
    };

#ifdef WORLD_INSTANTIATE_STATIC_TEMPLATES
    template <typename T>
    std::ostream& operator<<(std::ostream& s, const RemoteReference<T>& ref) {
        s << "<remote: ptr=" << (void *) ref.ptr << ", rank=" << ref.rank << ", id=" << ref.id << ">";
        return s;
    }
#endif


}

#endif // MADNESS_WORLD_WORLDREF_H__INCLUDED
