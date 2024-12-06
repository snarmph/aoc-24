#include "../colla/build.c"

#define MAX_MAP_SIZE (200 * 200)

#define NONE   0
#define WALL   1
#define VISITU 2
#define VISITD 3
#define VISITL 4
#define VISITR 5

typedef struct {
    uint8 map[MAX_MAP_SIZE];
    int rows;
    int cols;
    int start_pos;
} input_t;

input_t parse_input(arena_t scratch, strview_t file) {
    input_t out = {0};
    str_t data = fileReadWholeStr(&scratch, file);
    instream_t in = istrInitLen(data.buf, data.len);

    int cur = 0;
    while (!istrIsFinished(in)) {
        strview_t line = istrGetLine(&in);
        if (strvIsEmpty(line)) {
            break;
        }

        if (out.cols == 0) {
            out.cols = line.len;
        }
        out.rows++;

        for (usize i = 0; i < line.len; ++i) {
            switch (line.buf[i]) {
                case '.': out.map[cur++] = NONE;  break;
                case '#': out.map[cur++] = WALL;  break;
                case '^': 
                    out.start_pos = cur;
                    out.map[cur++] = VISITU;
                    break;
            }
        }
    }

    return out;
}

uint8 enum_from_dir(int dirx, int diry) {
    if (dirx > 0) return VISITR;
    if (dirx < 0) return VISITL;
    if (diry > 0) return VISITD;
    if (diry < 0) return VISITU;
    assert(false);
    return NONE;
}

int part_1(input_t in) {
    int result = 1;

    int posx = in.start_pos % in.cols;
    int posy = in.start_pos / in.cols;
    int dirx = 0, diry = -1;

    while (true) {
        posx += dirx;
        posy += diry;
        if (posx < 0 || posx >= in.cols) break;
        if (posy < 0 || posy >= in.rows) break;
        int index = posx + posy * in.cols;
        switch (in.map[index]) {
            case NONE:
                in.map[index] = enum_from_dir(dirx, diry);
                result++;
                break;
            case WALL:
            {
                // rotate 90deg
                int oldx = dirx, oldy = diry;
                dirx = -oldy;
                diry =  oldx;
                posx -= oldx;
                posy -= oldy;
                break;
            }
        }
    }

    return result;
}

bool goes_in_loop(input_t in) {
    int posx = in.start_pos % in.cols;
    int posy = in.start_pos / in.cols;
    int dirx = 0, diry = -1;

    while (true) {
        posx += dirx;
        posy += diry;
        if (posx < 0 || posx >= in.cols) break;
        if (posy < 0 || posy >= in.rows) break;
        int index = posx + posy * in.cols;
        switch (in.map[index]) {
            case NONE:
                in.map[index] = enum_from_dir(dirx, diry);
                break;
            case WALL:
            {
                // rotate 90deg
                int oldx = dirx, oldy = diry;
                dirx = -oldy;
                diry =  oldx;
                posx -= oldx;
                posy -= oldy;
                break;
            }
            default:
            {   
                uint8 cur = enum_from_dir(dirx, diry);
                if (in.map[index] == cur) {
                    return true;
                }
            }
        }
    }

    return false;
}

int part_2(input_t in) {
    int result = 0;

    input_t original = in;

    int posx = in.start_pos % in.cols;
    int posy = in.start_pos / in.cols;
    int dirx = 0, diry = -1;
    int prev_index = in.start_pos;

    while (true) {
        posx += dirx;
        posy += diry;
        if (posx < 0 || posx >= in.cols) break;
        if (posy < 0 || posy >= in.rows) break;
        int index = posx + posy * in.cols;

        switch (in.map[index]) {
            case NONE:
                original.map[index] = WALL;
                result += goes_in_loop(original);
                original.map[index] = in.map[index];

                in.map[index] = enum_from_dir(dirx, diry);
                break;
            case WALL:
            {
                // rotate 90deg
                int oldx = dirx, oldy = diry;
                dirx = -oldy;
                diry =  oldx;
                posx -= oldx;
                posy -= oldy;
                break;
            }
        }
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    input_t input = parse_input(arena, strv("input.txt"));

    int p1 = part_1(input);
    int p2 = part_2(input);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);

    arenaCleanup(&arena);
}