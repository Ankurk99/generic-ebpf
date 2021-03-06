#include <gtest/gtest.h>

extern "C" {
#include <errno.h>
#include <stdint.h>
#include <sys/ebpf.h>

#include "../test_common.hpp"
}

namespace {
class HashTableMapLookupTest : public CommonFixture {
 protected:
  struct ebpf_map *em;

  virtual void SetUp() {
    int error;
    uint32_t gkey = 50, gval = 100;

    CommonFixture::SetUp();

    struct ebpf_map_attr attr;
    attr.type = EBPF_MAP_TYPE_HASHTABLE;
    attr.key_size = sizeof(uint32_t);
    attr.value_size = sizeof(uint32_t);
    attr.max_entries = 100;
    attr.flags = 0;

    error = ebpf_map_create(ee, &em, &attr);
    ASSERT_TRUE(!error);

    error = ebpf_map_update_elem_from_user(em, &gkey, &gval, 0);
    ASSERT_TRUE(!error);
  }

  virtual void TearDown() {
    ebpf_map_destroy(em);
    CommonFixture::TearDown();
  }
};

TEST_F(HashTableMapLookupTest, LookupUnexistingEntry) {
  int error;
  uint32_t key = 51;
  uint32_t value;

  error = ebpf_map_lookup_elem_from_user(em, &key, &value);

  EXPECT_EQ(ENOENT, error);
}

TEST_F(HashTableMapLookupTest, CorrectLookup) {
  int error;
  uint32_t key = 50;
  uint32_t value;

  error = ebpf_map_lookup_elem_from_user(em, &key, &value);

  EXPECT_EQ(0, error);
  EXPECT_EQ(100, value);
}
}  // namespace
