/*
    webalizer - a web server log analysis program

    Copyright (c) 2004-2013, Stone Steps Inc. (www.stonesteps.ca)

    See COPYING and Copyright files for additional licensing and copyright information

    unode.h
*/
#ifndef __UNODE_H
#define __UNODE_H

#include "hashtab.h"
#include "basenode.h"
#include "types.h"

// -----------------------------------------------------------------------
//
// unode_t
//
// -----------------------------------------------------------------------
// 1. vstref is not a generic reference count, but rather an app-level
// indicator of how many visit nodes refer to this URL. That is, if 
// vstref is zero, unode_t may still be valid (although may be swapped
// out of memory at any time). 
//
typedef struct unode_t : public base_node<unode_t> {
      bool     hexenc : 1;           // any %xx sequences?
      bool     target : 1;           // Target URL?
      u_char   urltype;					 /* URL type (e.g. URL_TYPE_HTTP)*/
      u_short  pathlen;              /* URL path length              */
      u_long   count;                /* requests counter             */
      u_long   files;                /* files counter                */
      u_long   entry;                /* entry page counter           */
      u_long   exit;                 /* exit page counter            */

      u_long   vstref;               // visit references

      double   xfer;                 /* xfer size in bytes           */
      double   avgtime;				    // average processing time (sec)
      double   maxtime;              // maximum processing time (sec)


      public:
         typedef void (*s_unpack_cb_t)(unode_t& unode, void *arg);

      public:
         unode_t(u_long nodeid = 0);
         unode_t(const unode_t& unode);
         unode_t(const char *urlpath, const char *srchargs);

         void reset(u_long nodeid = 0);

         //
         // serialization
         //
         u_int s_data_size(void) const;
         u_int s_pack_data(void *buffer, u_int bufsize) const;
         u_int s_unpack_data(const void *buffer, u_int bufsize, s_unpack_cb_t upcb, void *arg);

         static u_int s_data_size(const void *buffer);

         static const void *s_field_value_hash(const void *buffer, u_int bufsize, u_int& datasize);
         static const void *s_field_xfer(const void *buffer, u_int bufsize, u_int& datasize);
         static const void *s_field_hits(const void *buffer, u_int bufsize, u_int& datasize);
         static const void *s_field_entry(const void *buffer, u_int bufsize, u_int& datasize);
         static const void *s_field_exit(const void *buffer, u_int bufsize, u_int& datasize);

         static int s_compare_xfer(const void *buf1, const void *buf2);
         static int s_compare_hits(const void *buf1, const void *buf2);
         static int s_compare_entry(const void *buf1, const void *buf2);
         static int s_compare_exit(const void *buf1, const void *buf2);
} *UNODEPTR;

//
// URLs
//
class u_hash_table : public hash_table<unode_t> {
   public:
      struct param_block {
         u_int type;
         const string_t *url;
         const string_t *srchargs;
      };

   private:
      virtual bool compare(const unode_t *nptr, const void *param) const;

   public:
      u_hash_table(void) : hash_table<unode_t>(LMAXHASH) {}
};

#endif // __UNODE_H
