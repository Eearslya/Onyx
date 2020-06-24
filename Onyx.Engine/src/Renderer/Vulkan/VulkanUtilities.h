#pragma once

#define VK_CHECK(vkcall) \
  { ASSERT(vkcall == VK_SUCCESS); }