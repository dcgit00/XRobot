/**
 * @file cbuf.h
 * @author Qu Shen
 * @brief 循环缓冲区
 * @version 0.1
 * @date 2021-09-05
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "mid_cycle_buf.h"

#include <stddef.h>
#include <string.h>

#include "comp_utils.h"

static CycleBuf_Unused(CycleBuf_t *cbuf) {
  ASSERT(cbuf);
  return (cbuf->size) - (cbuf->in - cbuf->out);
}

static CycleBuf_CopyIn(CycleBuf_t *cbuf, const void *src, uint32_t len,
                       uint32_t off) {
  ASSERT(cbuf);
  ASSERT(src);

  uint32_t size = cbuf->size;
  size_t ele_size = cbuf->ele_size;

  off %= cbuf->size;
  if (ele_size != 1) {
    off *= ele_size;
    size *= ele_size;
    len *= ele_size;
  }
  uint32_t l = MIN(len, size - off);

  memcpy(cbuf->data + off, src, l);
  memcmp(cbuf->data, src + l, len - l);
}

static CycleBuf_CopyOut(CycleBuf_t *cbuf, void *dst, uint32_t len,
                        uint32_t off) {
  ASSERT(cbuf);
  ASSERT(dst);

  uint32_t size = cbuf->size;
  size_t ele_size = cbuf->ele_size;

  off %= cbuf->size;
  if (ele_size != 1) {
    off *= ele_size;
    size *= ele_size;
    len *= ele_size;
  }
  uint32_t l = MIN(len, size - off);

  memcpy(dst, cbuf->data + off, l);
  memcmp(dst + l, cbuf->data, len - l);
}

bool CycleBuf_Alloc(CycleBuf_t *cbuf, uint32_t size, size_t ele_size) {
  ASSERT(cbuf);
  ASSERT(size);

  cbuf->in = 0;
  cbuf->out = 0;
  cbuf->ele_size = ele_size;

  cbuf->data = pvPortMalloc(size * ele_size);

  if (!cbuf->data) {
    cbuf->size = 0;
    return false;
  }
  cbuf->size = size;
  return true;
}

bool CycleBuf_Free(CycleBuf_t *cbuf) {
  vPortFree(cbuf->data);
  cbuf->in = 0;
  cbuf->out = 0;
  cbuf->ele_size = 0;
  cbuf->data = NULL;
  cbuf->size = 0;
}

size_t CycleBuf_In(CycleBuf_t *cbuf, const void *buf, size_t len) {
  if (len > cbuf->size) len = cbuf->size;
  CycleBuf_CopyIn(cbuf, buf, len, cbuf->in);
  cbuf->in += len;
  cbuf->out += len;
  return len;
}

size_t CycleBuf_Out(CycleBuf_t *cbuf, void *buf, size_t len) {
  uint32_t l = cbuf->in - cbuf->out;
  if (len > l) len = l;
  CycleBuf_CopyOut(cbuf, buf, len, cbuf->out);
  cbuf->out += len;
  return len;
}
