#include "AppHdr.h"

#include "coord.h"

#include "libutil.h"
#include "state.h"
#include "viewgeom.h"

//////////////////////////////////////////////////////////////////////////
// coord_def

int grid_distance(const coord_def& p1, const coord_def& p2)
{
    return (p2 - p1).rdist();
}

int distance2(const coord_def& p1, const coord_def& p2)
{
    return (p2 - p1).abs();
}

bool adjacent(const coord_def& p1, const coord_def& p2)
{
    return grid_distance(p1, p2) <= 1;
}

bool map_bounds_with_margin(coord_def p, int margin)
{
    return p.x >= X_BOUND_1 + margin && p.x <= X_BOUND_2 - margin
           && p.y >= Y_BOUND_1 + margin && p.y <= Y_BOUND_2 - margin;
}

coord_def clip(const coord_def &p)
{
    if (crawl_state.game_is_arena())
    {
        const coord_def &ul = crawl_view.glos1; // Upper left
        const coord_def &lr = crawl_view.glos2; // Lower right
        int x = p.x % (lr.x - ul.x + 1);
        int y = p.y % (lr.y - ul.y + 1);
        return coord_def(x, y);
    }
    int x = abs(p.x) % (GXM - 1) + MAPGEN_BORDER - 1;
    int y = abs(p.y) % (GYM - 1) + MAPGEN_BORDER - 1;
    return coord_def(x, y);
}

coord_def random_in_bounds(int constrain)
{
    if (crawl_state.game_is_arena())
    {
        const coord_def &ul = crawl_view.glos1; // Upper left
        const coord_def &lr = crawl_view.glos2; // Lower right

        return coord_def(random_range(ul.x, lr.x - 1),
                         random_range(ul.y, lr.y - 1));
    } else {
        int minx = MAPGEN_BORDER; int maxx = GXM-MAPGEN_BORDER - 1;
        int miny = MAPGEN_BORDER; int maxy = GYM-MAPGEN_BORDER - 1;
        // XXX we assume only called with constrain relative to you.pos!
        if (constrain > 0) {
            int factor = 1 + 2 * constrain;
            minx = max(minx,you.pos().x-(GXM / factor));
            maxx = min(maxx,you.pos().x+(GXM / factor));
            miny = max(miny,you.pos().y-(GYM / factor));
            maxy = min(maxy,you.pos().y+(GYM / factor));
        }
        return coord_def(random_range(minx, maxx),
                         random_range(miny, maxy));
    }
}

// Coordinate system conversions.

coord_def player2grid(const coord_def &pc)
{
    return pc + you.pos();
}

coord_def grid2player(const coord_def &gc)
{
    return gc - you.pos();
}

//rotates a coord_def that points to an adjacent square
//clockwise (direction > 0), or counter-clockwise (direction < 0)
coord_def rotate_adjacent(coord_def vector, int direction)
{
    int xn, yn;

    xn = vector.x - sgn(direction) * vector.y;
    yn = sgn(direction) * vector.x + vector.y;
    vector.x = xn;
    vector.y = yn;
    return vector.sgn();
}

coord_def clamp_in_bounds(const coord_def &p)
{
    return coord_def(
        min(X_BOUND_2 - 1, max(X_BOUND_1 + 1, p.x)),
        min(Y_BOUND_2 - 1, max(Y_BOUND_1 + 1, p.y)));
}
