#ifndef __GSGPU_TTM_H__
#define __GSGPU_TTM_H__

#include "gsgpu.h"
#include <drm/gpu_scheduler.h>

#define GSGPU_GTT_MAX_TRANSFER_SIZE	512
#define GSGPU_GTT_NUM_TRANSFER_WINDOWS	2

struct gsgpu_gtt_mgr {
        struct ttm_resource_manager manager;
        struct drm_mm mm;
        spinlock_t lock;
};

struct gsgpu_mman {
	struct ttm_device		bdev;
	bool				initialized;
	void __iomem			*aper_base_kaddr;

#if defined(CONFIG_DEBUG_FS)
	struct dentry			*debugfs_entries[8];
#endif

	/* buffer handling */
	const struct gsgpu_buffer_funcs	*buffer_funcs;
	struct gsgpu_ring		*buffer_funcs_ring;
	bool				buffer_funcs_enabled;

	struct mutex			gtt_window_lock;
	/* Scheduler entity for buffer moves */
	struct drm_sched_entity		entity;

	struct gsgpu_vram_mgr		vram_mgr;
	struct gsgpu_gtt_mgr		gtt_mgr;
};

int gsgpu_gtt_mgr_init(struct gsgpu_device *adev, uint64_t gtt_size);
void gsgpu_gtt_mgr_fini(struct gsgpu_device *adev);

bool gsgpu_gtt_mgr_has_gart_addr(struct ttm_resource *mem);
uint64_t gsgpu_gtt_mgr_usage(struct gsgpu_gtt_mgr *man);
void gsgpu_gtt_mgr_recover(struct gsgpu_gtt_mgr *man);

int gsgpu_ttm_init(struct gsgpu_device *adev);
void gsgpu_ttm_late_init(struct gsgpu_device *adev);
void gsgpu_ttm_fini(struct gsgpu_device *adev);
void gsgpu_ttm_set_buffer_funcs_status(struct gsgpu_device *adev,
				       bool enable);

int gsgpu_copy_buffer(struct gsgpu_ring *ring, uint64_t src_offset,
		      uint64_t dst_offset, uint32_t byte_count,
		      struct dma_resv *resv,
		      struct dma_fence **fence, bool direct_submit,
		      bool vm_needs_flush);
int gsgpu_fill_buffer(struct gsgpu_bo *bo,
		      uint32_t src_data,
		      struct dma_resv *resv,
		      struct dma_fence **fence);

int gsgpu_ttm_alloc_gart(struct ttm_buffer_object *bo);
int gsgpu_ttm_recover_gart(struct ttm_buffer_object *tbo);

int gsgpu_ttm_tt_get_user_pages(struct gsgpu_bo *bo, struct page **pages,
				struct hmm_range **range);
bool gsgpu_ttm_tt_get_user_pages_done(struct ttm_tt *ttm,
				      struct hmm_range *range);
void gsgpu_ttm_tt_set_user_pages(struct ttm_tt *ttm, struct page **pages);
void gsgpu_ttm_tt_mark_user_pages(struct ttm_tt *ttm);
int gsgpu_ttm_tt_set_userptr(struct ttm_tt *ttm, uint64_t addr,
			     uint32_t flags);
bool gsgpu_ttm_tt_has_userptr(struct ttm_tt *ttm);
struct mm_struct *gsgpu_ttm_tt_get_usermm(struct ttm_tt *ttm);
bool gsgpu_ttm_tt_affect_userptr(struct ttm_tt *ttm, unsigned long start,
				 unsigned long end);
bool gsgpu_ttm_tt_userptr_invalidated(struct ttm_tt *ttm,
				      int *last_invalidated);
bool gsgpu_ttm_tt_is_userptr(struct ttm_tt *ttm);
bool gsgpu_ttm_tt_is_readonly(struct ttm_tt *ttm);
uint64_t gsgpu_ttm_tt_pte_flags(struct gsgpu_device *adev, struct ttm_tt *ttm,
				struct ttm_resource *mem);
uint64_t gsgpu_ttm_domain_start(struct gsgpu_device *adev, uint32_t type);

#endif /* __GSGPU_TTM_H__ */
