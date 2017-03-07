#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "edoc_private.h"

static void
edoc_doc_init(Edoc_Data *edoc)
{
   edoc->title = eina_strbuf_new();
   edoc->detail = eina_strbuf_new();
   edoc->param = eina_strbuf_new();
   edoc->ret = eina_strbuf_new();
   edoc->see = eina_strbuf_new();
}

static void
_edoc_doc_free(Edoc_Data *edoc)
{
   if (!edoc) return;

   eina_strbuf_free(edoc->title);
   eina_strbuf_free(edoc->detail);
   eina_strbuf_free(edoc->param);
   eina_strbuf_free(edoc->ret);
   eina_strbuf_free(edoc->see);
}

static Eina_Bool
edoc_doc_newline_check(Eina_Strbuf *strbuf)
{
   const char *str;

   str = eina_strbuf_string_get(strbuf);

   if (strlen(str) < 4)
     return EINA_TRUE;

   str = str + strlen(str) - 4;

   if (!strcmp(str, "<br>"))
     return EINA_FALSE;
   else
     return EINA_TRUE;
}

static void
edoc_doc_trim(Eina_Strbuf *strbuf)
{
   const char *str;
   int cmp_strlen, ori_strlen;

   str = eina_strbuf_string_get(strbuf);
   ori_strlen = strlen(str);

   if (strlen(str) < 8)
     return;

   cmp_strlen = strlen(str) - 8;
   str += cmp_strlen;

   if (!strcmp(str, "<br><br>"))
     {
        eina_strbuf_remove(strbuf, cmp_strlen, ori_strlen);
        edoc_doc_trim(strbuf);
     }
   else
     return;
}

static void
edoc_doc_title_get(CXCursor cursor, Eina_Strbuf *strbuf)
{
   CXCompletionString str;
   int chunk_num;

   str = clang_getCursorCompletionString(cursor);
   chunk_num = clang_getNumCompletionChunks(str);

   for (int i = 0; i < chunk_num; i++)
     {
        enum CXCompletionChunkKind kind = clang_getCompletionChunkKind(str, i);
        switch (kind)
          {
           case CXCompletionChunk_ResultType:
             eina_strbuf_append_printf(strbuf, "<color=#31d12f><b>%s</b></color><br>",
                        clang_getCString(clang_getCompletionChunkText(str, i)));
             break;
           case CXCompletionChunk_Placeholder:
             eina_strbuf_append_printf(strbuf, "<color=#edd400><b>%s</b></color>",
                        clang_getCString(clang_getCompletionChunkText(str, i)));
             break;
           default:
             eina_strbuf_append(strbuf,
                        clang_getCString(clang_getCompletionChunkText(str, i)));
             break;
          }
     }
}

static void
edoc_doc_dump(Edoc_Data *edoc, CXComment comment, Eina_Strbuf *strbuf)
{
   const char *str ,*tag;
   enum CXCommentKind kind = clang_Comment_getKind(comment);

   if (kind == CXComment_Null) return;

   switch (kind)
     {
      case CXComment_Text:
        str = clang_getCString(clang_TextComment_getText(comment));

        if (edoc->see == strbuf)
          {
             eina_strbuf_append_printf(strbuf, "   %s", str);
             break;
          }
        if (clang_Comment_isWhitespace(comment))
          {
             if (edoc_doc_newline_check(strbuf))
               {
                  if (strbuf == edoc->detail)
                    eina_strbuf_append(strbuf, "<br><br>");
                  else
                    eina_strbuf_append(strbuf, "<br>");
               }
             break;
          }
        eina_strbuf_append(strbuf, str);
        break;
      case CXComment_InlineCommand:
        str = clang_getCString(clang_InlineCommandComment_getCommandName(comment));

        if (str[0] == 'p')
          eina_strbuf_append_printf(strbuf, "<font_style=italic>%s</font_style>",
             clang_getCString(clang_InlineCommandComment_getArgText(comment, 0)));
        else if (str[0] == 'c')
          eina_strbuf_append_printf(strbuf, "<b>%s</b>",
             clang_getCString(clang_InlineCommandComment_getArgText(comment, 0)));
        else
          eina_strbuf_append_printf(strbuf, "@%s", str);
        break;
      case CXComment_BlockCommand:
        tag = clang_getCString(clang_BlockCommandComment_getCommandName(comment));

        if (!strcmp(tag, "return"))
          strbuf = edoc->ret;
        else if (!strcmp(tag, "see"))
          strbuf = edoc->see;

        break;
      case CXComment_ParamCommand:
        str = clang_getCString(clang_ParamCommandComment_getParamName(comment));
        strbuf = edoc->param;

        eina_strbuf_append_printf(strbuf, "<color=#edd400><b>   %s</b></color>",
                                  str);
        break;
      case CXComment_VerbatimBlockLine:
        str = clang_getCString(clang_VerbatimBlockLineComment_getText(comment));

        if (str[0] == 10)
          {
             eina_strbuf_append(strbuf, "<br>");
             break;
          }
        eina_strbuf_append_printf(strbuf, "%s<br>", str);
        break;
      case CXComment_VerbatimLine:
        str = clang_getCString(clang_VerbatimLineComment_getText(comment));

        if (edoc->see == strbuf)
          eina_strbuf_append(strbuf, str);
        break;
      default:
        break;
     }
   for (unsigned i = 0; i < clang_Comment_getNumChildren(comment); i++)
     edoc_doc_dump(edoc, clang_Comment_getChild(comment, i), strbuf);
}

static CXCursor
_edoc_doc_cursor_get(Edoc_Data *edoc)
{
   CXFile cxfile;
   CXSourceLocation location;
   CXCursor cursor;
   char path[PATH_MAX];

   snprintf(path, sizeof(path), "%s/edoc_tmp.c", eina_environment_tmp_get());

   cxfile = clang_getFile(edoc->clang_unit, path);
   location = clang_getLocation(edoc->clang_unit, cxfile, 2, 12);
   cursor = clang_getCursor(edoc->clang_unit, location);

   return clang_getCursorReferenced(cursor);
}

void
edoc_doc_lookup(Edoc_Data *edoc, char *summary)
{
   CXCursor cursor;
   CXComment comment;
   struct CXUnsavedFile unsaved_file;
   char path[PATH_MAX], buf[1024];
   int len;
   FILE *fp;

   snprintf(path, sizeof(path), "%s/edoc_tmp.c", eina_environment_tmp_get());
   fp = fopen(path, "r");
   len = fread(buf, 1, 1024, fp);
   buf[len] = '\0';
   fclose(fp);

   strcat(buf, summary);

   unsaved_file.Filename = path;
   unsaved_file.Contents = buf;
   unsaved_file.Length = strlen(unsaved_file.Contents);

   clang_reparseTranslationUnit(edoc->clang_unit, 1, &unsaved_file,
                                clang_defaultReparseOptions(edoc->clang_idx));
   cursor = _edoc_doc_cursor_get(edoc);
   comment = clang_Cursor_getParsedComment(cursor);

   if (clang_Comment_getKind(comment) == CXComment_Null)
     return;

   _edoc_doc_free(edoc);
   edoc_doc_init(edoc);
   edoc_doc_dump(edoc, comment, edoc->detail);
   edoc_doc_title_get(cursor, edoc->title);
   edoc_doc_trim(edoc->detail);

   elm_object_text_set(edoc->title_entry, eina_strbuf_string_get(edoc->title));
   elm_object_text_set(edoc->body_entry, eina_strbuf_string_get(edoc->detail));
}

