#include "../colla/build.c"

typedef struct {
    strview_t *lines;
    int lines_count;
} input_t;

input_t read_input(arena_t *arena, str_t input) {
    input_t out = {
        .lines = alloc(arena, strview_t, 1000)
    };

    instream_t in = istrInitLen(input.buf, input.len);

    while (!istrIsFinished(in)) {
        strview_t line = strvTrim(istrGetLine(&in));
        if (!strvIsEmpty(line)) {
            assert(out.lines_count < 1000);
            out.lines[out.lines_count++] = line;
        }
    }

    return out;
}

int check_crossword(input_t in, int row, int col) {
    int result = 0;
    int line_length = in.lines[0].len;
    strview_t xmas = strv("XMAS");

    for (int yoff = -1; yoff <= 1; ++yoff) {
        int row_beg = row + yoff;
        int rend = row + yoff * 3;
        if (rend < 0 || rend >= in.lines_count) continue;

        for (int xoff = -1; xoff <= 1; ++xoff) {
            if (xoff == 0 && yoff == 0) continue;
            int col_beg = col + xoff;
            int cend = col + xoff * 3;
            if (cend < 0 || cend >= line_length) continue;

            bool is_xmas = true;
            int r = row_beg, c = col_beg;
            // start off 1 because we already checked 'X'
            for (int i = 1; i < 4; ++i) {
                char cur = in.lines[r].buf[c];
                if (cur != xmas.buf[i]) {
                    is_xmas = false;
                    break;
                }
                r += yoff;
                c += xoff;
            }
            result += is_xmas;
        }
    }

    return result;
}

int part_1(input_t in) {
    int result = 0;

    for (int row = 0; row < in.lines_count; ++row) {
        strview_t line = in.lines[row];
        for (int col = 0; col < line.len; ++col) {
            if (line.buf[col] == 'X') {
                result += check_crossword(in, row, col);
            }
        }
    }

    return result;
}

bool check_mas(input_t in, int row, int col) {
    strview_t line_bef = in.lines[row - 1];
    strview_t line_aft = in.lines[row + 1];

    char topleft  = line_bef.buf[col - 1];
    char topright = line_bef.buf[col + 1];
    char btmleft  = line_aft.buf[col - 1];
    char btmright = line_aft.buf[col + 1];

    bool tl = topleft == 'M' && btmright == 'S';
    bool br = topleft == 'S' && btmright == 'M';

    bool tr = topright == 'M' && btmleft == 'S';
    bool bl = topright == 'S' && btmleft == 'M';

    return (tl || br) && (tr || bl);
}

int part_2(input_t in) {
    int result = 0;

    for (int row = 1; row < (in.lines_count - 1); ++row) {
        strview_t line = in.lines[row];
        for (int col = 1; col < (line.len - 1); ++col) {
            if (line.buf[col] == 'A') {
                result += check_mas(in, row, col);
            }
        }
    }

    return result;
}

int main() {
    arena_t arena = arenaMake(ARENA_VIRTUAL, GB(1));
    str_t file_data = fileReadWholeStr(&arena, strv("input.txt"));
    input_t input = read_input(&arena, file_data);

    int p1 = part_1(input);
    int p2 = part_2(input);

    info("result part 1: %d", p1);
    info("result part 2: %d", p2);

    arenaCleanup(&arena);
}