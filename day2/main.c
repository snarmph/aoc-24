#include "../colla/build.c"

typedef struct {
    int values[8];
    int count;
} level_t;

typedef struct {
    level_t *levels;
    int count;
} input_t;

input_t parse_input(arena_t *arena) {
    str_t input = fileReadWholeStr(arena, strv("input.txt"));
    instream_t in = istrInitLen(input.buf, input.len);

    level_t *levels = alloc(arena, level_t, 1000);
    int count = 0;

    while (!istrIsFinished(in)) {
        strview_t lineview = istrGetLine(&in);
        instream_t line = istrInitLen(lineview.buf, lineview.len);

        if (strvIsEmpty(lineview)) {
            break;
        }

        level_t *level = &levels[count++];
        while (!istrIsFinished(line)) {
            int32 value = 0;
            if (!istrGetI32(&line, &value)) {
                break;
            }
            level->values[level->count++] = value;
        }
    }

    if (levels[count - 1].count == 0) {
        count--;
    }

    return (input_t){
        .levels = levels,
        .count = count,
    };
}

#define get_sign(x) ((x > 0) - (x < 0))

bool solve_line(level_t level) {
    int prev_sign = 0;
    for (int i = 1; i < level.count; ++i) {
        int diff = level.values[i] - level.values[i - 1];
        int sign = get_sign(diff);

        if (diff == 0 || abs(diff) > 3) {
            return false;
        }

        if (prev_sign && prev_sign != sign) {
            return false;
        }

        prev_sign = sign;
    }

    return true;
}

int part_1(input_t *input) {
    int safe = 0;

    for (int i = 0; i < input->count; ++i) {
        safe += solve_line(input->levels[i]);
    }

    return safe;
}

level_t line_remove(level_t level, int index) {
    level_t out = {0};

    for (int i = 0; i < level.count; ++i) {
        if (i == index) continue;
        out.values[out.count++] = level.values[i];
    }

    return out;
}

int part_2(input_t *input) {
    int safe = 0;

    for (int i = 0; i < input->count; ++i) {
        level_t line = input->levels[i];
        if (solve_line(line)) {
            safe++;
        }
        else {
            // brute force
            for (int i = 0; i < line.count; ++i) {
                if (solve_line(line_remove(line, i))) {
                    safe++;
                    break;
                }
            }
        }
    }

    return safe;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    
    input_t input = parse_input(&arena);

    int p1 = part_1(&input);
    int p2 = part_2(&input);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);

    arenaCleanup(&arena);
}