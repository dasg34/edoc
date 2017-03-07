#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* NOTE: Respecting header order is important for portability.
 * Always put system first, then EFL, then your public header,
 * and finally your private one. */

#if ENABLE_NLS
# include <libintl.h>
#endif

#include <Ecore_Getopt.h>
#include <Elementary.h>

#include "edoc.h"

#include "edoc_private.h"

#define COPYRIGHT "Copyright © 2017 yohoho <cleanlyj@gmail.com> and various contributors (see AUTHORS)."

static void
_temp_file_set()
{
   char path[PATH_MAX];
   const char *buf = "#include <Elementary.h> \nint main(){";
   FILE *fp;

   snprintf(path, sizeof(path), "%s/edoc_tmp.c", eina_environment_tmp_get());
   fp = fopen(path, "w");

   fwrite(buf, sizeof(char), strlen(buf), fp);
   fclose(fp);
}

static void
_edoc_win_del(void *data, Evas_Object *obj EINA_UNUSED,
              void *event_info EINA_UNUSED)
{
   Edoc_Data *edoc = data;
   search_destroy(edoc);
   free(edoc);

   elm_exit();
}

void
_edoc_search_lookup(Edoc_Data *edoc)
{
   Eina_List *list = NULL;

   list = (Eina_List *)evas_object_data_get(edoc->genlist, "list");

   if (list)
     {
        char *summary;

        EINA_LIST_FREE(list, summary)
          free(summary);

        list = NULL;
        evas_object_data_del(edoc->genlist, "list");
     }

   search_lookup(edoc);
}

static void
_entry_cb_cursor_changed(void *data, Evas_Object *obj EINA_UNUSED,
                         void *event_info EINA_UNUSED)
{
   Edoc_Data *edoc;

   edoc = (Edoc_Data*)data;
   _edoc_search_lookup(edoc);
}

static void
_entry_cb_clicked(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED,
                  void *event_info EINA_UNUSED)
{
   Edoc_Data *edoc;

   edoc = (Edoc_Data*)data;
   _edoc_search_lookup(edoc);
}

static void
_search_box_set(Edoc_Data *edoc, Evas_Object *box)
{
   Evas_Object *label, *entry, *icon, *btn;
   Evas_Coord h;

   label = elm_label_add(box);
   elm_object_text_set(label, "API : ");
   evas_object_size_hint_weight_set(label, 0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(label);
   elm_box_pack_end(box, label);

   entry = elm_entry_add(box);
   edoc->search_entry = entry;
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_entry_single_line_set(entry, EINA_TRUE);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(entry, "cursor,changed",
                                  _entry_cb_cursor_changed, edoc);
   evas_object_smart_callback_add(entry, "clicked", _entry_cb_clicked, edoc);
   evas_object_show(entry);
   elm_box_pack_end(box, entry);

   icon = elm_icon_add(box);
   evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_recalculate(box);
   evas_object_geometry_get(entry, NULL, NULL, NULL, &h);
   evas_object_size_hint_min_set(icon, h, h);
   elm_icon_standard_set(icon, "edit-find");

   btn = elm_button_add(box);
   elm_object_part_content_set(btn, "icon", icon);
   //evas_object_smart_callback_add(btn, "clicked", _btn_cb_clicked, NULL);
   evas_object_show(btn);
   elm_box_pack_end(box, btn);
}

static void
_document_box_set(Edoc_Data *edoc, Evas_Object *box)
{
   //Title
   Evas_Object *title = elm_entry_add(box);
   edoc->title_entry = title;
   elm_entry_editable_set(title, EINA_FALSE);
   evas_object_size_hint_weight_set(title, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(title, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(title);
   elm_box_pack_end(box, title);

   //Entry
   Evas_Object *entry = elm_entry_add(box);
   edoc->body_entry = entry;
   elm_entry_editable_set(entry, EINA_FALSE);
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_scroller_policy_set(entry, ELM_SCROLLER_POLICY_OFF,
                           ELM_SCROLLER_POLICY_AUTO);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(entry);
   elm_box_pack_end(box, entry);
}

static Evas_Object *
edoc_win_setup(void)
{
   Evas_Object *win, *search_box, *main_box, *doc_box;
   Edoc_Data *edoc;

   win = elm_win_util_standard_add("main", "edoc");
   if (!win) return NULL;

   _temp_file_set();

   edoc = (Edoc_Data *)calloc(1, sizeof(Edoc_Data));
   edoc->win = win;
   search_init(edoc);

   elm_win_focus_highlight_enabled_set(win, EINA_FALSE);
   evas_object_smart_callback_add(win, "delete,request", _edoc_win_del, edoc);

   /* main box */
   main_box = elm_box_add(win);
   evas_object_size_hint_weight_set(main_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(main_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, main_box);
   evas_object_show(main_box);

   /* search box */
   search_box = elm_box_add(main_box);
   edoc->search_box = search_box;
   elm_box_horizontal_set(search_box, EINA_TRUE);
   evas_object_size_hint_weight_set(search_box, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(search_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(search_box);
   elm_box_pack_end(main_box, search_box);
   _search_box_set(edoc, search_box);

   /* document box */
   doc_box = elm_box_add(main_box);
   evas_object_size_hint_weight_set(doc_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(doc_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(doc_box);
   elm_box_pack_end(main_box, doc_box);
   _document_box_set(edoc, doc_box);

   search_popup_setup(edoc);

   evas_object_resize(win, 400 * elm_config_scale_get(),
                           500 * elm_config_scale_get());
   evas_object_show(win);

   return win;
}

static const Ecore_Getopt optdesc = {
  "edoc",
  "%prog [options]",
  PACKAGE_VERSION,
  COPYRIGHT,
  "3 clause BSD license",
  "An EFL edoc program",
  0,
  {
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{
   Evas_Object *win;
   int args;
   Eina_Bool quit_option = EINA_FALSE;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   textdomain(PACKAGE);
#endif

   edoc_init();

   args = ecore_getopt_parse(&optdesc, values, argc, argv);
   if (args < 0)
     {
	EINA_LOG_CRIT("Could not parse arguments.");
	goto end;
     }
   else if (quit_option)
     {
	goto end;
     }

   elm_app_info_set(elm_main, "edoc", "images/edoc.png");

   if (!(win = edoc_win_setup()))
     goto end;

   edoc_library_call();

   elm_run();

 end:
   edoc_shutdown();
   elm_shutdown();

   return 0;
}
ELM_MAIN()
