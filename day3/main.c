#include "../colla/build.c"

bool is_number(strview_t v) {
    for (usize i = 0; i < v.len; ++i) {
        if (v.buf[i] < '0' || v.buf[i] > '9') {
            return false;
        }
    }
    return true;
}

int view_to_num(strview_t v) {
    int out = 0;
    for (usize i = 0; i < v.len; ++i) {
        int k = v.buf[i] - '0';
        out = out * 10 + k;
    }
    return out;
}

bool try_parse_mul(instream_t *in, int *out) {
    instream_t before = *in;
    strview_t prefix = istrGetViewLen(in, 4); // try get mul(

    if (!strvEquals(prefix, strv("mul("))) {
        goto failed;
    }
    
    strview_t left = istrGetView(in, ',');
    if (left.len > 3 || !is_number(left)) {
        goto failed;
    }

    istrSkip(in, 1); // skip ,

    strview_t right = istrGetView(in, ')');
    if (right.len > 3 || !is_number(right)) {
        goto failed;
    }

    if (istrPeek(in) != ')') {
        goto failed;
    }

    *out = view_to_num(left) * view_to_num(right);

    return true;
failed:
    *in = before;
    return false;
}

int part_1(instream_t in) {
    int result = 0;
    while (!istrIsFinished(in)) {
        int curmul = 0;
        bool is_valid = istrPeek(&in) == 'm' && try_parse_mul(&in, &curmul);
        if (!is_valid) {
            istrSkip(&in, 1);
        }
        else {
            result += curmul;
        }
    }
    return result;
}

bool try_parse_do(instream_t *in, bool *out) {
    strview_t v = istrGetViewLen(in, 4); // do()
    
    if (strvEquals(v, strv("do()"))) {
        *out = true;
        return true;
    }

    istrRewindN(in, 4);
    return false;
}

bool try_parse_dont(instream_t *in, bool *out) {
    strview_t v = istrGetViewLen(in, 7); // don't()
    
    if (strvEquals(v, strv("don't()"))) {
        *out = false;
        return true;
    }

    istrRewindN(in, 7);
    return false;
}

int part_2(instream_t in) {
    int result = 0;
    bool is_enabled = true;

    while (!istrIsFinished(in)) {
        int curmul = 0;

        char cur = istrPeek(&in);

        bool parsed = false;

        if (!is_enabled) {
            if (cur == 'd') {
                parsed = try_parse_do(&in, &is_enabled);
            }
        }
        else {
            if (cur == 'm') {
                parsed = try_parse_mul(&in, &curmul);
            }
            else if (cur == 'd') {
                parsed = try_parse_dont(&in, &is_enabled);
            }
        }

        if (!parsed) {
            istrSkip(&in, 1);
        }
        result += curmul;
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    str_t input = fileReadWholeStr(&arena, strv("input.txt"));
    instream_t in = istrInitLen(input.buf, input.len);

    int p1 = part_1(in);
    int p2 = part_2(in);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);

    arenaCleanup(&arena);
}