#include "../colla/build.c"

typedef struct {
    int v[25];
    int len;
} line_t;

typedef struct {
    bool dep[100][100];
    line_t lines[200];
    int lines_count;
} input_t;

void strv_split(strview_t v, strview_t *left, strview_t *right, char c) {
    usize pos = strvFind(v, c, 0);
    if (left) {
        (*left) = strvSub(v, 0, pos);
    }
    if (right) {
        (*right) = strvSub(v, pos < SIZE_MAX ? pos + 1 : SIZE_MAX, SIZE_MAX);
    }
}

int view_to_num(strview_t v) {
    int out = 0;
    for (usize i = 0; i < v.len; ++i) {
        int k = v.buf[i] - '0';
        out = out * 10 + k;
    }
    return out;
}

input_t parse_input(arena_t scratch, strview_t file) {
    input_t out = {0};
    str_t filedata = fileReadWholeStr(&scratch, file);
    instream_t in = istrInitLen(filedata.buf, filedata.len);

    while (!istrIsFinished(in)) {
        strview_t line = istrGetLine(&in);
        // empty line divides inputs
        if (strvIsEmpty(line)) break; 
        strview_t left, right;
        strv_split(line, &left, &right, '|');
        int bef = view_to_num(left);
        int aft = view_to_num(right);
        assert(bef < 100 && aft < 100);
        out.dep[aft][bef] = true;
    }

    while (!istrIsFinished(in)) {
        strview_t line_view = istrGetLine(&in);
        instream_t line_istr = istrInitLen(line_view.buf, line_view.len);
        line_t *line = &out.lines[out.lines_count++];
        while (!istrIsFinished(line_istr)) {
            istrGetI32(&line_istr, &line->v[line->len++]);
            istrSkip(&line_istr, 1); // skip ,
        }
    }

    return out;
}

int part_1(input_t *in) {
    int result = 0;

    for (int i = 0; i < in->lines_count; ++i) {
        line_t *line = &in->lines[i];

        bool valid_line = true;
        for (int cur = 0; cur < line->len; ++cur) {
            bool valid_num = true;
            for (int check = cur + 1; check < line->len; ++check) {
                if (!in->dep[line->v[check]][line->v[cur]]) {
                    valid_num = false;
                    break;
                }
            }
            if (!valid_num) {
                valid_line = false;
                break;
            }
        }

        if (valid_line) {
            result += line->v[line->len / 2];
        }
    }

    return result;
}

int part_2(input_t *in) {
    int result = 0;

    for (int i = 0; i < in->lines_count; ++i) {
        line_t *line = &in->lines[i];

        bool invalid = false;
        bool valid_line = false;
        for (int try = 0; try < 100 && !valid_line; ++try) {
            valid_line = true;
            for (int cur = 0; cur < line->len; ++cur) {
                bool valid_num = true;
                for (int check = cur + 1; check < line->len; ++check) {
                    if (!in->dep[line->v[check]][line->v[cur]]) {
                        // warn("failed: %d|%d", line->v[cur], line->v[check]);
                        int t = line->v[check];
                        line->v[check] = line->v[cur];
                        line->v[cur] = t;
                        valid_num = false;
                    }
                }
                if (!valid_num) {
                    valid_line = false;
                    invalid = true;
                    break;
                }
            }
        }

        if (invalid) {
            result += line->v[line->len / 2];
        }
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    input_t input = parse_input(arena, strv("input.txt"));

    int p1 = part_1(&input);
    int p2 = part_2(&input);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);
}