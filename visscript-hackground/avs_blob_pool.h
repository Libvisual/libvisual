#ifndef _AVS_BLOB_POOL_H
#define _AVS_BLOB_POOL_H 1

#define AVS_BLOB_POOL(obj)			(VISUAL_CHECK_CAST ((obj), AvsBlobPool))

struct _AvsBlobPool;
typedef struct _AvsBlobPool AvsBlobPool;
struct _AvsBloblPoolElement;
typedef struct _AvsBlobPoolElement AvsBlobPoolElement;

struct _AvsBlobPool {
	VisObject		object;
	int			elem_size;	/**< Element size */
	AvsBlobManager		*blob; 		/**< Avs Blobl Manager used for memeory management */
	AvsBlobPoolElement	*pool; 		/**< AEP: Available Element Pool */
};

struct _AvsBlobPoolElement {
	AvsBlobPoolElement	*next;
	char			data[];
};

#define avs_blob_pool_new0(elem_type) \
	avs_blob_pool_new(sizeof(elem_type))

/* prototypes */
int avs_blob_pool_pop(AvsBlobPool *bp, void *elem);
void *avs_blob_pool_push(AvsBlobPool *bp);
AvsBlobPool *avs_blob_pool_new(int elem_size);

#endif /* !_AVS_BLOB_POOL_H */
