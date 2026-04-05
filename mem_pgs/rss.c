#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>


// ЗАПУСК БИНАРЯ С SUDO!!!
// Иначе ядро не покажет физические адреса

/*
0x8 = 1000 — первый бит (бит 63) = 1 -> Страница в памяти
Если бы entry начинался с 0x0... — бит 63 = 0, страницы нет в памяти.
*/
int pagemap(int fd, long page_size, void *addr) {
    off_t offset = (unsigned long)addr / page_size * 8;
    if (lseek(fd, offset, SEEK_SET) < 0) {
        close(fd);
        perror("lseek");
        return 1;
    }
    uint64_t entry;
    size_t n = read(fd, &entry, 8); // прочитали сырую запись адреса страницы RSS
    if (n != 8) {
        perror("read");
        return 1;
    }
    /*
    1) (1ULL << 63) — это единица на позиции 63. В шестнадцатеричной: 0x8000000000000000
       entry & (1ULL << 63) — проверяем, взведён ли (1) бит 63

    2) (1ULL << 55) - 1 — маска
       Вычитаем 1 из этого числа. Получаем: (1ULL << 55) - 1 = 0x007FFFFFFFFFFFFF
       В двоичном виде — это 55 единиц в младших битах (биты 0–54). Все старшие биты (55–63) = 0.
       Операция entry & mask:
       & — побитовое И. Обнуляет все биты, где в маске 0. Оставляет только биты 0–54.
       Таким образом, pfn получает значение только битов 0–54 из entry. Это и есть номер физической страницы.
    */
   printf("Dirty addr: 0x%lx\n", entry);
    if (entry & (1ULL << 63)) {
        uint64_t pfn = entry & ((1ULL << 55) - 1);
        printf("PFN: 0x%016lx\n", pfn);
    }
    return 0;
}

/*
После mmap: виртуальная память выделена, физической нет. В /proc/statm RSS показывает только физические страницы, которые уже отобраны.

При чтении (char c = *page):
Происходит page fault (minor fault). Ядро выделяет одну физическую страницу, заполняет её нулями, и привязывает к виртуальному адресу.
→ RSS увеличивается на 1 страницу за каждую прочитанную страницу.
У тебя их 10 → +10 страниц.

При записи (*page = 1):
Происходит ещё один page fault, но не тот, который ты думаешь.
Анонимная страница после mmap помечена как copy-on-write (COW). При первой записи ядро создаёт частную копию страницы для твоего процесса.
→ Это новая физическая страница.
→ RSS увеличивается ещё на 10 страниц.
*/

void statm() {
    long page_size = sysconf(_SC_PAGESIZE);
    printf("Mem pagesize: %ld bytes\n", page_size);
    int *mem_pages[10];
    for (int i = 0; i < 10; i++)
    {
        int *page = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (page < 0) {
            perror("mmap");
            _exit(1);
        }
        mem_pages[i] = page;
    }

    int fd = open("/proc/self/statm", O_RDONLY, 0666);
    if (fd < 0) {
        perror("open");
        _exit(1);
    }
    char buf[64];
    read(fd, buf, 64);
    close(fd);
    unsigned long vss, rss;
    sscanf(buf, "%lu %lu", &vss, &rss);
    printf("Start:\n  RSS pages: %lu\n  RSS bytes: %lu\n", rss, rss*page_size);

    

    for (int i = 0; i < 10; i++)
    {
        char c = *mem_pages[i];
    }

    fd = open("/proc/self/statm", O_RDONLY, 0666);
    if (fd < 0) {
        perror("open");
        _exit(1);
    }
    read(fd, buf, 64);
    close(fd);
    sscanf(buf, "%lu %lu", &vss, &rss);
    printf("After read:\n  RSS pages: %lu\n  RSS bytes: %lu\n", rss, rss*page_size);

    int pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
    if (pagemap_fd < 0) {
        perror("open");
        _exit(1);
    }
    for (int i = 0; i < 10; i++)
    {
        if (pagemap(pagemap_fd, page_size, mem_pages[i]) == 1) {
            perror("pagemap");
            continue;
        };
    }
    close(pagemap_fd);
    
    /*
    Почему после чтения все PFN одинаковые?
    Потому что ты читаешь нулевые страницы.
    После mmap все 10 виртуальных страниц указывают на одну и ту же физическую страницу, заполненную нулями. Это оптимизация ядра — zero page.
    При первом чтении любой из этих 10 страниц:
    Происходит minor fault
    Ядро отображает виртуальную страницу на глобальную zero page (только для чтения)
    PFN у всех 10 будет одинаковый (адрес zero page)

    Почему после записи PFN разные?
    Когда ты записываешь в страницу:
    Происходит fault, потому что zero page защищена от записи
    Ядро создаёт новую физическую страницу для каждой виртуальной страницы
    Копирует туда нули (или то, что ты записал)
    Теперь каждая виртуальная страница указывает на свою физическую
    Поэтому после записи у всех 10 страниц PFN разные.
    */
    for (int i = 0; i < 10; i++)
    {
        *mem_pages[i] = 1;
    }

    fd = open("/proc/self/statm", O_RDONLY, 0666);
    if (fd < 0) {
        perror("open");
        _exit(1);
    }
    read(fd, buf, 64);
    close(fd);
    sscanf(buf, "%lu %lu", &vss, &rss);
    printf("After write:\n  RSS pages: %lu\n  RSS bytes: %lu\n", rss, rss*page_size);

    pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
    if (pagemap_fd < 0) {
        perror("open");
        _exit(1);
    }
    for (int i = 0; i < 10; i++)
    {
        if (pagemap(pagemap_fd, page_size, mem_pages[i]) == 1) {
            perror("pagemap");
            continue;
        };
    }
    close(pagemap_fd);
    return;
}


int main() {
    statm();
    return 0;
}