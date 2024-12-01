#include "colla/build.c"

int int_cmp(const void *a, const void *b) {
    int left = *((const int *)a);
    int right = *((const int *)b);
    return left - right;
}

int part_1(int a[1000], int b[1000], int count) {
    int diff = 0;
    for (int i = 0; i < count; ++i) {
        diff += abs(a[i] - b[i]);
    }

    return diff;
}

int part_2(int a[1000], int b[1000], int count) {
    int k = 0;
    int result = 0;

    int prev_num = 0;
    int prev_mul = 0;

    for (int i = 0; i < count; ++i) {
        if (a[i] == prev_num) {
            result += prev_mul;
            continue;
        }
        prev_num = a[i];

        int found = 0;
        for (; k < count; ++k) {
            if (a[i] == b[k]) {
                found++;
            }
            else if (a[i] > b[k]) {
                continue;
            }
            else if (a[i] < b[k]) {
                break;
            }
        }

        prev_mul = a[i] * found;
        result += prev_mul;
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, MB(1));
    
    int a[1000];
    int b[1000];
    int count = 0;

    str_t input = fileReadWholeStr(&arena, strv("input-final.txt"));
    instream_t in = istrInitLen(input.buf, input.len);

    while (!istrIsFinished(in)) {
        istrGetI32(&in, a + count);
        istrSkipWhitespace(&in);
        istrGetI32(&in, b + count);
        istrSkipWhitespace(&in);

        ++count;
    }

    qsort(a, count, sizeof(int), int_cmp);
    qsort(b, count, sizeof(int), int_cmp);

    int result = part_2(a, b, count);
    info("result: %d", result);
}
    /*
    arr<int> a, b;
    
    str s = file_read("input.txt")
    while (view line = read_line(s)) {
        view left, right = split(line, ' ')
        a.push(to_num(left))
        b.push(to_num(right))
    }

    a.sort()
    b.sort()

    int k = 0;
    int result = 0;
    for (int i = 0; i < count; ++i) {
        info("> %d", a[i]);
        int before = k;
        for (; k < count; ++k) {
            if (a[i] != b[k]) {
                break;
            }
        }

        result += a[i] * (k - before);
        break;
    }
    
    print(diff)
    
    */