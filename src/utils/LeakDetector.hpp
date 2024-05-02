#ifndef LEAKDETECTOR_HPP
#define LEAKDETECTOR_HPP

#include <iostream>
#include <cstring>

#define __NEW_OVERLOAD_IMPLEMENTATION__

void* operator new( size_t _size, char* _file, unsigned int _line );
void* operator new[]( size_t _size, char* _file, unsigned int _line );

#ifndef __NEW_OVERLOAD_IMPLEMENTATION__
#define new     new( __FILE__, __LINE__ )
#endif

class _leak_detector
{
public:
    static unsigned int callCount;
    _leak_detector() noexcept
    {
        ++callCount;
    }

    ~_leak_detector() noexcept
    {
        if( --callCount == 0 )
            LeakDetector();
    }
private:
    static unsigned int LeakDetector() noexcept;
};

static _leak_detector _exit_counter;

typedef struct _MemoryList
{
    struct  _MemoryList *next, *prev;
    size_t  size;       // size of allocated memory
    bool    isArray;    // either allocated array or not
    char    *file;      // locating leaking file
    unsigned int line;  // locating leaking line
} _MemoryList;

static unsigned long _memory_allocated = 0; // saving unreleased memory size
static _MemoryList _root =
{
    &_root, &_root, // pointers of first element all pointing to itself
    0, false,       // 0 allocated memory size, not array
    NULL, 0         // no file, line 0
};

unsigned int _leak_detector::callCount = 0;

void* AllocateMemory( size_t _size, bool _array, char *_file, unsigned _line )
{
    // calculate new size
    size_t newSize = sizeof( _MemoryList ) + _size;

    // we can only use malloc for allocation since new has already been overwritten
    _MemoryList *newElem = (_MemoryList*)malloc( newSize );

    newElem->next = _root.next;
    newElem->prev = &_root;
    newElem->size = _size;
    newElem->isArray = _array;
    newElem->file = NULL;

    // save file info if existing
    if( _file )
    {
        newElem->file = (char*)malloc( strlen( _file ) + 1 );
        strcpy( newElem->file, _file );
    }

    // save line number
    newElem->line = _line;

    // update list
    _root.next->prev = newElem;
    _root.next = newElem;

    // save to unreleased memory size
    _memory_allocated += _size;

    // return allocated memory
    return (char*)newElem + sizeof( _MemoryList );
}
void DeleteMemory( void* _ptr, bool _array )
{
    // back to the beginning of MemoryList
    _MemoryList *currentElem = (_MemoryList*)((char*)_ptr - sizeof( _MemoryList ));

    if( currentElem->isArray != _array ) return;

    // update list
    currentElem->prev->next = currentElem->next;
    currentElem->next->prev = currentElem->prev;
    _memory_allocated -= currentElem->size;

    // release allocated memory for file information
    if( currentElem->file ) free( currentElem->file );
    free( currentElem );
}

void* operator new( size_t _size )
{
    return AllocateMemory( _size, false, NULL, 0 );
}

void* operator new[]( size_t _size )
{
    return AllocateMemory( _size, true, NULL, 0 );
}

void* operator new( size_t _size, char *_file, unsigned int _line )
{
    return AllocateMemory( _size, false, _file, _line );
}

void* operator new[]( size_t _size, char *_file, unsigned int _line )
{
    return AllocateMemory( _size, true, _file, _line );
}

void operator delete( void *_ptr ) noexcept
{
    DeleteMemory( _ptr, false );
}

void operator delete( void *_ptr, const std::nothrow_t& tag ) noexcept
{
    (void)tag;
    DeleteMemory( _ptr, false );
}

void operator delete( void *_ptr, size_t sz ) noexcept
{
    (void)sz;
    DeleteMemory( _ptr, false );
}

void operator delete[]( void *_ptr ) noexcept
{
    DeleteMemory( _ptr, true );
}

void operator delete[]( void *_ptr, const std::nothrow_t& tag ) noexcept
{
    (void)tag;
    DeleteMemory( _ptr, false );
}

void operator delete[]( void *_ptr, size_t sz ) noexcept
{
    (void)sz;
    DeleteMemory( _ptr, false );
}
unsigned int _leak_detector::LeakDetector() noexcept
{
    unsigned int count = 0;

    // iterates the whole list, if there is a memory leak,
    // then _LeakRoot.next doesn't point to itself

    _MemoryList *ptr = _root.next;
    while( ptr && ptr != &_root )
    {
        // output all leaking information
        if( ptr->isArray )
        {
            std::cout << "Leaking[] ";
        }
        else
        {
            std::cout << "Leaking   ";
        }

        std::cout << ptr << ", size of " << ptr->size;

        if( ptr->file )
        {
            std::cout << " (located in " << ptr->file << ", line " << ptr->line << ")";
        }
        else
        {
            std::cout << " (no file info)";
        }

        std::cout << std::endl;

        ++count;
        ptr = ptr->next;
    }

    if( count )
        std::cout << "Existing " << count << " memory leaking behavior, " << _memory_allocated << " byte in total." << std::endl;

    return count;
}

#endif // LEAKDETECTOR_HPP