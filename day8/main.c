#include "../colla/build.c"

#define ANTENNA_TYPES 256 // CHAR_MAX

typedef struct {
    int x;
    int y;
} antenna_t;

typedef struct {
    antenna_t antennas[ANTENNA_TYPES][12];
    int count[ANTENNA_TYPES];
    int width;
    int height;
} input_t;

input_t parse_input(arena_t scratch, strview_t file) {
    input_t out = {0};
    str_t data = fileReadWholeStr(&scratch, file);
    instream_t in = istrInitLen(data.buf, data.len);

    while (!istrIsFinished(in)) {
        strview_t line = istrGetLine(&in);
        if (!out.width) out.width = line.len;

        for (usize i = 0; i < line.len; ++i) {
            char c = line.buf[i];
            if (c != '.') {
                assert(out.count[c] < 12);
                antenna_t *an = &out.antennas[c][out.count[c]++];
                an->x = i;
                an->y = out.height;
            }
        }

        out.height++;
    }

    return out;
}

int intcmp(const void *a, const void *b) {
    return (*(const int*)a) - (*(const int*)b);
}

int count_unique(int *arr, int len) {
    int out = 0;

    qsort(arr, len, sizeof(int), intcmp);
    
    out += len > 1 ? arr[0] != arr[1] : 0;

    for (int i = 1; i < len; ++i) {
        out += arr[i] != arr[i - 1];
    }

    return out;
}

int test(input_t *in, char c, int left_idx, int right_idx) {
    antenna_t *left = &in->antennas[c][left_idx];
    antenna_t *right = &in->antennas[c][right_idx];

    int diffx = right->x - left->x;
    int diffy = right->y - left->y;

    int px = right->x + diffx;
    int py = right->y + diffy;

    bool valid = (px >= 0 && px < in->width) && (py >= 0 && py < in->height);
    
    return valid ? px + py * in->width : -1;
}

int part_1(input_t *in) {
    int positions[1000];
    int pcount = 0;
    
    for (int i = 0; i < ANTENNA_TYPES; ++i) {
        if (in->count[i] == 0) continue;
        int count = in->count[i];
        for (int y = 0; y < count; ++y) {
            for (int x = 0; x < count; ++x) {
                if (x == y) continue;
                int p = test(in, i, x, y);
                if (p >= 0) {
                    positions[pcount++] = p;
                }
            }
        }
    }

    return count_unique(positions, pcount);
}

void test_repeat(input_t *in, char c, int left_idx, int right_idx, int *positions, int *pcount) {
    antenna_t *left = &in->antennas[c][left_idx];
    antenna_t *right = &in->antennas[c][right_idx];

    int diffx = right->x - left->x;
    int diffy = right->y - left->y;

    int px = right->x;
    int py = right->y;

    int count = *pcount;

    while (true) {
        bool valid = (px >= 0 && px < in->width) && (py >= 0 && py < in->height);
        if (!valid) break;
        assert(count < 10000);
        positions[count++] = px + py * in->width;
        px += diffx;
        py += diffy;
    }

    (*pcount) = count;
}

int part_2(input_t *in) {
    int positions[10000];
    int pcount = 0;
    
    for (int i = 0; i < ANTENNA_TYPES; ++i) {
        if (in->count[i] == 0) continue;
        int count = in->count[i];
        for (int y = 0; y < count; ++y) {
            for (int x = 0; x < count; ++x) {
                if (x == y) continue;
                test_repeat(in, i, x, y, positions, &pcount);
            }
        }
    }

    return count_unique(positions, pcount);
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    input_t input = parse_input(arena, strv("input.txt"));

    int p1 = part_1(&input);
    int p2 = part_2(&input);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);

    arenaCleanup(&arena);
}