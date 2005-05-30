#ifndef _QDP_STRING
#define _QDP_STRING

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *string;
  size_t length;
} QDP_String;

QDP_String *QDP_string_create(void);
void QDP_string_destroy(QDP_String *qs);
void QDP_string_set(QDP_String *qs, char *string);
void QDP_string_copy(QDP_String *dest, QDP_String *src);
size_t QDP_string_bytes(QDP_String *qs);
char * QDP_string_ptr(QDP_String *qs);

#ifdef __cplusplus
}
#endif

#endif
