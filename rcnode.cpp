/*
    webalizer - a web server log analysis program

    Copyright (c) 2004-2013, Stone Steps Inc. (www.stonesteps.ca)

    See COPYING and Copyright files for additional licensing and copyright information

    rcnode.cpp
*/
#include "pch.h"

#include "rcnode.h"
#include "serialize.h"
#include "util.h"
#include "exception.h"

#include <typeinfo>

//
//
//

rcnode_t::rcnode_t(const rcnode_t& rcnode) : base_node<rcnode_t>(rcnode)
{
   count = rcnode.count;
   hexenc = rcnode.hexenc;
   method = rcnode.method;
   respcode = rcnode.respcode;
}

rcnode_t::rcnode_t(const char *_method, const char *url, u_int _respcode) : base_node<rcnode_t>(url) 
{
   count = 0; 
   hexenc = (url && strchr(url, '%')) ? true : false;
   method = _method;
   respcode = _respcode;
}


bool rc_hash_table::compare(const rcnode_t *nptr, const void *param) const
{
   param_block *pb = (param_block*) param;

   if(nptr->respcode != pb->respcode)
      return false;

   if(strcmp(nptr->method, pb->method))
      return false;

   return (!strcmp(nptr->string, pb->url)) ? true : false;
}

//
// serialization
//

u_int rcnode_t::s_data_size(void) const
{
   return base_node<rcnode_t>::s_data_size() + sizeof(u_char) + sizeof(u_short) + sizeof(u_long) * 2 + s_size_of(method);
}

u_int rcnode_t::s_pack_data(void *buffer, u_int bufsize) const
{
   u_int basesize, datasize;
   void *ptr = buffer;

   basesize = base_node<rcnode_t>::s_data_size();
   datasize = s_data_size();

   if(bufsize < datasize)
      return 0;

   base_node<rcnode_t>::s_pack_data(buffer, bufsize);
   ptr = &((u_char*)buffer)[basesize];

   ptr = serialize(ptr, hexenc);
   ptr = serialize(ptr, respcode);
   ptr = serialize(ptr, count);
   ptr = serialize(ptr, method);

   ptr = serialize(ptr, s_hash_value());

   return datasize;
}

u_int rcnode_t::s_unpack_data(const void *buffer, u_int bufsize, s_unpack_cb_t upcb, void *arg)
{
   u_int basesize, datasize;
   const void *ptr = buffer;

   basesize = base_node<rcnode_t>::s_data_size(buffer);
   datasize = s_data_size(buffer);

   if(bufsize < datasize)
      return 0;

   base_node<rcnode_t>::s_unpack_data(buffer, bufsize);
   ptr = &((u_char*)buffer)[basesize];

   ptr = deserialize(ptr, hexenc);
   ptr = deserialize(ptr, respcode);
   ptr = deserialize(ptr, count);
   ptr = deserialize(ptr, method);

   ptr = s_skip_field<u_long>(ptr);      // value hash
   
   if(upcb)
      upcb(*this, arg);

   return datasize;
}

u_long rcnode_t::s_hash_value(void) const
{
   return hash_str(hash_num(base_node<rcnode_t>::s_hash_value(), respcode), method, method.length());
}

u_int rcnode_t::s_data_size(const void *buffer)
{
   u_int basesize = base_node<rcnode_t>::s_data_size(buffer);
   // value hash follows the method when serialized
   return basesize + sizeof(u_char) + sizeof(u_short) + sizeof(u_long) * 2 + s_size_of<string_t>((u_char*)buffer + basesize + sizeof(u_char) + sizeof(u_short) + sizeof(u_long));
}

const void *rcnode_t::s_field_value_mp_url(const void *buffer, u_int bufsize, u_int& datasize)
{
   return base_node<rcnode_t>::s_field_value(buffer, bufsize, datasize);
}

const void *rcnode_t::s_field_value_mp_method(const void *buffer, u_int bufsize, u_int& datasize)
{
   const void *ptr = (u_char*) buffer + base_node<rcnode_t>::s_data_size(buffer) + sizeof(u_char) + sizeof(u_short) + sizeof(u_long);
   datasize = s_size_of<string_t>(ptr);
   return ptr;
}

const void *rcnode_t::s_field_value_mp_respcode(const void *buffer, u_int bufsize, u_int& datasize)
{
   datasize = sizeof(u_short);
   return (u_char*) buffer + base_node<rcnode_t>::s_data_size(buffer) + sizeof(u_char);
}

const void *rcnode_t::s_field_value_hash(const void *buffer, u_int bufsize, u_int& datasize)
{
   const void *ptr;
   datasize = sizeof(u_long);
   ptr = (u_char*) buffer + base_node<rcnode_t>::s_data_size(buffer) + sizeof(u_char) + sizeof(u_short) + sizeof(u_long);
   return (u_char*) ptr + s_size_of<string_t>(ptr);
}

int rcnode_t::s_compare_value(const void *buffer, u_int bufsize) const
{
   u_int datasize;
   string_t tstr;
   u_short tcode;
   int diff;

   if(bufsize < s_data_size(buffer))
      throw exception_t(0, string_t::_format("Record size is smaller than expected (node: %s; size: %d; expected: %d)", typeid(*this).name(), bufsize, s_data_size()));

   if((diff = base_node<rcnode_t>::s_compare_value(buffer, bufsize)) != 0)
      return diff;

   deserialize(s_field_value_mp_respcode(buffer, bufsize, datasize), tcode);

   // tcode and respcode are both u_short (no overflow)
   if((diff = tcode - respcode) != 0)
      return diff;

   deserialize(s_field_value_mp_method(buffer, bufsize, datasize), tstr);

   return method.compare(tstr);
}

const void *rcnode_t::s_field_hits(const void *buffer, u_int bufsize, u_int& datasize)
{
   datasize = sizeof(u_long);
   return (u_char*) buffer + base_node<rcnode_t>::s_data_size(buffer) + sizeof(u_char) + sizeof(u_short);
}

int rcnode_t::s_compare_hits(const void *buf1, const void *buf2)
{
   return s_compare<u_long>(buf1, buf2);
}
