#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <vcl.h>
#include <vrt.h>
#include <vtree.h>
#include <cache/cache.h>

#include "vcc_if.h"

struct node {
	unsigned		magic;
#define MFD_NODE_MAGIC		0xc5daec29
	char			*idx;
	VCL_BACKEND		be;
	VRB_ENTRY(node)		tr;
};

VRB_HEAD(backend_tree, node);

struct vmod_stendhal_director {
	unsigned		magic;
#define MFD_MAGIC		0xa99e570a
	pthread_rwlock_t	mtx;
	struct backend_tree	bet;
};

inline int
cmp_idx(const struct node *a, const struct node *b)
{
	return (strcmp(a->idx, b->idx));
}

VRB_PROTOTYPE_STATIC(backend_tree, node, tr, cmp_idx);
VRB_GENERATE_STATIC(backend_tree, node, tr, cmp_idx);

VCL_VOID __match_proto__()
vmod_director__init(VRT_CTX,
    struct vmod_stendhal_director **sd,
    const char *vcl_name)
{
	struct vmod_stendhal_director *d;

	(void)ctx;
	(void)vcl_name;
	ALLOC_OBJ(d, MFD_MAGIC);
	AZ(pthread_rwlock_init(&d->mtx, NULL));
	VRB_INIT(&d->bet);
	*sd = d;
}


VCL_VOID __match_proto__()
vmod_director__fini(struct vmod_stendhal_director **sd)
{
	struct node *nd, *nnd;
	struct vmod_stendhal_director *d;

	AN(sd);
	d = *sd;
	CHECK_OBJ_NOTNULL(d, MFD_MAGIC);

	for (nd = VRB_MIN(backend_tree, &d->bet);
			nd != NULL; nd = nnd) {
		nnd = VRB_NEXT(backend_tree, &d->bet, nd);
		VRB_REMOVE(backend_tree, &d->bet, nd);
		free(nd->idx);
		free(nd);
	}
	AZ(pthread_rwlock_destroy(&d->mtx));
}

VCL_VOID __match_proto__()
vmod_director_add_backend(VRT_CTX, struct vmod_stendhal_director *sd,
		VCL_STRING idx, const VCL_BACKEND be)
{
	struct node *nd, tmp = {0};
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(sd, MFD_MAGIC);

	tmp.idx = strdup(idx ? idx : "");

	pthread_rwlock_wrlock(&sd->mtx);
	nd = VRB_FIND(backend_tree, &sd->bet, &tmp);
	if (nd) {
		nd->be = be;
	} else {
		ALLOC_OBJ(nd, MFD_NODE_MAGIC);
		nd->idx = tmp.idx;
		tmp.idx = NULL;
		nd->be = be;
		VRB_INSERT(backend_tree, &sd->bet, nd);
	}
	pthread_rwlock_unlock(&sd->mtx);

	free(tmp.idx);
}

VCL_VOID __match_proto__()
vmod_director_remove_backend(VRT_CTX, struct vmod_stendhal_director *sd,
		VCL_STRING idx)
{
	struct node *nd, tmp = {0};
	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(sd, MFD_MAGIC);

	tmp.idx = strdup(idx ? idx : "");

	pthread_rwlock_wrlock(&sd->mtx);
	nd = VRB_FIND(backend_tree, &sd->bet, &tmp);
	if (nd) { 
		VRB_REMOVE(backend_tree, &sd->bet, nd);
		free(nd->idx);
		free(nd);
	}
	pthread_rwlock_unlock(&sd->mtx);

	free(tmp.idx);
}

VCL_BACKEND __match_proto__()
vmod_director_backend(VRT_CTX, struct vmod_stendhal_director *sd,
		VCL_STRING idx)
{
	VCL_BACKEND be = NULL;
	struct node *nd, tmp = {0};

	tmp.idx = strdup(idx ? idx : "");

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	CHECK_OBJ_NOTNULL(sd, MFD_MAGIC);
	pthread_rwlock_rdlock(&sd->mtx);
	nd = VRB_FIND(backend_tree, &sd->bet, &tmp);
	if (nd)
		be = nd->be;
	pthread_rwlock_unlock(&sd->mtx);

	free(tmp.idx);
	return (be);
}
