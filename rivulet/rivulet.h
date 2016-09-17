// vim:set sw=4 ts=4 sts=4 ai et:

#ifndef RIVULET_H
#define RIVULET_H

typedef  uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

/*
 * @buf: Pointer to the buffer backing this rivulet
 * @len: The allocated size of @buf
 * @pos: Our current read/write position into @buf
 * @overruns: Number of bytes attempted to be read or written past the buffer
 * @eof: Set if @pos is at the end of the buffer.  Nothing left to read, and
 *       no room left to write.
 */
struct rivulet {
    u8 *buf;
    size_t len;
    size_t pos;
    size_t overruns;
    int eof;
};

void   rivulet_init            (struct rivulet *r, u8 *buf, size_t len);
void   rivulet_rewind          (struct rivulet *r);
size_t rivulet_bytes_remaining (struct rivulet *r);

u8  rivulet_read_u8  (struct rivulet *r);
u16 rivulet_read_u16 (struct rivulet *r);
u32 rivulet_read_u32 (struct rivulet *r);

size_t rivulet_write_u8  (struct rivulet *r, u8 val);
size_t rivulet_write_u16 (struct rivulet *r, u16 val);
size_t rivulet_write_u32 (struct rivulet *r, u32 val);

void rivulet_unread_u8  (struct rivulet *r);
void rivulet_unread_u16 (struct rivulet *r);
void rivulet_unread_u32 (struct rivulet *r);

u8  rivulet_pop_u8  (struct rivulet *r);
u16 rivulet_pop_u16 (struct rivulet *r);
u32 rivulet_pop_u32 (struct rivulet *r);

#endif
