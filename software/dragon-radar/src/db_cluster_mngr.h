
#ifndef __DB_CLUSTER_MNGR_H__
#define __DB_CLUSTER_MNGR_H__


#include "gfx.h"

#include <inttypes.h>


typedef struct s_db_cluster_mngr_ctx *DB_CLUSTER_MNGR_CTX;


void db_cluster_mngr_create(DB_CLUSTER_MNGR_CTX *ctx, GFX_CTX gfx_ctx);
void db_cluster_mngr_delete(DB_CLUSTER_MNGR_CTX ctx);
bool db_cluster_mngr_update(DB_CLUSTER_MNGR_CTX ctx, int zoom);


#endif