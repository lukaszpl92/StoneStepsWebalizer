/*
    webalizer - a web server log analysis program

    Copyright (c) 2004-2017, Stone Steps Inc. (www.stonesteps.ca)

    See COPYING and Copyright files for additional licensing and copyright information

    rnode.h
*/
#ifndef RNODE_H
#define RNODE_H

#include "hashtab.h"
#include "types.h"
#include "basenode.h"
#include "storable.h"

///
/// @struct rnode_t
///
/// @brief  Referrer node
///
struct rnode_t : public base_node<rnode_t> {   
      uint64_t count;               // request count
      uint64_t visits;              // visits started

      public:
         typedef void (*s_unpack_cb_t)(rnode_t& rnode, void *arg);

      public:
         rnode_t(void) : base_node<rnode_t>() {count = 0;}
         rnode_t(const rnode_t& rnode);
         rnode_t(const string_t& ref);

         //
         // serialization
         //
         size_t s_data_size(void) const;
         size_t s_pack_data(void *buffer, size_t bufsize) const;
         size_t s_unpack_data(const void *buffer, size_t bufsize, s_unpack_cb_t upcb, void *arg);

         static size_t s_data_size(const void *buffer);

         static const void *s_field_value_hash(const void *buffer, size_t bufsize, size_t& datasize);
         static const void *s_field_hits(const void *buffer, size_t bufsize, size_t& datasize);

         static int64_t s_compare_hits(const void *buf1, const void *buf2);
};

//
// Referrers
//
class r_hash_table : public hash_table<storable_t<rnode_t>> {
};

#endif // RNODE_H
