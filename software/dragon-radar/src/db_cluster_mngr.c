
#include "db_cluster_mngr.h"

#include "resources/image_dragon_ball_anim.h"
#include "resources/image_number_1_anim.h"
#include "resources/image_number_2_anim.h"
#include "resources/image_number_3_anim.h"
#include "resources/image_number_4_anim.h"
#include "resources/image_number_5_anim.h"
#include "resources/image_number_6_anim.h"
#include "resources/image_number_7_anim.h"
#include "constants.h"
#include "utils.h"

#include <string.h>
#include <math.h>
#include "esp_random.h"


#define DRAGON_BALL_CLUSTER_BALL_DX (-23)
#define DRAGON_BALL_CLUSTER_BALL_DY (-2)
#define DRAGON_BALL_CLUSTER_NUMBER_DX (0)
#define DRAGON_BALL_CLUSTER_NUMBER_DY (-21)
#define DRAGON_BALL_CLUSTER_W (-(DRAGON_BALL_CLUSTER_BALL_DX) + (DRAGON_BALL_CLUSTER_NUMBER_DX) + (NUMBER_FRAME_W))
#define DRAGON_BALL_CLUSTER_H (-(DRAGON_BALL_CLUSTER_NUMBER_DY) + (DRAGON_BALL_CLUSTER_BALL_DY) + (DRAGON_BALL_FRAME_H))

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define DRAGON_BALL_FRAME_W 24
#define DRAGON_BALL_FRAME_H 24

#define NUMBER_FRAME_W 24
#define NUMBER_FRAME_H 31

#define ANIM_FRAME_COUNT 19


const uint8_t anim_frame_ids[ANIM_FRAME_COUNT] = {
    0, 1, 2, 3, 3, 4, 4, 4,
    4, 4, 3, 3, 3, 2, 2, 1, 1, 0, 0
};

const uint8_t *image_number_anim_data[7] = {
    image_number_1_anim_data,
    image_number_2_anim_data,
    image_number_3_anim_data,
    image_number_4_anim_data,
    image_number_5_anim_data,
    image_number_6_anim_data,
    image_number_7_anim_data,
};


typedef struct {
    int x;
    int y;
    int n;
    int enabled;
} DRAGON_BALL_CLUSTER;

struct s_db_cluster_mngr_ctx {
    GFX_CTX gfx_ctx;
    DRAGON_BALL_CLUSTER *dragon_ball_clusters;
    int dragon_ball_cluster_count;
    int max_cluster_distance;
    int anim_frame_id;
};


bool collide_dragon_ball_cluster(int x, int y, DRAGON_BALL_CLUSTER *dragon_ball_clusters, int dragon_ball_cluster_count);
bool draw_dragon_ball_cluster(DB_CLUSTER_MNGR_CTX ctx, int dragon_ball_cluster_id, int width, int height, int zoom);


bool collide_dragon_ball_cluster(int x, int y, DRAGON_BALL_CLUSTER *dragon_ball_clusters, int dragon_ball_cluster_count) {
    for (int i = 0; i < dragon_ball_cluster_count; i++) {
        if ((x + DRAGON_BALL_CLUSTER_W >= dragon_ball_clusters[i].x && x < dragon_ball_clusters[i].x + DRAGON_BALL_CLUSTER_W)
            && (y + DRAGON_BALL_CLUSTER_H >= dragon_ball_clusters[i].y && y < dragon_ball_clusters[i].y + DRAGON_BALL_CLUSTER_H)) {
            return true;
        }
    }

    return false;
}

bool draw_dragon_ball_cluster(DB_CLUSTER_MNGR_CTX ctx, int dragon_ball_cluster_id, int width, int height, int zoom) {

    DRAGON_BALL_CLUSTER *dragon_ball_cluster = &ctx->dragon_ball_clusters[dragon_ball_cluster_id];

    if (!dragon_ball_cluster->enabled) {
        return false;
    }

    int x = (int)((dragon_ball_cluster->x - (width >> 1)) * (1 + (MAX_ZOOM_OUT - zoom) * 0.5));
    int y = (int)((dragon_ball_cluster->y - (height >> 1)) * (1 + (MAX_ZOOM_OUT - zoom) * 0.5));

    if (sqrt(x * x + y * y) >= ctx->max_cluster_distance) {
        return false;
    }

    x += (width >> 1);
    y += (height >> 1);

    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }

    gfx_draw_image(
        ctx->gfx_ctx,
        x + DRAGON_BALL_CLUSTER_BALL_DX,
        y + DRAGON_BALL_CLUSTER_BALL_DY,
        image_dragon_ball_anim_data + (anim_frame_ids[ctx->anim_frame_id] * DRAGON_BALL_FRAME_W * DRAGON_BALL_FRAME_H * 4),
        DRAGON_BALL_FRAME_W,
        DRAGON_BALL_FRAME_H);

    gfx_draw_image(
        ctx->gfx_ctx,
        x + DRAGON_BALL_CLUSTER_NUMBER_DX,
        y + DRAGON_BALL_CLUSTER_NUMBER_DY,
        image_number_anim_data[dragon_ball_cluster->n - 1] + anim_frame_ids[ctx->anim_frame_id] * NUMBER_FRAME_W * NUMBER_FRAME_H * 4,
        NUMBER_FRAME_W,
        NUMBER_FRAME_H);

    return true;
}

void db_cluster_mngr_create(DB_CLUSTER_MNGR_CTX *db_cluster_mngr_ctx, GFX_CTX gfx_ctx) {
    struct s_db_cluster_mngr_ctx *ctx = (struct s_db_cluster_mngr_ctx *)malloc(sizeof(struct s_db_cluster_mngr_ctx));

    *db_cluster_mngr_ctx = ctx;

    memset(ctx, 0, sizeof(struct s_db_cluster_mngr_ctx));

    ctx->gfx_ctx = gfx_ctx;
    ctx->anim_frame_id = 0;

    int width = gfx_get_width(gfx_ctx);
    int height = gfx_get_height(gfx_ctx);

    ctx->max_cluster_distance = MIN((width - DRAGON_BALL_CLUSTER_W) / 2, (height - DRAGON_BALL_CLUSTER_H) / 2);

    ctx->dragon_ball_cluster_count = 1 + (esp_random() % 7);
    ctx->dragon_ball_clusters = (DRAGON_BALL_CLUSTER *)malloc(ctx->dragon_ball_cluster_count * sizeof(DRAGON_BALL_CLUSTER));

    for (int i = 0; i < ctx->dragon_ball_cluster_count; i++) {
        bool enabled = false;
        int x;
        int y;

        // Try 10 times to find a position without any collision with existing clusters
        for (int r = 0; r < 10; r++) {
            int angle = esp_random() % 360;
            int d = esp_random() % ctx->max_cluster_distance;
            x = (int)(sin(angle * M_PI / 180) * d) + (width / 2);
            y = (int)(cos(angle * M_PI / 180) * d) + (height / 2);
            if (!collide_dragon_ball_cluster(x, y, ctx->dragon_ball_clusters, ctx->dragon_ball_cluster_count)) {
                enabled = true;
                break;
            }
        }

        if (!enabled) {
            ctx->dragon_ball_clusters[i].x = 0;
            ctx->dragon_ball_clusters[i].y = 0;
            ctx->dragon_ball_clusters[i].n = 0;
            ctx->dragon_ball_clusters[i].enabled = false;
        } else {
            ctx->dragon_ball_clusters[i].x = x;
            ctx->dragon_ball_clusters[i].y = y;
            ctx->dragon_ball_clusters[i].n = 0;
            ctx->dragon_ball_clusters[i].enabled = true;
        }
    }

    int current_cluster_idx = 0;

    for (int i = 0; i < 7; i++) {
        ctx->dragon_ball_clusters[current_cluster_idx].n++;
        current_cluster_idx++;
        if (current_cluster_idx >= ctx->dragon_ball_cluster_count) {
            current_cluster_idx = 0;
        }
    }
}

void db_cluster_mngr_delete(DB_CLUSTER_MNGR_CTX ctx) {
    free(ctx);
}

bool db_cluster_mngr_update(DB_CLUSTER_MNGR_CTX ctx, int zoom) {
    bool one_dragon_ball_cluster_visible = false;
    
    if (ctx->anim_frame_id < ANIM_FRAME_COUNT) {
        int width = gfx_get_width(ctx->gfx_ctx);
        int height = gfx_get_height(ctx->gfx_ctx);

        for (int i = 0; i < ctx->dragon_ball_cluster_count; i++) {
            if (draw_dragon_ball_cluster(ctx, i, width, height, zoom)) {
                one_dragon_ball_cluster_visible = true;
            }
        }
    }

    bool playsound = false;

    static int64_t previous_frame_time = 0;
    int64_t t = millis();
    if (t - previous_frame_time >= 30) {
        if (one_dragon_ball_cluster_visible && ctx->anim_frame_id == 5) {
            playsound = true;
        }

        ctx->anim_frame_id++;
        if (ctx->anim_frame_id >= ANIM_FRAME_COUNT + 15) {
            ctx->anim_frame_id = 0;
        }
        previous_frame_time = t;
    }

    return playsound;
}
