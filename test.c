__attribute__((weak))
void test(void)
{
    printf("test\n");
}

int main(void)
{
    test();
    return (0);
}