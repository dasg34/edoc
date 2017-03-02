#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "edoc_private.h"

static void
_search_bg_cb_hide(void *data, Evas *e EINA_UNUSED,
                   Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Eina_List *list = NULL;
   Edoc_Data *edoc;

   edoc = (Edoc_Data *)data;
   list = (Eina_List *)evas_object_data_get(edoc->genlist, "list");

   if (list)
     {
        char *summary;

        EINA_LIST_FREE(list, summary)
          free(summary);

        list = NULL;
        evas_object_data_del(edoc->genlist, "list");
     }
   evas_object_key_ungrab(edoc->genlist, "Return", 0, 0);
   evas_object_key_ungrab(edoc->genlist, "Up", 0, 0);
   evas_object_key_ungrab(edoc->genlist, "Down", 0, 0);
}

static void
_search_list_cb_key_down(void *data, Evas *e EINA_UNUSED, Evas_Object *obj,
                         void *event_info)
{
   Edoc_Data *edoc = (Edoc_Data *)data;
   char *summary;
   Elm_Object_Item *it;
   Evas_Object *genlist = obj;
   Evas_Event_Key_Down *ev = event_info;

   if (!strcmp(ev->key, "Return"))
     {
        it = elm_genlist_selected_item_get(genlist);
        summary = elm_object_item_data_get(it);
//FIXMe : show doc
        evas_object_hide(edoc->search_bg);
     }
   else if (!strcmp(ev->key, "Up"))
     {
        it = elm_genlist_item_prev_get(elm_genlist_selected_item_get(genlist));
        if(!it) it = elm_genlist_last_item_get(genlist);

        elm_genlist_item_selected_set(it, EINA_TRUE);
        elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
   else if (!strcmp(ev->key, "Down"))
     {
        it = elm_genlist_item_next_get(elm_genlist_selected_item_get(genlist));
        if(!it) it = elm_genlist_first_item_get(genlist);

        elm_genlist_item_selected_set(it, EINA_TRUE);
        elm_genlist_item_show(it, ELM_GENLIST_ITEM_SCROLLTO_IN);
     }
}

static void
_search_list_cb_clicked_double(void *data, Evas_Object *obj EINA_UNUSED,
                               void *event_info)
{
   Elm_Object_Item *it = event_info;
   char *summary;
   Edoc_Data *edoc = (Edoc_Data *)data;

   summary = elm_object_item_data_get(it);
   //FIXMe : show doc
   //
   evas_object_hide(edoc->search_bg);
}

void
search_popup_setup(Edoc_Data *edoc)
{
   //Popup bg
   Evas_Object *bg = elm_bg_add(edoc->win);
   edoc->search_bg = bg;
   elm_bg_color_set(bg, 30, 30, 30);
   evas_object_event_callback_add(bg, EVAS_CALLBACK_HIDE, _search_bg_cb_hide,
                                  edoc);
   evas_object_resize(bg, 400 * elm_config_scale_get(),
                          300 * elm_config_scale_get());

   //Genlist
   Evas_Object *genlist = elm_genlist_add(bg);
   edoc->genlist = genlist;
   evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_focus_allow_set(genlist, EINA_FALSE);
   elm_genlist_homogeneous_set(genlist, EINA_TRUE);
   evas_object_event_callback_add(genlist, EVAS_CALLBACK_KEY_DOWN,
                                  _search_list_cb_key_down, edoc);
   evas_object_smart_callback_add(genlist, "clicked,double",
                                  _search_list_cb_clicked_double, edoc);
   elm_object_content_set(bg, genlist);

   //evas_object_smart_callback_add(genlist, "selected",
   //                               _suggest_list_cb_selected, label);
}

static char *
_search_list_text_get(void *data, Evas_Object *obj EINA_UNUSED, const char *part)
{
   char *summary = data;

   if (strcmp(part, "elm.text"))
     return NULL;

   return strdup(summary);
}

static void
_search_popup_show(Edoc_Data *edoc)
{
   Evas_Coord h;

   if (!edoc->genlist)
     return;

   if (elm_genlist_items_count(edoc->genlist) <= 0)
     return;

   if (evas_object_visible_get(edoc->search_bg))
     return;

   evas_object_geometry_get(edoc->search_box, NULL, NULL, NULL, &h);

   evas_object_move(edoc->search_bg, 0, h);
   evas_object_show(edoc->search_bg);

   if (!evas_object_key_grab(edoc->genlist, "Return", 0, 0, EINA_TRUE))
     EINA_LOG_ERR("Failed to grab key - %s", "Return");
   if (!evas_object_key_grab(edoc->genlist, "Up", 0, 0, EINA_TRUE))
     EINA_LOG_ERR("Failed to grab key - %s", "Up");
   if (!evas_object_key_grab(edoc->genlist, "Down", 0, 0, EINA_TRUE))
     EINA_LOG_ERR("Failed to grab key - %s", "Down");
}

static void
_thread_cb_lookup(void *data, Ecore_Thread *thread)
{
   Edoc_Data *edoc = data;
   Eina_List *list = NULL;
   CXCodeCompleteResults *res;
   char path[PATH_MAX];

   if (!edoc->clang_unit)
     return;

   snprintf(path, sizeof(path), "%s/edoc_tmp.c", eina_environment_tmp_get());

   res = clang_codeCompleteAt(edoc->clang_unit, path,
                              0, 0,
                              NULL, 0,
                              CXCodeComplete_IncludeMacros |
                              CXCodeComplete_IncludeCodePatterns);

   clang_sortCodeCompletionResults(res->Results, res->NumResults);

   for (unsigned int i = 0; i < res->NumResults; i++)
     {
        const CXCompletionString str = res->Results[i].CompletionString;
        const char *name = NULL;
        char *summary = NULL;

        if (ecore_thread_check(thread))
          {
             clang_disposeCodeCompleteResults(res);
             if (list)
               {
                  char *summary;

                  EINA_LIST_FREE(list, summary)
                    free(summary);
               }
             ecore_thread_cancel(thread);
             return;
          }

        for (unsigned int j = 0; j < clang_getNumCompletionChunks(str); j++)
          {
             enum CXCompletionChunkKind ch_kind;
             const CXString str_out = clang_getCompletionChunkText(str, j);

             ch_kind = clang_getCompletionChunkKind(str, j);

             switch (ch_kind)
               {
                case CXCompletionChunk_TypedText:
                case CXCompletionChunk_Text:
                   name = clang_getCString(str_out);
                   break;
                default:
                   break;
               }
          }

        if (name)
          summary = strdup(name);

        list = eina_list_append(list, summary);
     }
   clang_disposeCodeCompleteResults(res);

   ecore_thread_main_loop_begin();
   evas_object_data_set(edoc->genlist, "list", list);
   ecore_thread_main_loop_end();
   return;
}

static void
_thread_cb_end(void *data, Ecore_Thread *thread EINA_UNUSED)
{
   Edoc_Data *edoc;
   int pos;
   char *curword;

   edoc = (Edoc_Data *)data;

   pos = elm_entry_cursor_pos_get(edoc->search_entry);
   curword = strdup(elm_entry_entry_get(edoc->search_entry));
   curword[pos] = '\0';

   search_popup_update(edoc, curword);
   free(curword);

   edoc->clang_thread = NULL;
}

static void
_thread_cb_cancel(void *data, Ecore_Thread *thread EINA_UNUSED)
{
   Edoc_Data *edoc;

   edoc = (Edoc_Data *)data;

   edoc->clang_thread = NULL;
}

void
search_lookup(Edoc_Data *edoc)
{
   if (edoc->clang_thread)
     {
        ecore_thread_cancel(edoc->clang_thread);
        edoc->clang_thread = NULL;
     }

   edoc->clang_thread = ecore_thread_run(_thread_cb_lookup, _thread_cb_end, _thread_cb_cancel, edoc);
}

void
search_popup_update(Edoc_Data *edoc, char *word)
{
   char *summary;
   Eina_List *list, *l;
   Elm_Genlist_Item_Class *ic;
   Elm_Object_Item *item;

   elm_genlist_clear(edoc->genlist);

   list = (Eina_List *)evas_object_data_get(edoc->genlist, "list");
   ic = elm_genlist_item_class_new();
   ic->item_style = "default";
   ic->func.text_get = _search_list_text_get;

   EINA_LIST_FOREACH(list, l, summary)
     {
        if (eina_str_has_prefix(summary, word))
          {
             elm_genlist_item_append(edoc->genlist,
                                     ic,
                                     summary,
                                     NULL,
                                     ELM_GENLIST_ITEM_NONE,
                                     NULL,
                                     NULL);
          }
     }
   elm_genlist_item_class_free(ic);

   item = elm_genlist_first_item_get(edoc->genlist);
   if (item)
     {
        elm_genlist_item_selected_set(item, EINA_TRUE);
        elm_genlist_item_show(item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
        _search_popup_show(edoc);
     }
   else
     evas_object_hide(edoc->search_bg);
}

void
edoc_search_init(Edoc_Data *edoc)
{
   char path[PATH_MAX];
   const char *args;
   char **clang_argv;
   unsigned int clang_argc;

   snprintf(path, sizeof(path), "%s/edoc_tmp.c", eina_environment_tmp_get());

   args = "-I/usr/inclue/ " EFL_CFLAGS " -Wall -Wextra -DEFL_BETA_API_SUPPORT";
   clang_argv = eina_str_split_full(args, " ", 0, &clang_argc);

   edoc->clang_idx = clang_createIndex(0, 0);
   edoc->clang_unit = clang_parseTranslationUnit(edoc->clang_idx, path,
                                  (const char *const *)clang_argv,
                                  (int)clang_argc, NULL, 0,
                                  clang_defaultEditingTranslationUnitOptions() |
                                  CXTranslationUnit_DetailedPreprocessingRecord);

   edoc->title = eina_strbuf_new();
   edoc->detail = eina_strbuf_new();
   edoc->param = eina_strbuf_new();
   edoc->ret = eina_strbuf_new();
   edoc->see = eina_strbuf_new();
}

void
edoc_search_destroy(Edoc_Data *edoc)
{
   clang_disposeTranslationUnit(edoc->clang_unit);
   clang_disposeIndex(edoc->clang_idx);
}
