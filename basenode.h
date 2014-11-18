/*
    webalizer - a web server log analysis program

    Copyright (c) 2004-2014, Stone Steps Inc. (www.stonesteps.ca)

    See COPYING and Copyright files for additional licensing and copyright information

    basenode.h
*/
#ifndef __BASENODE_H
#define __BASENODE_H

#include "tstring.h"
#include "types.h"
#include "hashtab.h"
#include "keynode.h"
#include "datanode.h"

// -----------------------------------------------------------------------
//
// base_node
//
// -----------------------------------------------------------------------
template <typename node_t> 
struct base_node : public htab_node_t<node_t>, public keynode_t<u_long>, public datanode_t<node_t> {
      string_t string;              // node value (URL, user agent, etc)
      nodetype_t flag;              // object type (REG, GRP)

      public:
         base_node(u_long nodeid = 0);
         base_node(const base_node& node);
         base_node(const char *str);
         base_node(const string_t& str);

         virtual ~base_node(void) {}

         void reset(u_long nodeid = 0);

         const string_t& key(void) const {return string;}

         nodetype_t get_type(void) const {return flag;}

         //
         // serialization
         //
         //    key  : nodeid (u_long)
         //    data : flag (u_char), string (string_t)
         //    value: hash(string) (u_long)
         //
         u_int s_data_size(void) const;

         u_int s_pack_data(void *buffer, u_int bufsize) const;
         u_int s_unpack_data(const void *buffer, u_int bufsize);

         u_long s_hash_value(void) const;

         int s_compare_value(const void *buffer, u_int bufsize) const;

         static u_int s_data_size(const void *buffer);

         static const void *s_field_value(const void *buffer, u_int bufsize, u_int& datasize);

         static int s_compare_value_hash(const void *buf1, const void *buf2);

         static bool s_is_group(const void *buffer, u_int bufsize);
};

#endif // __BASENODE_H
