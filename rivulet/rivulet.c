// vim:set sw=4 ts=4 sts=4 ai et:

/*
 * riv·u·let, noun: a very small stream.
 *
 * Copyright (c) 2016  Michael Toren <mct@toren.net>
 * Released under the MIT license.
 *
 * A simple method of managing a byte buffer, with minimal accounting
 * overhead.  Provides a mechanism for reading and writing 8, 16, and 32
 * bit words from a buffer in such a way that the bytes do not need to
 * be properly word aligned.  All values are big-endian.  If you attempt
 * to read or write more data than is available in the buffer, an error
 * flag is recorded in the struct, but it will not result in a buffer
 * overflow.
 *
 * You aren't required to, but if you find this useful, I'd love to hear
 * about it.
 */

#include <stdlib.h>
#include <stdint.h>

#include "rivulet.h"

/*
 * Initializes @r, using @buf of length @len as the buffer backing.
 * No allocations are performed.
 */

void rivulet_init(struct rivulet *r, u8 *buf, size_t len)
{
    if (!buf)
        len = 0;

    r->buf = buf;
    r->len = len;
    r->pos = 0;
    r->overruns = 0;
    r->eof = len? 0 : 1;
}

/*
 * Rewinding does not restore bytes that were popped off the tail
 */

void rivulet_rewind(struct rivulet *r)
{
    rivulet_init(r, r->buf, r->len);
}

size_t rivulet_bytes_remaining(struct rivulet *r)
{
    return r->len - r->pos;
}

/*
 * For all of the read functions:
 *
 * Reads the specified word size (u8, u16, or u32) from the buffer in
 * big-endian order and returns the result.  If not enough buffer space
 * is available to satisfy the entire request, an undefined value is
 * returned, and @overruns is incremented for each byte that could not
 * be read.  @eof is always updated.
 */

u8 rivulet_read_u8(struct rivulet *r)
{
    if (r->pos < r->len)
    {
        u8 ret = r->buf[r->pos++];
        if (r->pos == r->len)
            r->eof = 1;
        return ret;
    }

    else
    {
        r->overruns++;
        return 0;
    }
}

u16 rivulet_read_u16(struct rivulet *r)
{
    u16 ret = 0;
    ret |= (u16) rivulet_read_u8(r) << 8;
    ret |= (u16) rivulet_read_u8(r) << 0;
    return ret;
}

u32 rivulet_read_u32(struct rivulet *r)
{
    u32 ret = 0;
    ret |= (u32) rivulet_read_u8(r) << 24;
    ret |= (u32) rivulet_read_u8(r) << 16;
    ret |= (u32) rivulet_read_u8(r) <<  8;
    ret |= (u32) rivulet_read_u8(r) <<  0;
    return ret;
}


/*
 * For all of the write functions:
 *
 * Writes the specified word size (u8, u16, or u32) to the buffer in
 * big-endian order and returns the number of bytes written.  If not
 * enough buffer space is available, @overruns is incremented for each
 * byte that could not be written.  @eof is always updated.
 */

size_t rivulet_write_u8(struct rivulet *r, u8 val)
{
    if (r->pos < r->len)
    {
        r->buf[r->pos++] = val;
        if (r->pos == r->len)
            r->eof = 1;
        return 1;
    }

    else
    {
        r->overruns++;
        return 0;
    }
}

size_t rivulet_write_u16(struct rivulet *r, u16 val)
{
    size_t ret = 0;
    ret += rivulet_write_u8(r, (val >> 8) & 0xff);
    ret += rivulet_write_u8(r, (val >> 0) & 0xff);
    return ret;
}

size_t rivulet_write_u32(struct rivulet *r, u32 val)
{
    size_t ret = 0;
    ret += rivulet_write_u8(r, (val >> 24) & 0xff);
    ret += rivulet_write_u8(r, (val >> 16) & 0xff);
    ret += rivulet_write_u8(r, (val >>  8) & 0xff);
    ret += rivulet_write_u8(r, (val >>  0) & 0xff);
    return ret;
}

/*
 * For all of the unread functions:
 *
 * Rewinds the current buffer position by the specified word size (u8,
 * u16, or u32).  It is safe to attempt to rewind past the beginning of
 * the buffer, but no error will be indicated.  @eof is always updated.
 */

void rivulet_unread_u8(struct rivulet *r)
{
    if (r->pos)
        r->pos--;
    r->eof = (r->pos < r->len) ? 0 : 1;
}

void rivulet_unread_u16(struct rivulet *r) {
    rivulet_unread_u8(r);
    rivulet_unread_u8(r);
}

void rivulet_unread_u32(struct rivulet *r) {
    rivulet_unread_u16(r);
    rivulet_unread_u16(r);
}

/*
 * The pop functions are similar to the read functions, but bytes are
 * consumed from the end of the buffer, not the current buffer position.
 * Because this modifies @len, bytes popped will not be restored with
 * rivulet_rewind()
 */

u8 rivulet_pop_u8(struct rivulet *r)
{
    if (r->pos < r->len)
    {
        u8 ret = r->buf[--r->len];
        if (r->pos == r->len)
            r->eof = 1;
        return ret;
    }

    else
    {
        r->overruns++;
        return 0;
    }
}

u16 rivulet_pop_u16(struct rivulet *r)
{
    u16 ret = 0;
    ret |= (u16) rivulet_pop_u8(r) << 8;
    ret |= (u16) rivulet_pop_u8(r) << 0;
    return ret;
}

u32 rivulet_pop_u32(struct rivulet *r)
{
    u32 ret = 0;
    ret |= (u32) rivulet_pop_u8(r) << 24;
    ret |= (u32) rivulet_pop_u8(r) << 16;
    ret |= (u32) rivulet_pop_u8(r) <<  8;
    ret |= (u32) rivulet_pop_u8(r) <<  0;
    return ret;
}
