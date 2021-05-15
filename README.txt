An easy-to-use hash table implementation in C.

This was first written in 2001, but has undergone some maintenance over
the years, mostly bringing it up to date with the evolving C standard.

It's released under the MIT License. See LICENSE.txt.


Limitations
===========
- It only supports nul terminated strings as keys. To use other types of data
  as keys it has to be turned into a string first. It is possible to use
  binary data as long as it doesn't contain nul bytes, the included hash
  functions do not require actually printable strings.
- This was originally written when 32-bit architectures were the norm, so
  the hash values are 32-bit integers. (The hash_string_good depends on
  this.) This limits the maximum size for tables accordingly, but since this
  is in-memory tables, 4 GB for a single should be more than enough anyway.


The functions
=============

See hashtable.h for a description of each function.

Some notes:

Creating a hashtable
--------------------
- When creating a hash table, an initial size is required. This is used as
  a hint, it will not necessarily use that exact size, it might us a
  larger number. The table will of course grow when needed, but if the
  expected size is known beforehand you can avoid a number of initial
  rehashes by setting a suitable size from the start.
  The default size is 101.
- Minimum and maximum load decides when and how to grow the table.
  The defaults are 0.5 and 0.8. Minimum must be less than maximum. If
  unreasonable values are given, it will force them to the default values
  or values in the range [0.2, 1.0[.
- Two hash functions are provided, hash_string_fast(), and hash_string_good().
  The default is the former. The names are somewhat misleading, they are
  both very good, but have different weaknesses. hash_string_good() is
  much slower but better for particular sets of keys where hash_string_fast()
  does not perform well. (See the test results for xxx-17576.txt for example.)
  You will almost always want to use default.
  You can also provide your own hash function.
- If a deallocator is given, values are deallocated with this function when
  removed or replaced. See below about memory management.

Memory management
-----------------
- Keys are managed internally by the hash table (allocated or stored
  directly in the table), so the caller does not have to allocate space for,
  and copy, them.
- The hash table does NOT allocate or copy value data, is just stores the
  pointer.
- The caller can free removed data itself, or provide a deallocator function
  and let the hash table take care of this.

Value types
-----------
- The values are of type void*, i.e. any pointer. It's possible to store
  scalar types, e.g. integers, directly in the table, but be vary of the
  restrictions in C on casting void* to and from a numeric type. It might
  not be portable or even work on all architectures.
