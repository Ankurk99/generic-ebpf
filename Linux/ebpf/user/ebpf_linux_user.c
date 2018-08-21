/*-
 * SPDX-License-Identifier: Apache License 2.0
 *
 * Copyright 2017-2018 Yutaro Hayakawa
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dev/ebpf/ebpf_platform.h>
#include <dev/ebpf/ebpf_map.h>
#include <dev/ebpf/ebpf_prog.h>
#include <dev/ebpf/ebpf_jhash.h>
#include <sys/ebpf.h>

void *
ebpf_malloc(size_t size)
{
	return malloc(size);
}

void *
ebpf_calloc(size_t number, size_t size)
{
	return calloc(number, size);
}

void *
ebpf_exalloc(size_t size)
{
	void *ret = NULL;

	ret = mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC,
		   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ret == MAP_FAILED) {
		fprintf(stderr, "mmap in ebpf_exalloc failed\n");
		return NULL;
	}

	return ret;
}

void
ebpf_exfree(void *mem, size_t size)
{
	munmap(mem, size);
}

void
ebpf_free(void *mem)
{
	free(mem);
}

int
ebpf_error(const char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vfprintf(stderr, fmt, ap);
	va_end(ap);

	return ret;
}

void
ebpf_assert(bool expr)
{
	assert(expr);
}

uint16_t
ebpf_ncpus(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

uint16_t
ebpf_curcpu(void)
{
	return 0; // This makes no sense. Just for testing.
}

long
ebpf_getpagesize(void)
{
	return sysconf(_SC_PAGESIZE);
}

void
ebpf_rw_init(ebpf_rwlock_t *rw, char *name)
{
	int error = pthread_rwlock_init(rw, NULL);
	assert(!error);
}

void
ebpf_rw_rlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_rdlock(rw);
	assert(!error);
}

void
ebpf_rw_runlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_unlock(rw);
	assert(!error);
}

void
ebpf_rw_wlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_wrlock(rw);
	assert(!error);
}

void
ebpf_rw_wunlock(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_unlock(rw);
	assert(!error);
}

void
ebpf_rw_destroy(ebpf_rwlock_t *rw)
{
	int error = pthread_rwlock_destroy(rw);
	assert(!error);
}

void
ebpf_refcount_init(uint32_t *count, uint32_t value)
{
	*count = value;
}

void
ebpf_refcount_acquire(uint32_t *count)
{
	ebpf_assert(*count < UINT32_MAX);
	ck_pr_inc_32(count);
}

int
ebpf_refcount_release(uint32_t *count)
{
	uint32_t old;

	old = ck_pr_faa_32(count, -1);
	ebpf_assert(old > 0);

	if (old > 1) {
		return 0;
	}

	return 1;
}

void
ebpf_mtx_init(ebpf_mtx_t *mutex, const char *name)
{
	int error = pthread_mutex_init(mutex, NULL);
	assert(!error);
}

void
ebpf_mtx_lock(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_lock(mutex);
	assert(!error);
}

void
ebpf_mtx_unlock(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_unlock(mutex);
	assert(!error);
}

void
ebpf_mtx_destroy(ebpf_mtx_t *mutex)
{
	int error = pthread_mutex_destroy(mutex);
	assert(!error);
}

void
ebpf_spinmtx_init(ebpf_spinmtx_t *mutex, const char *name)
{
	int error = pthread_spin_init(mutex, 0);
	assert(!error);
}

void
ebpf_spinmtx_lock(ebpf_spinmtx_t *mutex)
{
	int error = pthread_spin_lock(mutex);
	assert(!error);
}

void
ebpf_spinmtx_unlock(ebpf_spinmtx_t *mutex)
{
	int error = pthread_spin_unlock(mutex);
	assert(!error);
}

void
ebpf_spinmtx_destroy(ebpf_spinmtx_t *mutex)
{
	int error = pthread_spin_destroy(mutex);
	assert(!error);
}

uint32_t
ebpf_jenkins_hash(const void *buf, size_t len, uint32_t hash)
{
  return jenkins_hash(buf, len, hash);
}

__attribute__((constructor)) void
ebpf_init(void)
{
	ebpf_init_map_types();
	ebpf_init_prog_types();
}
