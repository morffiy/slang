#ifndef FUNDAMENTAL_LIB_SMART_POINTER_H
#define FUNDAMENTAL_LIB_SMART_POINTER_H

#include "common.h"
#include "type-traits.h"

#include <assert.h>

namespace Slang
{
    // TODO: Need to centralize these typedefs
    typedef uintptr_t UInt;
    typedef intptr_t Int;

    // Base class for all reference-counted objects
    class RefObject
    {
    private:
        UInt referenceCount;

    public:
        RefObject()
            : referenceCount(0)
        {}

        RefObject(const RefObject &)
            : referenceCount(0)
        {}

        virtual ~RefObject()
        {}

        void addReference()
        {
            referenceCount++;
        }

        void decreaseReference()
        {
            --referenceCount;
        }

        void releaseReference()
        {
            SLANG_ASSERT(referenceCount != 0);
            if(--referenceCount == 0)
            {
                delete this;
            }
        }

        bool isUniquelyReferenced()
        {
            SLANG_ASSERT(referenceCount != 0);
            return referenceCount == 1;
        }

        UInt debugGetReferenceCount()
        {
            return referenceCount;
        }
    };

    inline void addReference(RefObject* obj)
    {
        if(obj) obj->addReference();
    }

    inline void releaseReference(RefObject* obj)
    {
        if(obj) obj->releaseReference();
    }

    // "Smart" pointer to a reference-counted object
    template<typename T>
    struct RefPtr
    {
        RefPtr()
            : pointer(0)
        {}

        RefPtr(T* p)
            : pointer(p)
        {
            addReference(p);
        }

        RefPtr(RefPtr<T> const& p)
            : pointer(p.pointer)
        {
            addReference(p.pointer);
        }

        RefPtr(RefPtr<T>&& p)
            : pointer(p.pointer)
        {
            p.pointer = 0;
        }

        template <typename U>
        RefPtr(RefPtr<U> const& p,
            typename EnableIf<IsConvertible<T*, U*>::Value, void>::type * = 0)
            : pointer((U*) p)
        {
            addReference((U*) p);
        }

#if 0
        void operator=(T* p)
        {
            T* old = pointer;
            addReference(p);
            pointer = p;
            releaseReference(old);
        }
#endif

        void operator=(RefPtr<T> const& p)
        {
            T* old = pointer;
            addReference(p.pointer);
            pointer = p.pointer;
            releaseReference(old);
        }

        void operator=(RefPtr<T>&& p)
        {
            T* old = pointer;
            pointer = p.pointer;
            p.pointer = old;
        }

        template <typename U>
        typename EnableIf<IsConvertible<T*, U*>::value, void>::type
            operator=(RefPtr<U> const& p)
        {
            T* old = pointer;
            addReference(p.pointer);
            pointer = p.pointer;
            releaseReference(old);
        }

		int GetHashCode()
		{
			return (int)(long long)(void*)pointer;
		}

        bool operator==(const T * ptr) const
        {
            return pointer == ptr;
        }

        bool operator!=(const T * ptr) const
        {
            return pointer != ptr;
        }

		bool operator==(RefPtr<T> const& ptr) const
		{
			return pointer == ptr.pointer;
		}

		bool operator!=(RefPtr<T> const& ptr) const
		{
			return pointer != ptr.pointer;
		}

        template<typename U>
        RefPtr<U> As() const
        {
            RefPtr<U> result(dynamic_cast<U*>(pointer));
            return result;
        }

        ~RefPtr()
        {
            releaseReference((Slang::RefObject*) pointer);
        }

        T& operator*() const
        {
            return *pointer;
        }

        T* operator->() const
        {
            return pointer;
        }

		T * Ptr() const
		{
			return pointer;
		}

        operator T*() const
        {
            return pointer;
        }

        T* detach()
        {
            if (pointer)
                dynamic_cast<RefObject*>(pointer)->decreaseReference();
            auto rs = pointer;
            pointer = nullptr;
            return rs;
        }

    private:
        T* pointer;
        
	};
}

#endif