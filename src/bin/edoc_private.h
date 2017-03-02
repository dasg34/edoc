#ifndef EDOC_PRIVATE_H_
# define EDOC_PRIVATE_H_

#include <Elementary.h>
#include <clang-c/Index.h>
#include <clang-c/Documentation.h>

typedef struct _Edoc_Data Edoc_Data;

struct _Edoc_Data
{
   Evas_Object *win;
   Evas_Object *genlist;
   Evas_Object *search_bg;
   Evas_Object *search_box;
   Evas_Object *search_entry;

   Eina_Strbuf *title;
   Eina_Strbuf *detail;
   Eina_Strbuf *param;
   Eina_Strbuf *ret;
   Eina_Strbuf *see;

   CXIndex clang_idx;
   CXTranslationUnit clang_unit;
   CXCursor *cursors;
   Ecore_Thread *clang_thread;
};

void edoc_search_init(Edoc_Data *edoc);

void edoc_search_destroy(Edoc_Data *edoc);

void search_popup_setup(Edoc_Data *edoc);

void search_popup_update(Edoc_Data *edoc, char *word);

void search_lookup(Edoc_Data *edoc);

#endif
