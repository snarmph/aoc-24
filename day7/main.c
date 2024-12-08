#include "../colla/build.c"

typedef struct {
    int64 result;
    int values[16];
    int count;
} equation_t;

typedef struct {
    equation_t equations[1000];
    int count;
} input_t;

input_t parse_input(arena_t scratch, strview_t file) {
    input_t out = {0};
    str_t file_data = fileReadWholeStr(&scratch, file);
    instream_t in = istrInitLen(file_data.buf, file_data.len);

    while (!istrIsFinished(in)) {
        strview_t lineview = istrGetLine(&in);
        instream_t line = istrInitLen(lineview.buf, lineview.len);
        equation_t *eq = &out.equations[out.count++];
        istrGetI64(&line, &eq->result);
        istrSkip(&line, 1);
        while (!istrIsFinished(line)) {
            istrSkipWhitespace(&line);
            assert(eq->count < 16);
            istrGetI32(&line, &eq->values[eq->count++]);
        }
    }

    return out;
}

void print_eq(equation_t *eq, uint32 pattern, int split) {
    printf("%ld = ", eq->result);
    
    for (uint32 i = 0; i < split; ++i) {
        putchar(pattern & (1 << i) ? '*' : '+');
        printf(" %d ", eq->values[i]);
    }

    if (split) printf("|| ");
    
    for (uint32 i = split; i < eq->count; ++i) {
        putchar(pattern & (1 << i) ? '*' : '+');
        printf(" %d ", eq->values[i]);
    }
    puts("");
}

int64 merge_nums(int64 left, int64 right) {
    int64 out = left;
    int64 tmp = right;
    while (tmp) {
        out *= 10;
        tmp /= 10;
    }
    return out + right;
}

bool try_resolve(equation_t *eq, uint32 pattern, int split) {
    int64 result = 0;

    for (uint32 i = 0; i < eq->count; ++i) {
        if (result > eq->result) return false;
        if (pattern & (1 << i)) {
            result *= eq->values[i];
        }
        else {
            result += eq->values[i];
        }
    }

    return result == eq->result;
}

int64 part_1(input_t *in) {
    int64 result = 0;

    for (int i = 0; i < in->count; ++i) {
        equation_t *eq = &in->equations[i];

        uint32 max_pattern = 1 << eq->count;
        for (uint32 pattern = 0; pattern < max_pattern; pattern += 2) {
            if (try_resolve(eq, pattern, 0)) {
                result += eq->result;
                break;
            }
        }
    }

    return result;
}

bool try_resolve_split(equation_t *eq, uint32 pattern, uint32 split) {
    int64 result = 0;

    for (uint32 i = 0; i < eq->count; ++i) {
        if (result > eq->result) return false;
        bool mul = pattern & (1 << i);
        bool spl = split & (1 << i);
        if (mul && spl) {
            result = merge_nums(result, eq->values[i]);
        }
        else if (mul) {
            result *= eq->values[i];
        }
        else {
            result += eq->values[i];
        }
    }

    return result == eq->result;
}

int64 part_2(input_t *in) {
    int64 result = 0;

    for (int i = 0; i < in->count; ++i) {
        equation_t *eq = &in->equations[i];
        
        uint32 max_pattern = 1 << eq->count;

        for (uint32 split = 0; split < max_pattern; split += 2) {
            for (uint32 pattern = 0; pattern < max_pattern; pattern += 2) {
                if (try_resolve_split(eq, pattern, split)) {
                    result += eq->result;
                    goto finished;
                }
            }
        }
finished:
        continue;
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));

    input_t input = parse_input(arena, strv("input.txt"));

    int64 p1 = part_1(&input);
    int64 p2 = part_2(&input);

    info("result part 1: %ld", p1);
    info("result part 2: %ld", p2);

    arenaCleanup(&arena);
}