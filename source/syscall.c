#if 1
int _close(int file)
{
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}
#endif

void *_sbrk(int incr)
{
    return (void*)0;
}
