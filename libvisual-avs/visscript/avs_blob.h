#ifndef _AVS_BLOB_H
#define _AVS_BLOB_H 1

#define AVS_BLOB_MANAGER(obj)	(VISUAL_CHECK_CAST ((obj), AvsBlobManager))

struct _AvsBlobManager;
typedef struct _AvsBlobManager AvsBlobManager;
struct _AvsBlob;
typedef struct _AvsBlob AvsBlob;

struct _AvsBlob {
	AvsBlob		*prev, *next;
	unsigned char	data[];
};

struct _AvsBlobManager {
	VisObject	object;
	AvsBlob		*pool;
};

/* prototypes */
int avs_blob_destroy(AvsBlobManager *bm, void *data);
void *avs_blob_new(AvsBlobManager *bm, visual_size_t nbytes);
int avs_blob_manager_init(AvsBlobManager *bm);
AvsBlobManager *avs_blob_manager_new(void);

#endif /* !_AVS_BLOB_H */
