struct pair {
    int x, y;
    struct pair *next;
};

struct pair s1, s2;

int main(void)
{
    struct pair *sp;
    struct pair **spp;

    1 + 2 * 3 - 4 / 5 && 6 % 7 || 8 == 9;

    sp = &s1;
    spp = &sp;
    sp->x = s1.x;
    (*spp)->next->y = s2.y;
}
