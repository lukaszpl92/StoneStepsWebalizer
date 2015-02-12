/*
   webalizer - a web server log analysis program

   Copyright (c) 2004-1013, Stone Steps Inc. (www.stonesteps.ca)

   See COPYING and Copyright files for additional licensing and copyright information     

   scnode.h
*/
#ifndef __SCNODE_H
#define __SCNODE_H

#include "vector.h"
#include "types.h"
#include "keynode.h"
#include "datanode.h"

// -----------------------------------------------------------------------
//
// HTTP status code node
//
// -----------------------------------------------------------------------
// 1. IMPORTANT: code is a reference to keynode_t<u_int>::nodeid. If 
// node's memory is reallocated, this reference will point to an invalid
// memory block (old nodeid's location). Make sure to use proper copy 
// operations to maintain scnode_t's.
//
struct scnode_t : public keynode_t<u_int>, public datanode_t<scnode_t> {
   uint64_t       count;         // number of hits

   public:
      typedef void (*s_unpack_cb_t)(scnode_t& scnode, void *arg);

   public:
      scnode_t(void) : keynode_t<u_int>(0) {count = 0;}

      scnode_t(u_int code) : keynode_t<u_int>(code) {count = 0;}

      scnode_t(const scnode_t& node) : keynode_t<u_int>(node) {count = node.count;}
      
      scnode_t& operator = (const scnode_t& node) {keynode_t<u_int>::operator = (node); count = node.count; return *this;}

      u_int get_ccode(void) const {return nodeid;}

      //
      // serialization
      //
      size_t s_data_size(void) const;
      size_t s_pack_data(void *buffer, size_t bufsize) const;
      size_t s_unpack_data(const void *buffer, size_t bufsize, s_unpack_cb_t upcb, void *arg);

      static size_t s_data_size(const void *buffer);
};

// -----------------------------------------------------------------------
//
// sc_hash_table
//
// -----------------------------------------------------------------------
// 1. The first element is reserved for the unknown code, which is 
// returned when the requested code is not found or if the index is 
// out of range.
//
// 2. HTTP status codes are expected to be inserted in the ascending 
// order. Codes inserted out of order will be discarded and the first, 
// unknown code, will be returned when such code is requested.
//
// 3. IMPORTANT: scnode_t uses a reference to its base member (nodeid)
// and the stcodes vector must be initialized not to copy existing
// memory directly, but instead use stcode_t's copy constructor and
// assignment operator.
//
class sc_hash_table {
   vector_t<scnode_t>   stcodes;       // HTTP status codes

   size_t               clsindex[6];   // HTTP class group offsets (0th is not used)

   public:
      sc_hash_table(u_int maxcodes);

      ~sc_hash_table(void);

      void add_status_code(u_int code);

      scnode_t& get_status_code(u_int code);

      size_t size(void) const {return stcodes.size();}

      scnode_t& operator [] (size_t index) {return const_cast<scnode_t&>(const_cast<const sc_hash_table*>(this)->operator[](index));}

      const scnode_t& operator [] (size_t index) const;
};

#endif // __SCNODE_H