#ifndef _SPTR_HXX_
#define _SPTR_HXX_

template < class T >
class Sptr
{
public:
    T* ptr;
    int* count;
private:
    /// increment the reference count.
    void increment()
    {
        if (ptr)
        {
            if (!count)
            {
                count = new int(0);
            }
                
            (*count)++;
        }
    }

    /// decrement the reference count
    void decrement()
    {
        if (ptr && count)
        {
            (*count)--;
            if ((*count == 0))
            {
               delete count; count = 0;
               delete ptr; ptr = 0;
            }
        }
        ptr = 0;
        count = 0;
    }

public:
#if 1
    template < class T2 >
        operator Sptr<T2 > () 
    {
        return Sptr < T2 > (ptr, count);
    }

    template < class T2 >
       operator const Sptr<T2 > () const
    {
        return Sptr < T2 > (ptr, count);
    }
#endif

    Sptr() : ptr(0), count(0)
    {
    }
    
    Sptr(T* original, int* myCount = 0)
        : ptr(original), count(myCount)
    {
        if (ptr)
        {
            increment();
        }
    }

    template <class T2>
        Sptr(const Sptr<T2>& x)
        : ptr(x.ptr), count(x.count)
    {
        increment();
    }

    Sptr(const Sptr& x)
        : ptr(x.ptr), count(x.count)
    {
        increment();
    }

    ~Sptr()
    {
        decrement();
    }

    int operator!() const
    {
        if (ptr)
        {
            return 0;
        }
        return 1;
    }

    T* operator->() const
    {
        return ptr;
    }
  
    template < class T2 >
    Sptr& operator=(const Sptr < T2 > & x)
    {
        if (ptr == x.getPtr()) return * this;
        
        decrement();
        ptr = x.getPtr();
        count = x.getCount();
        increment();
        return *this;
    }


    Sptr& operator=(T* original)
    {
        if (ptr == original) return * this;

        decrement();
        ptr = original;
        increment();
        return *this;
    };

    Sptr& operator=(const Sptr& x)
    {
        if (ptr == x.getPtr()) return * this;
        
        decrement();
        ptr = x.ptr;
        count = x.count;
        increment();
        return *this;
    }
	  
	  friend bool operator!=(const void* y, const Sptr& x)
    {
        if (x.ptr != y)
            return true;
        else
            return false;
    }

	  friend bool operator!=(const Sptr& x, const void* y)
    {
        if (x.ptr != y)
            return true;
        else
            return false;
    }
    
    bool operator!=(const Sptr& x) const
    {
        if (x.ptr != ptr)
            return true;
        else
            return false;
    }

	  friend bool operator==(const void* y, const Sptr& x)
    {
        if (x.ptr == y)
            return true;
        else
            return false;
    }

	  friend bool operator==(const Sptr& x, const void* y)
    {
        if (x.ptr == y)
            return true;
        else
            return false;
    }

	  bool operator==(const Sptr& x) const
    {
        if (x.ptr == ptr)
            return true;
        else
            return false;
    }

    int* getCount() const
    {
        return count;
    }

    T* getPtr() const
    {
        return ptr;
    }
};


#endif

