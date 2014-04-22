/*
    webalizer - a web server log analysis program

    Copyright (c) 2004-2013, Stone Steps Inc. (www.stonesteps.ca)

    See COPYING and Copyright files for additional licensing and copyright information

    history.h
*/

#ifndef __HISTORY_H
#define __HISTORY_H

#include "vector.h"
#include "types.h"
#include "config.h"

//
// hist_month_t
//
struct hist_month_t {
   u_int   year;           // year (4 digits)
   u_int   month;          // month number (one-based)

   int     fday;           // first day
   int     lday;           // last day

   u_long  hits; 
   u_long  files;
   u_long  hosts;
   double  xfer; 
   u_long  pages;
   u_long  visits;

   public:
      hist_month_t(void);

      hist_month_t(u_int year, u_int month, u_long hits, u_long files, u_long pages, u_long visits, u_long hosts, double xfer, u_int fday, u_int lday);
};

//
// history_t
//
class history_t {
   public:
      typedef vector_t<hist_month_t>::const_iterator const_iterator;
      typedef vector_t<hist_month_t>::const_reverse_iterator const_reverse_iterator;

   private:
      const config_t&         config;
      vector_t<hist_month_t>  months;
      u_int                   max_length;

   public:
      history_t(const config_t& _config);

      ~history_t(void);

      void initialize(void);

      void cleanup(void);

      const_iterator begin(void) const {return months.begin();}

      const_reverse_iterator rbegin(void) const {return months.rbegin();}

      void set_max_length(u_int len) {max_length = (len > 12) ? len : 12;}

      u_int get_max_length(void) const {return max_length;}

      u_int length(void) const;

      u_int disp_length(void) const;

      void clear(void) {months.clear(); max_length = 0;}

      u_int size(void) const {return months.size();}

      const hist_month_t *first(void) const {return months.size() ? &months[0] : NULL;}

      const hist_month_t *last(void) const {return months.size() ? &months[months.size()-1] : NULL;}

      int month_index(u_int year, u_int month) const;

      u_int first_month(void) const;
                       
      const hist_month_t *find_month(u_int year, u_int month) const;

      bool update(const hist_month_t *month);

      bool update(u_int year, u_int month, u_long hits, u_long files, u_long pages, u_long visits, u_long hosts, double xfer, u_int fday, u_int lday);

      bool get_history(void);

      void put_history(void);
};

#endif // __HISTORY_H

