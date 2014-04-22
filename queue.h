/*
   webalizer - a web server log analysis program

   Copyright (c) 2004-2013, Stone Steps Inc. (www.stonesteps.ca)

   See COPYING and Copyright files for additional licensing and copyright information     
   
   queue.h
*/
#ifndef __QUEUE_H
#define __QUEUE_H

#include "types.h"
#include <stddef.h>

// -----------------------------------------------------------------------
//
// queue_t
//
// -----------------------------------------------------------------------
template <typename type_t>
class queue_t {
   private:
      static const u_int max_ecount;

   private:
      struct q_node_t {
         q_node_t    *next;
         type_t      *data;

         public:
            q_node_t(type_t *data) : data(data), next(NULL) {}

            q_node_t *reset(type_t *_data) {next = NULL; data = _data; return this;}
      };

   private:
      q_node_t    *head;            // first node
      q_node_t    *tail;            // last node
      u_int       count;            // node count

      q_node_t    *empty;           // empty node list
      u_int       ecount;           // empty node count

   private:
      q_node_t *new_node(type_t *data);
      void delete_node(q_node_t *node);

   public:
      queue_t(void);

      ~queue_t(void);

      inline u_int size(void) const {return count;}

      inline const type_t *top(void) const {return head ? head->data : NULL;}

      inline type_t *top(void) {return head ? head->data : NULL;}

      void add(type_t *data);

      type_t *remove(void);
};

#endif // __QUEUE_H
