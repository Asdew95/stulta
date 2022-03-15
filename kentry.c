void kentry(void)
{
    char *fb = (char*) 0xb8000;
    fb[0] = 'A';
}
